0. Caso ALERTA dentro de DIAGNOSTIC
usar un define UMBRAL para stack tareas, para heap y para FU.
STACK nulo 
HEAP nulo 
FU con la cuenta eso del 69% (valor teorico) y valor calculado (limite practico) 

1. Esquema de tareas
iba a haber 4 tareas, cada una destinada a un proposito concreto, conectadas con queues. Basados en lo recomendados por la consigna de la catedra

2. Tama~nos de los stack 
consideramos la tarea con posiblemente mas stack ocupado (DISPLAY), libreria ssd1306, libreria string, recibe muchas queues de las distintas tareas (enumerar queuees). 
Para graficar necesita conservar los valores -> array grande

el resto se fue considerando tama~nos a medida que se escribia codigo y la cantidad de variables necesarias y el tipo. Statick stack analyzer

3. Tama~no de heap
En base a las tareas Statick stack analyzer. 
primero se intento reducir el stack de las tareas pero a medida que avanzaba el proyecto fue necesario agrandar el heap para darle mas stack a ciertas tareas. (Analisis con debugger se noto que figuraban las tareas en el freeRTOS task list, en particular osThreadNew ->xTaskCreate -> xReturn = errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY devuelve error al no haber espacio suficiente para la tarea) 

4. Factor de utilizacion
**TODO** 
* comprobar con logic analyzer veracidad del resultado. 
* Preguntar agustin filtro.

5. grafica de la FSM de GUI

6. Justificacion de como se obtuvo el stack a tiempo real (magia negra con puenteros y casteos)

7. Capturas de pantalla (analizador logico, Stack analyzer y demas)

--------------------------------------------------------------------------------------------------------------
TODO mas adelante para entrega final (mas codigo)
--------------------------------------------------------------------------------------------------------------

* Display pantalla framerate CONSTANTE
* Caso capacitores par a PROCESSING 
* Borrar pantalla de forma eficiente
* Rango dinamico pantalla 


--------------------------------------------------------------------------------------------------------------
Feature innecesarias que estarian buenas
--------------------------------------------------------------------------------------------------------------

* que quede bonito el menu usando bitmaps en vez de simpelmente escribir texto con ssd1306_puts
* agregarle un buzzer para que haga el ruido de la se~nal graficada equivalencia resistencia frecuencia tipo sitentizador

