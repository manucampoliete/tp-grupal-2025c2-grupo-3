---
title: Manual del Usuario
permalink: /manual-usuario/
layout: default
nav_order: '002'
---

Bievenido al manual de usuario del **Need for Speed 2D**. Acá te explicamos cómo clonar el repositorio, instalar el proyecto y ejecutar los programas para jugar. Se recomienda utilizar un sistema operativo basado en Ubuntu 24.04.

<br>

# Instalación

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

<br>

# Ejecución e instrucciones de juego

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

A continuación se tienen dos opciones: crear una partida o unirse a una ya creada. Si se quiere la primera opción, clickear en el boton de *New Game* para acceder a la pantalla de creación. En ella se encuentra un campo para ingresar el nombre de jugador y un carrusel para elegir el auto con el que se quiera jugar. En el mismo se muestran sus valores de salud y velocidad. Utilizando las flechas se puede cambiar la selección, teniendo para elegir entre 7 opciones.

<p align="center">
  <img src="{{ site.baseurl }}/assets/crearpartida.png" width="900">
</p>
