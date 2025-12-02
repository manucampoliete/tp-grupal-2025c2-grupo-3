---
title: Manual del Proyecto
permalink: /manual-proyecto/
layout: default
nav_order: '004'
---

# Organización y división del trabajo

Para llevar a cabo el desarrollo del proyecto, tomamos la recomendación realizada por nuestros correctores y nos dividimos las tareas de la siguiente manera:

- **Interfaz gráfica con SDL:** Camila Suarez
- **Protocolos y servidor:** Manuel Campoliete
- **Lógica y físicas del juego:** Nicolás Franco Celano Minig
- **Lobby y editor con QT:** Julieta Perez Goldstein

Utilizamos los primeros días para plantear las bases y entender cómo se conectaba cada parte, al menos de manera sencilla para luego ir escalando. En las primeras dos a tres semanas se desarrollaron la lobby con su correspondiente protocolo, mientras que en paralelo se iban trabajando la interfaz y las físicas del juego. A su vez se definieron las primeras versiones de estructuras como el *Snapshot*, las cuales se fueron iterando y modificando a medida que se agregaban funcionalidades al juego. 

La etapa media del proyecto se basó más que nada en agregar funcionalidades de manera incremental y tener andando correctamente el manejo de hilos. Al tener el editor de mapas finalizado, se pudieron implementar los recorridos y las distintas carreras. En las etapas finales se agregaron animaciones, sonidos, cheats y lógica de NPCs.

<hr>

# Herramientas y documentación

Como entornos de desarrollo los integrantes utilizamos Visual Studio Code, CLion y QT Creator. Además se utilizaron las herramientas de formateo vistas en la cátedra cppcheck, cpplint y clang-format. Para aprender sobre las tecnologías, se consultó con los siguientes sitios y documentación:

- [CPlusPlus](https://cplusplus.com/)
- Tutorial de SDL de [Lazy Foo](https://lazyfoo.net/tutorials/SDL/index.php)
- Stack Overflow
- [Documentación de QT](https://doc.qt.io/qt-6/widget-classes.html) + [Foro de QT](https://forum.qt.io/)
- [Tutorial Box2D](https://www.iforce2d.net/b2dtut/)
- [GeeksforGeeks](https://www.geeksforgeeks.org/)
- [Tutorial de constant rate loop](https://book-of-gehn.github.io/articles/2019/10/23/Constant-Rate-Loop.html)

<hr>

# Pendientes

En el lado de la interfaz nos faltaron agregar y corregir algunas cosas menores:

-  Auto mal cortado (por falta de color key).
-  Humo, brake trails y checkpoints no aparecen sobre puentes: todos los efectos se renderizan después de los puentes. Se podrían separar los efectos en onBridge true o false y renderizarlo entre capas. Se asociarían checkpoints a "layer" (ground/bridge) y renderizarían en dos pasadas.
-  Efectos mal ubicados en *WorldRenderer*: partículas y trails están en *WorldRenderer*, deberían estar en *EffectsManager* (falta de tiempo para refactorizar)
-  Volumen por distancia no completado: la función *playSoundWithDistance()* existe pero no se usa. Todos los sonidos tienen volumen fijo.

En cuanto a features, nos quedó por implementar el mapa San Andreas ya que era el más extenso en cantidad de puentes y colisiones.

<hr>

# Problemáticas encontradas

La naturaleza de este trabajo práctico conlleva a encontrarse con diversos problemas de los cuales no se suele conocer la solución, pero pudimos superarlos. Entre las mayores adversidades destacamos:

- Dificultad para encontrar información sobre el wrapper de SDL y entender la diferencia con SDL crudo.
- ⁠Integrar nuevas funcionalidades en la base armada del proyecto.
- Decisión de diseño para el pasaje de etapa de Lobby a Game en términos de hilos del server.
- Cierre del servidor sin leaks.
- La traducción de coordenadas (tanto de posiciones como de angulos) entre Box2D (metros y radianes) y SDL (pixeles y grados) resultó confusa al principio, y llevó a una pérdida de tiempo a la hora de implementar las colisiones del mapa.
- Manejo/pasaje de distintos tipos de datos numéricos (uints, floats, etc.).
- La unica version que se pudo usar de Box2D es la 2.4.1, que no es compatible con el debugDraw del tutorial, ya que este usa una versión más vieja y sin soporte.
- El dibujado de las colisiones del mapa fue una tarea manual y bastante tediosa, ya que hay puentes con lógica y los edificios comparten colores con varias otras cosas, por lo que las colisiones por color key no fueron posibles (por eso solo se puede andar por las calles).

<hr>

# Comentarios finales

Creemos que a esta instancia se llega con las herramientas y conocimiento suficiente para poder realizar el desarrollo. Nos hubiera gustado tener más información sobre cómo pasar floats por socket para poder implementarlo, aunque entendemos que puede ser un tema que escapa el alcance de la materia. 

Mirando hacia atrás, nos hubiera gustado plantear mejor el alcance global desde el principio para tener en cuenta las modificaciones que se van a necesitar a futuro y ser aún un poco más organizados. Sin embargo, creemos que nos desarrollamos muy bien como equipo y cada uno cumplió con sus roles como se esperaba sin causar problemas en la dinámica grupal. La comunicación entre nosotros fue constante y siempre que fue posible nos ayudamos entre partes. En definitiva, es un trabajo al que hay que dedicarle mucho tiempo y llegar a acuerdos para que todo pueda funcionar como debe. Sabiendo de antemano el tiempo que demanda la materia todos llegamos preparados para afrontar el desafío y creemos haberlo logrado correctamente.