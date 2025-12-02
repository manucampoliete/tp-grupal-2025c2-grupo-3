---
title: Manual del Usuario
permalink: /manual-usuario/
layout: default
nav_order: '002'
---

Bievenido al manual de usuario del **Need for Speed 2D**. Acá te explicamos cómo clonar el repositorio, instalar el proyecto y ejecutar los programas para jugar. Se recomienda utilizar un sistema operativo basado en Ubuntu 24.04.

<hr>

- [Instalación](#instalacion)
- [Ejecución e inicio de partida](#ejecución-e-inicio-de-partida)
- [Juego](#juego)
  - [Reglas](#reglas)
  - [Controles](#controles)
- [Editor de Mapas](#editor-de-mapas)


# Instalación

<br>
Lo primero es clonar el [repositorio](https://github.com/manucampoliete/tp-grupal-2025c2-grupo-3) del proyecto y luego procedes a la instalación. Para ello se deben ejecutar los siguientes comandos en una terminal

```bash
# Clonar repositorio
git clone https://github.com/manucampoliete/tp-grupal-2025c2-grupo-3

# Moverse al directorio raíz
cd tp-grupal-2025c2-grupo-3 

# Instalar juego
./installer.sh 
```

Luego de unos minutos, se deberá ver el mensaje de instalación exitosa para poder empezar a jugar.

```bash
[installer] Installation completed.
```

<hr>

# Ejecución e inicio de partida
<br>

Una vez que la instalación finalizó correctamente, se puede empezar a jugar. Para correr el servidor que hosteará el juego, ejecutar en una terminal

```bash
# puerto: 8080, por ejemplo
needForSpeed2DServer <puerto> 
```

Cada vez que se quiera agregar un nuevo jugador se debe correr en una nueva terminal

```bash
# puerto: 8080, por ejemplo. Debe ser el mismo que el servidor
needForSpeed2DClient localhost <puerto> 
```
<br>

El menú principal del juego se verá así

<p align="center">
  <img src="{{ site.baseurl }}/assets/menu.png" width="900">
</p>

Se tienen dos opciones: crear una partida o unirse a una ya creada. Si se quiere la primera opción, clickear en el boton de *New Game* para acceder a la pantalla de creación. En ella se encuentra un campo para ingresar el nombre de jugador y un carrusel para elegir el auto con el que se quiera jugar. En el mismo se muestran sus valores de salud y velocidad. Utilizando las flechas se puede cambiar la selección, teniendo para elegir entre 7 opciones.

<p align="center">
  <img src="{{ site.baseurl }}/assets/crearpartida.png" width="900">
</p>

Una vez creada con el botón de *Create Game*, se pasará a la pantalla de espera donde se muestra el ID con el cual se podrán unir otros jugadores. Si no se quiere esperar a nadie, se puede comenzar haciendo click en *Start Game*.

<p align="center">
  <img src="{{ site.baseurl }}/assets/anfitrionespera.png" width="900">
</p>

Para unirse a una partida, clickear *Join Game* en el menú principal. En la siguiente pantalla se deberá completar el nombre de jugador y el ID de la partida a la cual se quiere unir. Luego se puede elegir el auto con el que se quiera jugar.

<p align="center">
  <img src="{{ site.baseurl }}/assets/unirse.png" width="900">
</p>

Al clickear *Join Game*, se esperará hasta que el creador inicie la partida.

<p align="center">
  <img src="{{ site.baseurl }}/assets/invitadoespera.png" width="900">
</p>

Para cerrar correctamente el servidor cuando no se quiera utilizar mas, ingresar la tecla *q* en la consola donde corre el mismo.

<hr>

# Juego
<br>

### Reglas

Cada partida soporta hasta 8 jugadores en ella, con múltiples partidas pudiendo correr en simultáneo. A su vez, cada partida se divide en un número de carreras parametrizable. Cada carrera se correrá en una ciudad y recorrido elegidos de manera aleatoria dentro de los disponibles.

El objetivo del juego es seguir el recorrido marcado sobre la pista y llegar a la meta en el menor tiempo posible pasando por todos los checkpoints previamente. Pero este no es el único objetivo... ¡también hay que sobrevivir! Cada jugador tiene una barra de vida que irá agotando en caso de colisionar con edificios u otros autos. Si llega a cero, la carrera se acabará y tendrá que esperar a la próxima para volver a jugar.

Entre cada carrera se tendrán diez segundos para realizar mejoras al auto: agregar vida, velocidad, aceleración o masa. ¡Cuidado! Estas mejoras vendrán con una penalización de tiempo por cada una que se seleccione.

### Controles

Los comandos básicos de movimiento para el auto son

- **Acelerar:** W / ↑
- **Frenar, dar marcha atrás:** S / ↓
- **Girar a la izquierda:** A / ←
- **Girar a la derecha:** D / →

Se puede manejar el sonido con los siguientes comandos
- **Ctrl M:** silenciar música
- **Ctrl N:** silenciar sonidos
- **Ctrl +:** subir volumen
- **Ctrl -:** bajar volumen

Se pueden utilizar cheats dentro del juego con las siguientes combinaciones de teclas
- **I + L:** inmortalidad
- **G + C:** victoria instantánea
- **L + E:** muerte instantánea
- **F + H:** súper velocidad

<hr>

# Editor de mapas

Se pueden crear nuevos recorridos para jugar en las ciudades de Liberty City y Vice City. Para ejecutar el editor ingresar el siguiente comando en una terminal

```bash
needForSpeed2DEditor
```

y se abrirá la siguiente pantalla de elección.

<p align="center">
  <img src="{{ site.baseurl }}/assets/menueditor.png" width="900">
</p>

Las opciones a elegir son Liberty City, Vice City o Custom (San Andreas coming soon...). La opción *Custom* nos permite abrir un archivo .yaml previamente generado con el editor y seguir agregando elementos al recorrido creado. 

Luego de elegir la opción buscada se abrirá el editor. El mismo cuenta con la imagen del mapa en la cual se puede agrandar o achicar e ir agregando elementos, los cuales se ubican en una barra lateral izquierda. 

<p align="center">
  <img src="{{ site.baseurl }}/assets/editor.png" width="900">
</p>

Los elementos pueden ser agregados en estilo *drag and drop* sobre el mapa y ser movidos una vez colocados. Los recorridos deben armarse **en orden**. Es decir, primero crear la grilla de partida con los spawn boxes, luego colocar una línea de salida, ir poniendo hints/checkpoints en el orden que se haga el recorrido y finalizar con una meta de llegada.

Al terminar, el mapa puede ser guardado haciendo click en *Save Map* y se generará un .yaml. Si se quiere que el mapa pueda ser elegido para una partida, se lo debe guardar en la carpeta

```bash
/etc/needForSpeed2D/server/gameLogic/races
```

¡Hay cientos de recorridos para crear!