Buenas. Estas correcciones las hice en base a la rama `main`, recibió unos commits mientras lo hacía pero no los revisé.

# Features
## Carrera y Partida
1. ~~Carreras de hasta 8 jugadores.~~  
2. ~~Las partidas tienen múltiples carreras. NO (siempre se queda en la primera partida)~~  
3. ~~Múltiples recorridos. NO (no hay recorridos)~~  
4. ~~Tabla de posiciones. NO (se entra en fase pero no se muestra nada)~~  
4.a. ~~Tiempo de cada jugadores. NO~~  
4.b. ~~Tiempo total acumulado. NO~~  
5. ~~Checkpoints en los recorridos (franjas que cruzan la calle). NO~~  
5.a. ~~Checkpoint de salida. NO~~  
5.b. ~~Checkpoint de llegada. NO~~
6. ~~Hints en los recorridos. NO~~
7. ~~Aparición de checkpoints y hints en orden. NO~~ (*)

(*) En vez de mostrar todos e ir quitando los que ya pasamos quizas tenemos que mostrar de a tramos.

## Fin de carrera
1. ~~Finaliza si todos llegan a la meta. NO~~
2. Autos descalificados no cuentan. NO (no hay descalificados, si me salgo el auto queda ahí inmóvil)
3. ~~Finaliza si pasan 10 minutos. SI~~
4. ~~Suma de penalizaciones para tiempo total en estadísticas. NO~~
5. ~~Elección y modificaciones del auto~~
6. ~~Se elige el auto a usar en el Lobby. SI~~
7. ~~Autos tienen:~~  
7.a. ~~Velocidad. SI~~  
7.b. ~~Aceleración. SI~~  
7.c. ~~Salud. PARCIAL (sí pero no se puede ver y el auto no se rompe)~~  
7.d. ~~Masa. NO~~
8. ~~Múltiples autos: al menos 4. SI~~
9. ~~Etapa de mejoras de autos. PARCIAL (se entra en fase pero no se ve nada)~~  
9.a. ~~Mejora de atributos del auto. NO~~  
9.b. ~~Penalización en tiempo por mejora. NO~~  

## NPCs
1. Existen NPCs. NO
2. Recorrido al azar. NO

## Choques
1. ~~Choque con edificios y autos. SI~~  
2. ~~Baja la velocidad y salud del auto. NO~~  
3. Severidad depende del ángulo. NO  
4. ~~Animación de choque. NO~~  
5. ~~Animación de explosión de auto por salud en cero. NO~~  
6. ~~Pantalla de carrera perdida por salud en cero. NO~~    



## Ciudades
1. 3 ciudades disponibles para jugar. NO (tenemos 2, falta juntar server y client)
2. ~~Autos pasan por arriba y por abajo de los puentes. NO~~

## Configuración
1. ~~Configuración en YAML~~.
2. ~~Atributos de autos.~~
3. ~~Tiempos de fases del juego.~~
4. ~~Tiempo de partida.~~ (*)
5. ~~Cantidad de carreras.~~
6. ~~Rate loop.~~

(*) El tiempo de partida en si no esta en el archivo de config, pero se puede deducir a partir de los demas valores

## Cheats
PARCIAL (los primeros tres están, pero hacerlos no tiene impacto real en el juego, sigue normalmente, y no hay daño por colisiones)
1. Vida infinita.
2. Ganar automáticamente.
3. ~~Perder automáticamente.~~  
4. Alguno adicional.

## Vista y sonidos
1. ~~Clara visualización de un auto cuando está por encima o por debajo de un puente. NO~~
2. ~~Visualización de rotaciones. SI~~
3. ~~Cámara centrada en el auto. SI~~
4. ~~Minimapa. SI~~ (agregar check + hints?)
5. ~~Checkpoints y hints en el minimapa. NO~~
6. ~~El minimapa renderiza al jugador. SI~~
7. Hints visuales al colisionar según la potencia del impacto. NO (*)
8. ~~Sonidos. PARCIAL (solo de countdown)~~  
8.a. ~~Sonidos al colisionar. NO~~  
8.b. ~~Sonido de fin de carrera. NO~~  
8.c. ~~Sonido de frenada. NO~~
9. Volumen regulado por distancia al origen del sonido. NO
10. ~~Música dentro de la partida. SI~~
11. ~~Animaciones (al menos dos). NO~~
12. ~~Hud con tiempo restante de partida. SI~~


## Editor
No pude compilar el editor porque no está en el makefile y cuando quise intentar añadir el target ‘taller_editor’ no funcionó, pero les dejo más o menos las features que tenemos en cuenta.
1. ~~Cargar carreras.~~
2. ~~Guardar carreras y usarlos en el juego~~
3. ~~Añadir checkpoints.~~
4. ~~Añadir hints.~~
5. ~~Visualizar mapa completo~~
6. ~~Drag & Drop para checkpoints y hints~~
7. ~~Elegir mapa para armar el recorrido.~~
8. ~~Checkpoint de salida y llegada.~~
9. Visualizar recorrido ordenado.  
10. ~~Zoom in & Zoom out.~~
11. ~~Spawns de autos.~~
Como no ví checkpoints ni hints al correr el cliente asumo que no está todavia.

## UX / Jugabilidad
El movimiento del auto es suave y las rotaciones son generosas porque permite salirme de edificios cuando las colisiones tienen bugs. Aparte de eso, la aceleración no se nota al empezar a conducir, es como si de golpe le diera velocidad al auto, aunque sí se detiene solo, y cuando choco con otro auto lo atravieso un poco.
También, estaría bueno ver la vida, aunque por ahora no parece que pierda vida al chocar. El minimapa está bien pero es difícil verme a mí y a los otros autos.

## Cliente Servidor
~~En código, el lado del servidor lo veo bien, con algunas mejoras de asignación de responsabilidades pero no mucho más. El cliente está desorganizado, tiene clases que hacen demasiadas cosas, y usan mutex en el hilo principal.~~

~~El servidor no cierra correctamente, ni con ni sin jugadores conectados. Probé bajar el server con ‘q’ cuando tengo un jugador conectado y uno se conectó y se desconectó, pero causó un core dump en el server, muchos leaks de valgrind, y el cliente conectado siguió reproduciendo música y sólo terminó cuando manualmente cerré la ventana del juego, causando otro core dump.
El servidor también cerro con core dump cuando no había jugadores conectados.~~

- Mejoras de asignacion de responsabilidades en el Server? Ver cuales mas hay (ya esta resuelto lo de no levantar el yaml dentro del protocolo)

## Robustez / Valgrind
~~El servidor tiene leaks y cerrarlo con jugadores conectados me causó un core dump tanto en servidor como cliente. El servidor cierra con un core dump independientemente de si los jugadores están conectados o no.~~

El servidor ya no tiene leaks ni core dumps. Ver lugares en donde se pueda dejar de usar new, y usar smart pointers en su lugar.

## Compilación / Instalación
Compilé con ‘make’, aunque no se indica qué dependencias hacen falta ni cómo se instalan o cómo se compila el juego. Tuve que instalar box2d, la librería de yaml y qt6 a mano, sería conveniente un instalador por makefile hasta que el instalador formal esté terminado.

## Performance
~~Performance óptimo, menos de 5% de consumo de en cliente y servidor CPU.~~

## Código
Acá les dejo algunas observaciones. Como les mencioné, el servidor está bien, solo añado sugerencias con el tema de RAII, yaml, etc, pero para el cliente les dejé algunas que creo que son más importantes.

Servidor:  
1- Estaría bueno que algunas clases que heredan de Thread, como Acceptor y Game, manejen su propio join, start y stop. Ahora mismo, por ejemplo, Server le hace eso al Acceptor, pero el Acceptor mismo podría encargarse de hacer eso en sus propios constructor y destructor respectivamente, lo que sería más RAII.  
2- ~~El Server no debería devolver códigos de error o éxito, solo tirar una excepción si algo salió mal o no hacer nada si se cerró correctamente.
https://github.com/manucampoliete/tp-grupal-2025c2-grupo-3/blob/c5b4256a4a2b357b41fa751c7a1406f4cda3476d/server/server.cpp#L15~~
3-
~~https://github.com/manucampoliete/tp-grupal-2025c2-grupo-3/blob/c5b4256a4a2b357b41fa751c7a1406f4cda3476d/server/synchronized/responseQueuesMonitor.h#L38-L39
Podría directamente devolver un bool o tirar una excepción si no está la queue.~~  
4- Acá el Player le pide todos sus datos al Car, podría ser, en su lugar, que el Car tenga la responsabilidad de construir su propio snapshot.
https://github.com/manucampoliete/tp-grupal-2025c2-grupo-3/blob/c5b4256a4a2b357b41fa751c7a1406f4cda3476d/server/gameLogic/player.cpp#L17-L32
5- ~~En vez de cargar el .yaml cada vez que quiero crear un auto podría cargarse una vez y luego acceder a él desde este método.
https://github.com/manucampoliete/tp-grupal-2025c2-grupo-3/blob/c5b4256a4a2b357b41fa751c7a1406f4cda3476d/server/gameLogic/car_builder.h#L15-L29~~
~~6- Esto podría venir del .yaml
https://github.com/manucampoliete/tp-grupal-2025c2-grupo-3/blob/c5b4256a4a2b357b41fa751c7a1406f4cda3476d/server/gameLogic/game.cpp#L27-L30~~
7- ~~Para evitar tener que hacer el CRL a mano es que deberían usar la clase que implementaron en common.
https://github.com/manucampoliete/tp-grupal-2025c2-grupo-3/blob/c5b4256a4a2b357b41fa751c7a1406f4cda3476d/server/gameLogic/game.cpp#L266-L285~~
8- ~~Entiendo por qué le pusieron este nombre
https://github.com/manucampoliete/tp-grupal-2025c2-grupo-3/blob/c5b4256a4a2b357b41fa751c7a1406f4cda3476d/server/clientHandling/clientHandler.cpp#L38-L41
Pero el ClientHandler no necesita saber que el Sender es un Thread o que el Receiver es un ‘fake thread’. Justamente, hacerlo RAII les permitiría encapsular eso, para el ClientHandler el Sender y el Receiver son entidades que de algún modo manejan IO para él, no necesita saber que lo hacen a través de threads. No son solo los nombres de los métodos, sino que debería bastar con en cada uno solo crear al Sender y al Receiver, no tener que hacerles start.~~  
9- ~~Si cada vez que tengo que llamarlo tengo que pasarle la referencia, es mejor directamente construirlo con esa referencia.
https://github.com/manucampoliete/tp-grupal-2025c2-grupo-3/blob/c5b4256a4a2b357b41fa751c7a1406f4cda3476d/server/clientHandling/clientHandler.cpp#L27-L30
Si lo que le pasara por referencia fuera el socket, estaría claramente mal, en este caso es lo mismo.~~  
10- ~~Este archivo está en common
[tp-grupal-2025c2-grupo-3/common/messages/gameData.h at main · manucampoliete/tp-grupal-2025c2-grupo-3](https://github.com/manucampoliete/tp-grupal-2025c2-grupo-3/blob/main/common/messages/gameData.h)
Aunque no encontré su uso en el servidor. De todas formas, esta clase tiene contenido de SDL, así que NO debería estar en common, solo la debería usar en el cliente. Esto implica que, o bien están dependiendo de SDL en el servidor, lo cual está mal, o está en la carpeta equivocada, que es un simple fix.~~

Cliente:  
1- Están usando dependencias de SDL puro. No es grave, pero lo mejor es SDL2pp para RAII, con SDL, en ese sentido, tienen que reinventar la rueda prácticamente. Lo menciono por cosas como el uso de SDL_Mixer
[tp-grupal-2025c2-grupo-3/client/audio/soundManager.h at main · manucampoliete/tp-grupal-2025c2-grupo-3](https://github.com/manucampoliete/tp-grupal-2025c2-grupo-3/blob/main/client/audio/soundManager.h#L8C1-L8C28)
Que podrían cambiar por SDL2pp::Mixer.  
2- ~~No usen std::thread, a mano para eso tienen la clase Thread.
https://github.com/manucampoliete/tp-grupal-2025c2-grupo-3/blob/c5b4256a4a2b357b41fa751c7a1406f4cda3476d/client/lobby/lobby.cpp#L123-L130
Es preferible que tengan varios Thread que solo usan una vez a tener varios std::thread sueltos por ahí.~~  
3- ~~No estoy seguro de si Qt les impone usar news a mano cada vez. Sin embargo, si alguna vez pueden elegir entre usar news y deletes a mano o usar smart pointers (si Qt se los permite), usen siempre smart pointers.
https://github.com/manucampoliete/tp-grupal-2025c2-grupo-3/blob/c5b4256a4a2b357b41fa751c7a1406f4cda3476d/client/lobby/guestwaiting.cpp#L8
Aunque, como les digo, no estoy al tanto de si con Qt hay problemas con eso, en cuyo caso estaría todo ok con usar news.~~  
4- ~~Tienen mucha lógica acoplada en Game. Deberían separarla en más clases.
https://github.com/manucampoliete/tp-grupal-2025c2-grupo-3/blob/main/client/gameHandling/game.cpp~~  
5- ~~En ese mismo Game están usando World, que tiene un mutex interno. Como Game corre la interfaz gráfica, lo debe hacer desde el hilo principal; si están usando mutex en el hilo principal, eso está mal, el hilo principal no debe bloquearse, lo único que puede tomarle tiempo es procesar eventos y renderizar, pero no puede estar bloqueado por mutexes.~~


El servidor está prolijo dentro de todo, quizá movería algunas cosas a otras clases y haría alguna que otra cosa más RAII, pero está bien aún así. En el cliente tienen mutexes en el hilo principal, lo que no está bien, el hilo principal debería procesar eventos y renderizar, pero sin bloquear el hilo en ese entretanto, y hay bastante lógica acoplada en las clases Game y GameLoop.
Sí es importante que solucionen todos los leaks del server y que no haya core dumps.

## Documentación
No hay documentación, ni instrucciones de compilación, ni de instalación.

Estos comentarios
https://github.com/manucampoliete/tp-grupal-2025c2-grupo-3/blob/c5b4256a4a2b357b41fa751c7a1406f4cda3476d/CMakeLists.txt#L50-L58
https://github.com/manucampoliete/tp-grupal-2025c2-grupo-3/blob/c5b4256a4a2b357b41fa751c7a1406f4cda3476d/CMakeLists.txt#L177
https://github.com/manucampoliete/tp-grupal-2025c2-grupo-3/blob/c5b4256a4a2b357b41fa751c7a1406f4cda3476d/CMakeLists.txt#L188
Pónganlos en el README, de otra forma quien va a jugar su juego va a tener que ir a su código fuente para saber qué tiene que instalar. Además, no deben esperar que el usuario final haga una instalación por terminal de las dependencias, podrían ponerlo en el makefile.

## Tests
No hay tests unitarios del protocolo.
