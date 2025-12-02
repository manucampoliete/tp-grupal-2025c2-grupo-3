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
    <li><a href="#clases-importantes">Clases Importantes del Cliente</a></li>
    <li><a href="#renderizado">Renderizado</a></li>
    <li><a href="#efectos-visuales">Efectos Visuales</a></li>
    <li><a href="#audio">Audio</a></li>
    <li><a href="#lógica-de-juego">Lógica de juego</a></li>
  </ul>

  <ul style="margin:0; padding-left:1.2rem;">
    <li><a href="#loop-principal">Loop principal</a></li>
    <li><a href="#colisiones-y-puentes">Colisiones y puentes</a></li>
    <li><a href="#carreras">Carreras</a></li>
    <li><a href="#npcs">NPCs</a></li>
    <li><a href="#hilos-del-servidor">Hilos del servidor</a></li>
    <li><a href="#diagramas-de-clase-complementarios">Diagramas de clase complementarios</a></li>
    <li><a href="#códigos-complementarios">Códigos complementarios</a></li>
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

### Tabla con mensajes de protocolo

| Nombre              | Descripción                                                                                             | Código |
|---------------------|---------------------------------------------------------------------------------------------------------|--------|
| SEND_CLIENT_ID      | Servidor manda al cliente su ID                                                                         | 0x01   |
| SEND_INITIAL_INFO   | Cliente pide al servidor info de los autos para renderizar la lobby                                     | 0x02   |
| SEND_INITIAL_INFO   | Servidor devuelve la info de los autos para renderizar la lobby                                         | 0x02   |
| SEND_CREATE         | Cliente pide al servidor crear una partida                                                              | 0x03   |
| SEND_CREATED        | Servidor responde con el ID de la partida creada                                                        | 0x04   |
| SEND_JOIN           | Cliente pide unirse a una partida                                                                       | 0x05   |
| SEND_JOINED         | Servidor informa si el join fue exitoso                                                                 | 0x06   |
| SEND_START          | Cliente pide al servidor empezar una partida                                                            | 0x07   |
| SEND_STARTED        | Servidor envia al cliente una senial de que la partida comenzo                                          | 0x08   |
| SEND_MOVE_STATE     | Cliente envía su estado de movimiento actual                                                            | 0x09   |
| SEND_RACE_SNAPSHOT  | Servidor envía un snapshot del estado del juego                                                         | 0x0A   |
| SEND_INMORTALITY    | Cliente pide activar cheat de inmortalidad (salud infinita)                                             | 0xFF   |
| SEND_INSTA_WIN      | Cliente pide activar cheat de victoria instantánea                                                      | 0xFE   |
| SEND_INSTA_LOSE     | Cliente pide activar cheat de derrota instantánea                                                       | 0xFD   |
| SEND_SUPER_SPEED    | Cliente pide activar cheat de súper velocidad                                                           | 0xFC   |
| MSG_MODIFY_CAR      | Cliente pide modificar propiedades entre carreras                                                       | 0x0D   |
| MSG_RACE_INFO       | Servidor envía info de la próxima carrera                                                               | 0x11   |
| MSG_COUNTDOWN       | Servidor envía cuenta regresiva para alguna etapa                                                       | 0x12   |
| MSG_RACE_START      | Servidor avisa que la carrera comenzó                                                                   | 0x13   |
| MSG_COLLISION       | Servidor informa detalles de una colisión                                                               | 0x15   |
| MSG_PLAYER_DIED     | Servidor informa que un jugador murió                                                                   | 0x16   |
| MSG_RACE_END        | Servidor avisa que la carrera terminó                                                                   | 0x17   |
| MSG_STATS_COUNTDOWN | Servidor envía cuenta regresiva para la etapa de estadísticas                                           | 0x1A   |
| MSG_MOD_PHASE       | Servidor envía valores actuales de propiedades que el jugador puede mejorar                             | 0x18   |
| MSG_MOD_COUNTDOWN   | Servidor envía cuenta regresiva para la etapa de modificaciones                                         | 0x1B   |
| MSG_GAME_END        | Servidor avisa que la partida terminó (ya hay un ganador)                                               | 0x19   |

<hr>

## Desglose de los mensajes

<br>

### Cliente -> Servidor

SEND_INITIAL_INFO  
- 1 byte con el literal 0x02  

SEND_CREATE `<username>` `<car-id>`  
- SEND_CREATE: un byte con el literal 0x03  
- `<username>`: string en network order  

SEND_JOIN `<match-id>` `<username>` `<car-id>`  
- SEND_JOIN: un byte con el literal 0x05  
- `<match-id>`: 2 bytes en network order  
- `<username>`: string en network order  
- `<car-id>`: 1 byte con el coche a utilizar (0x00--0x06)  

SEND_START  
- SEND_START: un byte con el literal 0x07  

SEND_MOVE_STATE `<directions-byte>`  
- SEND_MOVE_STATE: un byte con el literal 0x09  
- `<directions-byte>`: 1 byte que codifica en los últimos 4 bits, las direcciones de movimiento activas del cliente (en el orden Up, Down, Left, Right). Ejemplo: el byte 0b0000**1**0**1**0 codifica dos direcciones de movimiento activas: Up y Left.  

SEND_INMORTALITY  
- SEND_INMORTALITY: un byte con el literal 0xFF  

SEND_INSTA_WIN  
- SEND_INSTA_WIN: un byte con el literal 0xFE  

SEND_INSTA_LOSE  
- SEND_INSTA_LOSE: un byte con el literal 0xFD  

SEND_SUPER_SPEED  
- SEND_SUPER_SPEED: un byte con el literal 0xFC  

MSG_MODIFY_CAR `<speed-mod>` `<health-mod>` `<accel-mod>` `<mass-mod>`  
- MSG_MODIFY_CAR: un byte con el literal 0xFC  
- `<speed-mod>`: 1 byte con el literal 0x01 si se quiere activar la modificación del atributo, o con el literal 0x00 si no se quiere.  
- `<health-mod>`: ídem.  
- `<accel-mod>`: ídem.  
- `<mass-mod>`: ídem.  

### Servidor ->` Cliente

SEND_CLIENT_ID `<client-id>`  
- SEND_CLIENT_ID: un byte con el literal 0x01  
- `<client-id>`: 2 bytes en network order  

SEND_INITIAL_INFO `<n>` `<id-1>` `<name-1>` `<max-speed-1>` `<health-1>` ... `<id-n>` `<name-n>` `<max-speed-n>` `<health-n>`  
- SEND_INITIAL_INFO: un byte con el literal 0x02  
- `<n>`: 1 byte cantidad de autos disponibles en el juego  
- `<id-i>`: 1 byte con el id del coche i (por ahora los disponibles son 0x00-0x06)  
- `<name-i>`: string en network order con el nombre del coche i  
- `<max-speed-i>`: 2 bytes en network order con la velocidad maxima del coche i  
- `<health-i>`: 2 bytes en network order con la salud maxima del coche i  

SEND_CREATED `<match-id>`  
- SEND_CREATED: un byte con el literal 0x04  
- `<match-id>`: 2 bytes en network order con el id de la nueva partida  

SEND_JOINED `<join-status>`  
- SEND_JOINED: un byte con el literal 0x06  
- `<join-stats>`: 1 byte con el literal 0x01 en error, 0x00 en caso de éxito.  

SEND_STARTED  
- SEND_STARTED: un byte con el literal 0x08  

SEND_RACE_SNAPSHOT `<countdown>` `<n>` `<snapshot-1>` ... `<snapshot-n>`  
- SEND_RACE_SNAPSHOT: un byte con el literal 0x0A  
- `<countdown>`: 4 bytes en network order con el tiempo restante de carrera  
- `<n>`: 1 byte con la cantidad de jugadores jugando la partida (máximo 8 por ahora)  
- `<snapshot-i>` = `<client-id-i>` `<x-i>` `<y-i>` `<angle-i>` `<speed-i>` `<car-id-i>` `<health-i>` `<on-bridge-i>` `<m-i>` `<path-info-i-1>` ... `<path-info-i-m>`  
  - `<client-id>`: 2 bytes en network order con el id del cliente/jugador i  
  - `<x-i>` / `<y-i>`: 4 bytes en network order con las coordenadas (x, y) del cliente/jugador i  
  - `<angle-i>` / `<speed-i>`: 2 bytes en network order con el angulo y la velocidad, respectivamente, del cliente/jugador i  
  - `<car-id-i>`: 1 byte con el código de auto elegido por el cliente/jugador i (0x00-0x06)  
  - `<health-i>`: 2 bytes en network order con la salud actual del coche del cliente/jugador i  
  - `<on-bridge>`: 1 byte con el flag 0x01 o 0x00 dependiendo de si el coche del cliente/jugador i está arriba de un puente o no  
  - `<m>`: 1 byte con la cantidad de 'path elements' del cliente/jugador i  
  - `<path-info-i-j>` = `<path-id-i-j>` `<path-x-i-j>` `<path-y-i-j>`  
    - `<path-id-i-j>`: 1 byte con el id de 'path element' nro j del cliente/jugador i  
    - `<path-x-i-j>` / `<path-y-i-j>`: 4 bytes en network order para las coordenadas (x, y) del 'path element' nro j del cliente/jugador i  

MSG_RACE_INFO `<map-id>` `<race>` `<total-race>`  
  - MSG_RACE_INFO: un byte con el literal 0x11  
  - `<map-id>`: 1 byte con el código de mapa a utilizar (0x00-0x02)  
  - `<race>`: 1 byte con el indice de carrera actual  
  - `<total-race>`: 1 byte con la cantidad total de carreras  
    
**Nota:** en la UI del cliente se muestra durante la carrera "`<race>` / `<total-race>`" arriba a la derecha de la screen SDL (así sabemos en qué carrera estamos parados y cuántas faltan para terminar la partida).  

MSG_COUNTDOWN `<countdown>`  
- MSG_COUNTDOWN: un byte con el literal 0x12  
- `<countdown>`: 1 byte con la cuenta regresiva para la etapa de countdown previa al inicio de una carrera  

MSG_RACE_START  
- MSG_RACE_START: un byte con el literal 0x13  

MSG_COLLISION `<player-id>` `<intensity>` `<x>` `<y>`  
- MSG_COLLISION: un byte con el literal 0x15  
- `<player-id>`: 2 bytes en network order con el id del jugador que colisionó  
- `<intensity>`: 1 byte con la intensidad (la intensidad dentro de la lógica del juego que vale entre 0 y 1, se convierte a un valor entre 0 y 255)  
- `<x>` / `<y>`: 4 bytes en network order con las coordenadas (x, y) que marcan el punto donde se produjo la colisión  

MSG_PLAYER_DIED `<player-id>`  
- MSG_PLAYER_DIED: un byte con el literal 0x16  
- `<player-id>`: 2 bytes en network order con el id del cliente/jugador que murió  

MSG_RACE_END `<n>` `<player-name-1>` `<race-time-ms-1>` `<total-time-ms-1>` `<player-name-n>` `<race-time-ms-n>` `<total-time-ms-n>`  
- MSG_RACE_END: un byte con el literal 0x17  
- `<n>`: 1 byte con la cantidad de jugadores  
- `<player-name-i>`: string en network order con el nombre del cliente/jugador i  
- `<race-time-ms-i>`: 4 bytes en network order con el tiempo - parcial - en milisegundos en el que el cliente/jugador terminó la última carrera jugada  
- `<total-time-ms-i>`: 4 bytes en network order con el tiempo - total - en milisegundos que se computa como la suma de los tiempos parciales de todas las carreras jugadas hasta el momento  

MSG_STATS_COUNTDOWN `<countdown>`  
- MSG_STATS_COUNTDOWN: un byte con el literal 0x1A  
- `<countdown>`: 1 byte con la cuenta regresiva para la etapa de estadisticas  

MSG_MOD_PHASE `<n>` `<mod-block-1>` ... `<mod-block-n>`  
- MSG_MOD_PHASE: un byte con el literal 0x18  
- `<n>`: 1 byte con la cantidad de jugadores  
- `<mod-block-i>` = `<player-id-i>` `<speed-i>` `<health-i>` `<acceleration-i>` `<mass-i>`  
  - `<player-id-i>`: 2 bytes en network order con el id del cliente/jugador i  
  - `<speed-i>`: 2 bytes en network order con el valor actual de velocidad máxima del cliente/jugador i  
  - `<health-i>`: ídem pero para la salud máxima  
  - `<acceleration-i>`: ídem pero para la aceleración máxima  
  - `<mass-i>`: ídem pero para la masa máxima  

MSG_MOD_COUNTDOWN `<countdown>`  
- MSG_MOD_COUNTDOWN: un byte con el literal 0x1B  
- `<countdown>`: 1 byte con la cuenta regresiva para la etapa de modificaciones  

MSG_GAME_END `<n>` `<game-end-block-1>` ... `<game-end-block-n>` `<winner-id>` `<winner-name>`  
- MSG_GAME_END: un byte con el literal 0x19  
- `<n>`: 1 byte con la cantidad de jugadores  
- `<game-end-block-i>` = `<player-id-i>` `<player-name-i>` `<total-time-ms-i>` `<position-i>`  
  - `<player-id-i>`: 2 bytes en network order con el id del cliente/jugador i  
  - `<player-name-i>`: string en network order con el nombre del cliente/jugador i  
  - `<total-time-ms-i>`: 4 bytes en network order con el tiempo - total - en milisegundos que se computa como la suma de los tiempos parciales de todas las carreras de la partida para el cliente/jugador i  
  - `<position-i>`: 1 byte con la posición del cliente/jugador en la tabla final de resultados  
- `<winner-id>`: 2 bytes en network order con el id del ganador de la partida  
- `<winner-name>`: string en network order con el nombre del ganador de la partida 

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

## Clases Importantes del Cliente
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

## Lógica de juego
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

## NPCs

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

<hr>

## Hilos del Servidor

El servidor tiene 2 + 2N + M hilos corriendo en un momento arbitrario, siendo: **(*)**
- N: la cantidad de clientes conectados en dicho momento
- M: la cantidad de partidas que se están jugando en dicho momento

**(*)** En realidad esto no es del todo correcto. Se explicará posteriormente.

<br>

## Hilo principal

El primer hilo en lanzarse es el hilo main. Este es lanzado por defecto al ejecutar el binario del servidor. Este hilo lo único que hace es bloquearse esperando por una 'q'. De llegar una 'q' por stdin en cualquier momento, el servidor procederá a cerrarse.

### Hilo aceptador (Objeto que lo encapsula: Acceptor)

Este hilo es lanzado por el hilo main, antes de que este último se bloquee. Este será el encargado de recibir nuevas conexiones entrantes. Código simplificado de `Acceptor::run()`:

```cpp
while (shouldKeepRunning()) {
    Socket peer = acceptor.accept();
    auto c = std::make_unique`<ClientHandler>`(std::move(peer), ...);
    reapDead();
    clients.push_back(std::move(c));
}
```

Por cada conexión entrante, `Socket::accept()` se desbloqueará y devolverá un Socket (que se utilizará para la comunicación con el cliente tanto en la etapa de lobby como en la etapa de juego).

<br>

## Hilos manejadores de cliente

### Etapa de Lobby: 1 hilo manejador de cliente (Objeto que lo encapsula: ClientHandler)

Como se ve en el snippet de código previo, por cada conexión entrante se crea un ClientHandler y se le pasa el Socket por movimiento. Este objeto será el que encapsule el thread en el cual se manejará la etapa de lobby. Véase en `ClientHandler::run`() la llamada a `ClientHanlder::handleLobbyPhase()`. La comunicación se hará a través de un ServerLobbyProtocol (que internamente guarda una referencia al Socket devuelto por `Socket::accept())`.

### Etapa de Game: 2 hilos manejadores de cliente (Objetos que los encapsulan: ClientHandler y Sender)

Para la etapa de juego necesitamos 2 hilos, un hilo recibidor y un hilo enviador (para lograr comunicación asincrónica con el cliente). Podríamos estar tentados a lanzar dichos hilos, pero hay que tener en cuenta que ya contamos con uno de esos 2 recursos (hilos). El hilo que encapsula el ClientHandler sigue vivo, y podemos seguir utilizándolo. Esa fue la decisión de diseño que tomamos. El hilo encapsulado por el ClientHandler se sigue usando en el contexto del objeto Receiver (véase que no es un objeto activo). Luego, sí tenemos que lanzar un 2do hilo adicional. Este será lanzado por medio del objeto Sender, que sí es un objeto activo. De esta manera logramos la comunicación asincrónica. Código simplificado de ClientHandler::run():

```cpp
while(shouldKeepRunning()) {
    handleLobbyPhase();
    if (not shouldKeepRunning())
        return;
    sender.emplace(...);    // <- Se lanza el hilo adicional
    receiver.emplace(...);  // <- Se sigue usando el recurso (hilo) pre-existente 
}
```

Nótese lo RAII de este pseudocódigo. El `Thread::start()` del hilo Sender queda encapsulado en su constructor.

Aclaración: más arriba se expresa que no es correcto que la cantidad de hilos en un momento arbitrario es 2 + 2N + M. Se verá ahora que existe la posibilidad de que el cliente aún se encuentre en etapa de lobby, y por lo tanto tendrá 1 y no 2 hilos manejadores en el servidor. Se podría decir entonces que si H es la cantidad de hilos en un momento arbitrario del servidor, entonces:

2 + N + M <= H <= 2 + 2N + M

Siendo:
- N: la cantidad de clientes conectados en dicho momento
- M: la cantidad de partidas que se están jugando en dicho momento

<br>

## Hilos de partida (Objeto que lo encapsula: Game)

Cada uno de estos hilos correrá una partida. Pseudocódigo de Game::run():

```cpp
while (shouldKeepRunning()) {
    updateGameState();
    handleGameState();
    broadcast();
    sleepAndCalcIt();  // Constant Rate Loop Algorithm
}
```

¿Cómo se corre una partida a nivel hilos? Ya se ha visto que durante la etapa de lobby tenemos 1 hilo manejador de cliente. Este hilo recibirá por protocolo las peticiones para:
- Crear una partida
- Unirse a una partida
- **Empezar una partida**

Cuando se mande una petición para empezar una partida, el hilo del ClientHandler accederá al monitor de partidas (MatchesMapMonitor) a través de un LobbyResolver, y el resultado final será una llamada a `Thread::start()`, que lanzará un nuevo hilo que ejecutará el código de `Game::run()`.

Lo explicado anteriormente se podrá visualizar correctamente en el siguiente diagrama de hilos. Véase que el diagrama corresponde a clientes que ya terminaron la etapa de lobby, y están en etapa de partida. En el caso del diagrama se cumple que la cantidad de threads H es 2 (main y aceptador) + 2 * 4 (un sender y un receiver por cada uno de los 4 clientes) + 2 (partidas jugándose) = 12.

<p align="center">
  <img src="{{ site.baseurl }}/assets/ThreadsDiagram.svg" width="900">
</p>

<hr>

## Diagramas de Clase complementarios

### Manejo de clientes

<p align="center">
  <img src="{{ site.baseurl }}/assets/ClientHandling.png" width="900">
</p>

### Manejo de partidas

<p align="center">
  <img src="{{ site.baseurl }}/assets/MatchHandling.png" width="300">
</p>

<hr>

## Códigos complementarios

| Nombre de Coche | Código |
|-----------------|--------|
| Jeep Wrangler   | 0x00   |
| Ferrari F40     | 0x01   |
| BMW Z4          | 0x02   |
| VW Beetle       | 0x03   |
| Ford Bronco     | 0x04   |
| Ford F100       | 0x05   |
| MB S-Class      | 0x06   |

| Nombre de Mapa | Código |
|----------------|--------|
| Liberty City   | 0x00   |
| San Andreas    | 0x01   |
| Vice City      | 0x02   |