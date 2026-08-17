# J-Shell
Proyecto - Sistemas Operativos 2025-1

JSHELL es un shell personalizado que amplía las funcionalidades básicas de un intérprete de comandos tradicional, permitiendo la ejecución de comandos simples y encadenados mediante tuberías, además de manejar internamente comandos esenciales como cd. Para facilitar la ejecución de programas locales, JSHELL modifica la variable de entorno PATH incluyendo el directorio actual.

El shell incorpora scripts especializados como cleanbin, diseñado para liberar espacio de almacenamiento identificando y eliminando archivos temporales antiguos, y memproc, que permite monitorear en tiempo real el uso de memoria de los procesos, destacando con colores aquellos que superan un límite establecido.

Por otro lado, cuenta con comandos propios para trabajar con archivos de forma sencilla.
Por ejemplo, unarch permite juntar varios archivos en uno solo, verificando que los archivos existan; divarch permite dividir un archivo en varios archivos más pequeños, repartiendo su contenido en partes lo más equilibradas posible según la cantidad de líneas que el usuario especifique; y newline permite insertar texto en una línea específica de un archivo, evitando que se generen espacios vacíos.
