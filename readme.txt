Para la compilación del programa convolucion basta con ingresar el comando make en la terminal y el programa será compilado.
Para eso, es necesario tener instalado el compliador gcc en su sistema operativo. Para comprobar si está instalado, use el comando:
	gcc --version
En caso de no reconocer la versión, siga los siguientes pasos para la instalación en Linux:
1. Ingrese el siguiente comando: sudo apt update
2. Ingrese el comando: sudo apt install build-essential
3. Verificar la versión, confirmando así su instalación

Para la compilación del programa, haga lo siguiente:
1. Ingresar a la terminal (consola)
2. Ingresar al directorio del programa
3. Ingresar el comando: make

Uso del programa:
Ingresando solo nombre y dirección de imagen a procesar: 
	<dirección/nombre_imagen> 

Ingresando el nombre y dirección de imagen a procesar junto al filtro que quiere utilizar: 
	<dirección/nombre_imagen> <filtro>

Ingresando nombre y dirección de imagen a procesar y archivo resultante: 
	<dirección/nombre_imagen> <dirección/nombre_resultado>

Ingresando nombre y dirección de imagen a procesar y archivo resultante junto al filtro que quiere utilizar: 
	<dirección/nombre_imagen> <dirección/nombre_resultado>  <filtro>

Filtros disponibles que puede ingresar mediante texto o mediante su respectivo número:
1 -> sobel, 2 -> blur (por defecto), 3 -> sharpen

Tests:
Dentro del directorio del programa, existen dos carpetas: images y results,
    images: contienen imágenes en formato pgm que serán usados para la realización de pruebas.
    results: es la carpeta donde se guardará por defecto el resultado de la convolución en caso de no ingresar un nombre/directorio de archivo resultante.

Aquí presento unos ejemplos para realizar tests del programa (¡Importante!: Asegúrese que haya compilado el programa con make o que tenga el ejecutable):

images/tank.pgm 1
images/barbara.pgm results/resultado.pgm sharpen
images/sandiego.pgm
images/lanna.pgm identity
