---
title: Documentación Técnica
permalink: /documentacion-tecnica/
layout: default
nav_order: '003'
---

<div style="display:grid; grid-template-columns: 1fr 1fr; gap: 1rem;">
  <ul style="margin:0; padding-left:1.2rem;">
    <li><a href="#arquitectura-general-del-cliente">Arquitectura General del Cliente</a></li>
    <li><a href="#protocolo">Protocolo</a></li>
    <li><a href="#ciclo-de-vida-de-una-partida">Ciclo de Vida de una Partida</a></li>
    <li><a href="#clases-importantes">Clases Importantes</a></li>
    <li><a href="#renderizado">Renderizado</a></li>
    <li><a href="#efectos-visuales">Efectos Visuales</a></li>
    <li><a href="#audio">Audio</a></li>
  </ul>

  <ul style="margin:0; padding-left:1.2rem;">
    <li><a href="#lógica-de-juego">Lógica de juego</a></li>
    <li><a href="#loop-principal">Loop principal</a></li>
    <li><a href="#colisiones-y-puentes">Colisiones y puentes</a></li>
    <li><a href="#carreras">Carreras</a></li>
    <li><a href="#npcs">NPCs</a></li>
  </ul>
</div>

<br>

---

## Arquitectura General del Cliente
<br>

### Visión General
Para el cliente se implementó una arquitectura basada en 3 threads que se comunican mediante queues:


```
    - Receiver Thread: lee mensajes del servidor -> push a serverMessagesQueue
    - GameLoop Thread: pop mensajes -> actualiza modelo -> renderiza con SDL -> captura input -> push comandos a clientCommandQueue
    - Sender Thread: pop comandos -> envía al servidor
```
<br>

### Diagrama de clases principal


<p align="center">
  <img src="{{ site.baseurl }}/assets/diagramaClases.png" width="900">
</p>

<hr>

## Protocolo
<br>

### Formato del Protocolo
El protocolo es binario y utiliza network byte order (big-endian) para enteros mayores a 8 bits.

Tipos base:

```
    - uint8_t: 1 byte, sin conversión
    - uint16_t: 2 bytes, convertidos con htons/ntohs
    - uint32_t: 4 bytes, convertidos con htonl/ntohl
    - string: uint16_t length + bytes
```
<br>

### Mensajes Cliente → Servidor

| Mensaje              | Código | Contenido                                           |
|----------------------|--------|-----------------------------------------------------|
| **SEND_MOVE_STATE**  | 0x09   | uint8_t moveState (bitmask: UP\|DOWN\|LEFT\|RIGHT)  |
| **MSG_MODIFY_CAR**   | 0x0D   | 4x uint8_t (speedMod, healthMod, accelMod, massMod) |
| **SEND_INMORTALITY** | 0xFF   | solo código                                         |
| **SEND_INSTA_WIN**   | 0xFE   | solo código                                         |
| **SEND_INSTA_LOSE**  | 0xFD   | solo código                                         |
| **SEND_SUPER_SPEED** | 0xFC   | solo código                                         |

<br>
### Mensajes Servidor → Cliente

| Mensaje                 | Código | Contenido                     |
|-------------------------|--------|-------------------------------|
| **SEND_RACE_SNAPSHOT**  | 0x0A   | Estado completo del juego     |
| **MSG_COUNTDOWN**       | 0x12   | uint8_t: número del countdown |
| **MSG_RACE_INFO**       | 0x14   | mapId, race, totalRaces       |
| **MSG_RACE_START**      | 0x13   | Señal de inicio               |
| **MSG_RACE_END**        | 0x17   | Resultados de la carrera      |
| **MSG_STATS_COUNTDOWN** | 0x1A   | uint8_t: countdown stats      |
| **MSG_MOD_PHASE**       | 0x18   | Propiedades de autos          |
| **MSG_MOD_COUNTDOWN**   | 0x1B   | uint8_t: countdown mods       |
| **MSG_GAME_END**        | 0x19   | Resultados finales            |
| **MSG_COLLISION**       | 0x15   | playerId, intensity, x, y     |
| **MSG_PLAYER_DIED**     | 0x16   | uint16_t playerId             |

<hr>

## Ciclo de vida de una partida

<br>
### Diagrama de Secuencia

<p align="center">
  <img src="{{ site.baseurl }}/assets/diagramaSecuencia.png" width="900">
</p>
<br>

### Diagrama de Estados

<p align="center">
  <img src="{{ site.baseurl }}/assets/diagramaEstados.png" style="max-width:600px; height:auto;">
</p>

<hr>

## Clases Importantes
<br>
### World
**Responsabilidad:** Mantener el estado local del mundo del juego.

La clase `World` almacena la representación local del estado del juego que recibe desde el servidor mediante snapshots.

**Características clave:**
- No thread-safe (solo GameLoop accede)
- Actualización atómica por snapshot
- Inmutable para renderizado
- Datos por auto: posición, ángulo, salud, checkpoints, estado de puente

**Flujo de actualización:** `Snapshot` → `GameLoop::applySnapshot()` → `World::update()` → `Renderers::render()`

<br>

### GameStateManager
**Responsabilidad:** Gestionar las transiciones de estado del juego y los temporizadores de cada fase.

El `GameStateManager` coordina las diferentes fases de la partida, desde el countdown inicial hasta el podio final.

<br>

### InputHandler
**Responsabilidad:** capturar eventos SDL y traducirlos a comandos del juego.

El `InputHandler` actúa como intermediario entre SDL y la lógica del juego, procesando tanto input continuo (movimiento) como eventos ocasionales (clicks, cheats).

**Tipos de input manejados:**

1. **Movimiento (continuo):** WASD/flechas  
2. **Cheats:** combinaciones especiales  
3. **Controles de audio:** Ctrl+tecla  
4. **Mouse (fase modificaciones)**

<hr>

## Renderizado
**Clases:** `WorldRenderer`, `UIRenderer`, `EffectsManager`, `MapRenderer`, `BridgeRenderer`, `CheckpointRenderer`.

<p align="center">
  <img src="{{ site.baseurl }}/assets/diagramaClasesRender.png" width="900">
</p>

<hr>

## Efectos Visuales

Los efectos se implementan mediante un sistema de partículas que usa estructuras de datos simples. Cada tipo de efecto tiene:
- Una estructura de datos propia (*Particle*, *SmokeParticle*, *BrakeTrail*, etc.)
- Un contenedor en `WorldRenderer` (vector de efectos activos)
- Lógica de update que decrementa *life* y actualiza física
- Lógica de cleanup que elimina efectos

**Optimizaciones implementadas:**
- Límite de puntos en trails
- Cleanup automático de partículas muertas cada frame
- Solo se actualizan efectos con `life > 0`

### Ciclo de vida de efectos

**Explosión (cuando muere un auto):**
- Crea 30–50 partículas con velocidad radial  
- Colores: rojo, naranja, amarillo (aleatorio)  
- Gravedad hacia abajo  
- Duración aproximada: 2 segundos  

**Humo de aceleración:**
- Se crea continuamente mientras W está presionado  
- Sale desde la parte de atrás del auto  
- Dirección opuesta al movimiento  
- Se expande y se desvanece  

**Marcas de freno (brake trails):**
- Dos líneas (ruedas) de puntos  
- Desvanecimiento gradual (≈ 3 segundos)  
- Ancho fijo de 6 px entre líneas  

**Flash de colisión:**
- Círculo blanco expandiéndose  
- Intensidad basada en la fuerza del impacto  
- Duración: 0.3 segundos  

<hr>

## Audio

El sistema de audio proporciona control de volumen independiente, throttling de sonidos, y manejo de loops especiales para efectos continuos.

1. **Música de fondo (Music):**
   - Un solo track activo a la vez
   - Loop infinito mientras dura la carrera
   - Volumen independiente (default: 64/128)
   - Se detiene al finalizar carrera o morir

2. **Efectos de sonido (Chunks):**
   - 16 canales disponibles para reproducción simultánea
   - Volumen independiente (default: 128/128)
   - Dos canales dedicados para loops especiales:
     - *engineChannel*: Motor (mientras W está presionado)
     - *brakeChannel*: Freno (mientras S está presionado)

**Sistema de Throttling:** para evitar saturacion de audio (ej: 100 colisiones en 1 segundo), se implementa un throttling por nombre de sonido.

**Volumen por distancia:** aunque no se implementó completamente, el sistema soporta modular volumen según distancia al evento.

<hr>

# Lógica de juego
<br>
La lógica del juego se encuentra en el thread *Game*, que se encarga de orquestar las distintas etapas de juego y la simulación de físicas en Box2D. En este thread se pueden encontrar las tres clases mas importantes: el propio *Game*, *Player* y *Car*. Cada una separa las simulaciones y el manejo de la lógica de juego en diferentes capas de abstracción, siendo *Car* la encargada de simular directamente las físicas de los autos de cada jugador.

<p align="center">
  <img src="{{ site.baseurl }}/assets/clasesPrincipalesSv.png" width="900">
</p>

<hr>

## Loop principal

```cpp
while (shouldKeepRunning()) {
  updateGameState();

  uint64_t deltaIt = it - lastIt;
  while (deltaIt-- > 0)
    handleGameState();
  broadcast();
  lastIt = it;

  // CRL algorithm
  it = crl.sleepAndCalcIt();
}
```

En el loop principal podemos ver las tres funciones clave que nos permiten manejar el flujo del juego: `updateGameState()`, `handleGameState()` y `broadcast()`. Esta estructura es muy útil, ya que la comunicación entre cliente y servidor consiste principalmente en dos tipos de mensajes:

- Se envía un mensaje al cliente cuando el estado de juego cambia
- Se envía un mensaje en cada *frame* para actualizar el estado del juego

`updateGameState()` es el encargado de verificar en cada loop si el estado de juego debe ser cambiado y hacerlo si es necesario. De ser así, se llama al *setter* del estado en cuestión y se manda un broadcast que indica el cambio de estado, acompañado con la información que sea necesaria (como puede ser las estadísticas de la carrera o las modificaciones posibles).

`handleGameState()` lleva a cabo las tareas de los dos estados que lo necesitan: la fase de carrera y la fase de modificaciones. Ambas usan el patrón comando, y hacen un *pop* de la cola de comandos de cliente para ejecutar las peticiones necesarias (que pueden ser peticiones de movimiento o de mejora de auto).

`broadcast()` es quien crea los *snapshots* que se van a mandar al cliente en cada *frame*. Cada fase tiene su función de snapshot, ya que algunas como el countdown, la fase de estadísticas y la fase de modificaciones solo mandan el tiempo restante, mientras que la fase de carrera manda la información pertinente de cada jugador, como su vida, la posición y sprite de su auto, su estado de carrera, entre otros. Además es la encargada de avisarle al cliente cuando choca o muere un jugador para activar los sonidos y animaciones.

Todo esto y la simulación de físicas se hace en tiempo constante usando el algoritmo de [*constant rate loop*](https://book-of-gehn.github.io/articles/2019/10/23/Constant-Rate-Loop.html).

<hr>

## Colisiones y puentes

Cada *fixture* de Box2D tiene un atributo *filter* donde se puede guardar su categoría (a qué capa de colisión pertenece) y su mascara (contra qué categorías se puede chocar). Teniendo esto en cuenta, cada auto se crea usando un `CarBuilder::createCar` que *settea* los autos en la capa baja de colisión para que luego los sensores de cambio de capa lo puedan modificar.

Tanto estos sensores como las colisiones del mapa se cargan a partir de archivos yaml que guardan la posición central de cada caja de colisión junto con su anchura y altura. Estos archivos guardan la información en píxeles, por lo que se debe traducir luego a coordenadas de Box2D.

El cargado de sensores y colisiones se hacen mediante `CollisionLoader::loadCollisions`, que devuelve los datos de colisiones a introducir en `CollisionGenerator::generateCollisions`, que finalmente crea esas colisiones en el *b2World* recibido. Esta última función permite que las cajas de colisión se interpreten como sensores mediante el uso de la variable *sensorId*.
 
Finalmente, el manejo de la lógica de colisiones se hace mediante un `ContactListener` que permite controlar los contactos usando tres funciones principales: `BeginContact()`, `EndContact()` y `PostResolve()`. La instancia de esta clase vive en el *scope* del juego y guarda un puntero a *Game*, que permite llamar a `Game::handleCollision` de forma sencilla.

<p align="center">
  <img src="{{ site.baseurl }}/assets/comunicacion.png" width="300">
</p>

Sin embargo esto trae un problema, y es que *ContactListener* solo devuelve los cuerpos que participaron en las colisiones, pero no las clases *Player* y *Car* que son las encargadas de manejar el resto de la lógica de los choques. Para esto el `b2Body* body` del auto guarda el *Player* para poder pasarle al juego quién fue el que chocó contra una colisión o accionó un sensor. Esta asignación se hace dentro del constructor de *Player*. De forma similar, los sensores guardan su ID de sensor, ya que existen dos tipos de sensores (*toggle* de capa y checkpoints de carrera) que tienen comportamientos distintos. Estos datos se guardan dentro de un pointer a `struct BodyData`.

<p align="center">
  <img src="{{ site.baseurl }}/assets/flujo.png" width="900">
</p>

El atributo PathElement corresponde a la fase de carreras.

<hr>

## Carreras

La fase de carreras tiene dos aspectos principales. Uno es el manejo de físicas, el otro son las carreras en sí.
### Físicas
Como se mencionó anteriormente el *gameloop* llama al controlador del estado de juego, que durante la etapa de carrera corresponde a `handleRacingState()`

```cpp
void Game::handleRacingState() {
  std::unique_ptr<Command> cmd;
  while (clientCommandsQueue.tryPop(cmd)) {
    cmd->execute(*this);
  }
  updatePlayerCars();

  world->Step(TIME_STEP, velocityIt, positionIt);
}
```

Este lleva a cabo la simulación en tres pasos:
- Actualiza el movimiento actual del jugador usando el patrón comando.
- Actualiza las físicas del auto acorde a ese movimiento.
- Simula las físicas en el mundo de Box2D.
  
El paso más complejo es el de simular las físicas, que implica cuatro funciones principales que se encargan de que el comportamiento del auto sea mínimamente realista: `applyFriction()`, `applyThrottle()`, `applySteering()` y `applySpeedLimits()`.

<p align="center">
  <img src="{{ site.baseurl }}/assets/fisicas.png" width="500">
</p>

`applyFriction()` se encarga de controlar tanto la velocidad lateral como la frontal. Controla la velocidad lateral haciendo que el auto no pueda ir de costado o rotando sobre su propio eje, y la frontal haciendo que se frene con el tiempo.

`applyFriction()` aplica fuerzas en el sentido en el que mira el auto, procurando que la aceleración se sienta y que no sea un movimiento súbito. Además reduce la velocidad al ir en reversa.

`applySteering()` controla el giro del auto, que es la parte más compleja para representar el movimiento de un auto. Le aplica un torque al auto para simular un giro de volante y procura que el sentido de giro sea el mismo sin importar si el auto va hacia adelante o hacia atrás, como en un auto real.

`applySpeedLimits()` es la última función que se aplica, y su tarea es restringir los movimientos lineales y angulares del auto de forma que no patine sobre el piso y no supere las velocidades máximas que puede tener un auto. 

<br>

### Recorridos

Por otro lado los recorridos se cargan de forma similar a las colisiones, pero usando un `PathLoader` y un `PathGenerator`. Al comienzo de cada carrera en la función `setCountdownState()` se cargan todos los datos necesarios (colisiones, recorridos y NPCs) y se reinicia el estado de todos los jugadores, recuperando su vida y empezando de cero la carrera. En esta función se llama a estas dos funciones que le permiten a los jugadores guardarse el recorrido como un `struct Path`. Esta variable almacena el recorrido que se va a ir modificando a medida que el jugador avanza, y se va a mandar al cliente para que lo renderice.

Los recorridos se eligen aleatoriamente al momento de empezar la partida siempre y cuando el número de carreras elegido sea igual o inferior a la cantidad de recorridos disponibles. Una vez empieza el recorrido los autos aparecen en las ocho posiciones indicadas por el usuario en el editor.

Similar a los choques, la lógica de los checkpoints comienza en el *ContactListener* cuando se escucha un contacto entre un jugador y un sensor de checkpoint, que luego permite llamar al controlador pertinente.

<p align="center">
  <img src="{{ site.baseurl }}/assets/recorridos.png" width="500">
</p>

Al llegar la señal al jugador, este se encarga de verificar si el checkpoint tocado es el que le corresponde (ya que no debe saltarse checkpoints en la carrera), borra el checkpoint y todos los hints que llegan hasta él y calcula el próximo checkpoint en su recorrido.

En ese momento si termina la carrera se guarda su tiempo de finalización. Esto también sucede cuando un jugador muere o termina por tiempo límite, y en ambos casos se guarda el tiempo máximo que puede durar la carrera. También cabe aclarar que las carreras terminan una vez que el último jugador termina de alguna de estas tres maneras.

<hr>

# NPCs

Un NPC no es más que un *Player* con un ID mayor a la cantidad de jugadores que se pueden unir a la misma partida. Esto permite que los NPCs hereden los comportamientos de los jugadores, como chocarse entre sí, poder pasar por arriba y por abajo de puentes y usar la misma implementación de físicas que los jugadores. Esta decisión de diseño tiene en cuenta factores como que no se debe incluir a los NPCs en las pantallas de estadísticas o de finalización del juego, o que no se debe esperar a que un NPC termine la carrera para seguir con la siguiente, por ejemplo.

Actualmente el único NPC aparece en el mapa de Vice City, rondando por la parte superior del mapa.

<p align="center">
  <img src="{{ site.baseurl }}/assets/nodos.png" width="900">
</p>

Al momento de recorrer los jugadores para cargar los recorridos, si se encuentra uno con un ID superior a la cantidad maxima de jugadores se le asigna un `Graph` obtenido usando `GraphLoader` para un *yaml* de nodos y aristas. No se usan sensores de Box2D para esta implementación, por lo que no se cuenta con un `GraphGenerator`. El grafo se carga siempre y cuando el mapa sea Vice City usando al función `initCurrentGraph()` que le da su posición inicial sobre el primer nodo cargado (está comentada la linea para hacerlo aleatorio, pero se optó por la implementación determinística para que el comportamiento del NPC sea predecible), se le asigna el próximo nodo y se le da la lógica de NPC con el flag *isNPC*. Esto también permite que, si el desarrollador lo desea, se descomente las lineas en setCountdownState() que hacen que el jugador pueda controlar el NPC, siguiéndolo con la camara y facilitando las tareas de debug y ampliación de la funcionalidad.

Una vez inicializado el NPC se lo va a controlar usando la función `updateNPCDirections()`, que se encuentra dentro de la función del controlador del estado de carrera.

```cpp
void Game::updatePlayerCars() {
	for (auto& [id, player]: players) {
		player.updateNPCDirections();
		player.updateCarPhysics();
	}
}
```

`updateNPCDirections()` aprovecha el uso del `struct ActiveDirections` por parte del usuario, que simplemente guarda con 4 booleanos el estado actual del teclado. Lo que hace es calcular a partir de su posición y la del nodo objetivo los giros que tiene que hacer. Cabe aclarar que los NPCs aceleran constantemente, y para evitar que se comporten de forma inesperada se instancian con su velocidad cinco veces inferior a la de un jugador normal.

<p align="center">
  <img src="{{ site.baseurl }}/assets/npcs.png" width="700">
</p>

Una vez que se actualiza el estado actual del movimiento, el auto se encarga solo de moverse usando las físicas ya implementadas para el resto de jugadores.

También cabe aclarar que `chooseNextNode()` selecciona un nodo al azar entre los vecinos del nodo al que llegó pero ignorando el nodo del que acaba de venir.