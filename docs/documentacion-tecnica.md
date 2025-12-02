---
title: Documentación Técnica
permalink: /documentacion-tecnica/
layout: default
nav_order: '003'
---

- [Cliente](#cliente)
  - [Arquitectura General del Cliente](#arquitectura-general-del-cliente)
    - [Visión General](#visión-general)
    - [Diagrama de clases principal](#diagrama-de-clases-principal)
  - [Protocolo](#protocolo)
    - [Formato del Protocolo](#formato-del-protocolo)
    - [Mensajes Cliente → Servidor](#mensajes-cliente--servidor)
    - [Mensajes Servidor → Cliente](#mensajes-servidor--cliente)
  - [Ciclo de Vida de una Partida](#ciclo-de-vida-de-una-partida)
    - [Diagrama de Secuencia](#diagrama-de-secuencia)
    - [Diagrama de Estados](#diagrama-de-estados)
  - [Clases Importantes](#clases-importantes)
    - [World](#world)
    - [GameStateManager](#gamestatemanager)
    - [InputHandler](#inputhandler)
  - [Renderizado](#renderizado)
  - [Efectos Visuales](#efectos-visuales)
    - [Ciclo de vida de efectos](#ciclo-de-vida-de-efectos)
  - [Audio](#audio)

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

La clase *World* almacena la representación local del estado del juego que recibe desde el servidor mediante snapshots.

**Características clave:**
- No thread-safe (solo GameLoop accede)
- Actualización atómica por snapshot
- Inmutable para renderizado
- Datos por auto: posición, ángulo, salud, checkpoints, estado de puente

**Flujo de actualización:** Snapshot → GameLoop::applySnapshot() → World::update() → Renderers::render()

<br>

### GameStateManager
**Responsabilidad:** Gestionar las transiciones de estado del juego y los temporizadores de cada fase.

El *GameStateManager* coordina las diferentes fases de la partida, desde el countdown inicial hasta el podio final.

<br>

### InputHandler
**Responsabilidad:** capturar eventos SDL y traducirlos a comandos del juego.

El *InputHandler* actúa como intermediario entre SDL y la lógica del juego, procesando tanto input continuo (movimiento) como eventos ocasionales (clicks, cheats).

**Tipos de input manejados:**

1. **Movimiento (continuo):** WASD/flechas  
2. **Cheats:** combinaciones especiales  
3. **Controles de audio:** Ctrl+tecla  
4. **Mouse (fase modificaciones)**

<hr>

## Renderizado
**Clases:** WorldRenderer, UIRenderer, EffectsManager, MapRenderer, BridgeRenderer, CheckpointRenderer.

<p align="center">
  <img src="{{ site.baseurl }}/assets/diagramaClasesRender.png" width="900">
</p>

<hr>

## Efectos Visuales

Los efectos se implementan mediante un sistema de partículas que usa estructuras de datos simples. Cada tipo de efecto tiene:
- Una estructura de datos propia (*Particle*, *SmokeParticle*, *BrakeTrail*, etc.)
- Un contenedor en *WorldRenderer* (vector de efectos activos)
- Lógica de update que decrementa *life* y actualiza física
- Lógica de cleanup que elimina efectos

**Optimizaciones implementadas:**
- Límite de puntos en trails
- Cleanup automático de partículas muertas cada frame
- Solo se actualizan efectos con 'life > 0'

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