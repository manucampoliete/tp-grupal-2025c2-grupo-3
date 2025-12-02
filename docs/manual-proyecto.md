---
title: Manual del Proyecto
permalink: /manual-proyecto/
layout: default
nav_order: '004'
---

# División del trabajo

Para llevar a cabo el desarrollo del proyecto, tomamos la recomendación realizada por nuestros correctores y nos dividimos de la siguiente manera:

- **Interfaz gráfica con SDL:** Camila Suarez
- **Protocolos y servidor:** Manuel Campoliete
- **Lógica y físicas del juego:** Nicolás Franco Celano Minig
- **Lobby y editor con QT:** Julieta Perez Goldstein

# Pendientes

En el lado de la interfaz nos faltaron agregar y corregir algunas cosas menores:

-  Auto mal cortado (por falta color key).
-  Humo, brake trails y checkpoints no aparecen sobre puentes: todos los efectos se renderizan después de los puentes. Se podrían separar los efectos en onBridge true o false y renderizarlo entre capas. Se asociarían checkpoints a "layer" (ground/bridge) y renderizarían en dos pasadas.
-  Efectos mal ubicados en *WorldRenderer*: partículas y trails están en *WorldRenderer*, deberían estar en *EffectsManager* (falta de tiempo para refactorizar)
-  Volumen por distancia no completado: la función *playSoundWithDistance()* existe pero no se usa. Todos los sonidos tienen volumen fijo.

Como features, nos quedó por implementar:

- Mapa San Andreas
