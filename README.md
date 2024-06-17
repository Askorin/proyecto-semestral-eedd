## Cómo hacer el testeo

Para correr los tests de este proyecto, tener las siguientes consideraciones:

### Prerrequisitos

El testeo se basa en textos de [Pizza&Chili](https://pizzachili.dcc.uchile.cl/texts.html), por lo que cualquier archivo de texto con un peso minimo de 100MB en este sitio deberia funcionar correctamente para realizar el testeo.

Por motivos de espacio, en el repositorio solo hay un archivo .gz que contiene un texto llamado 'sources', por loq ue sera importante descompirmirlo antes que nada.

Para crear archivos de distintos tamaños, usamos `truncate.cpp`, por ejemplo para un archivo `sources` que se encuentra en la carpeta `data`:
```
g++ -o ejecutable truncate.cpp
./ejecutable sources
```

### Ejecutar los tests

Una vez obtenidos todos los textos, dependiendo de lo que se necesite, se puede realizar una o varias veces un mismo test:

#### Realizar 1 test
```
g++ -o ejecutable  huffmanTest.cpp ../src/huffman.cpp
./ejecutable sources
```
#### Realizar varios test
Para ello usaremos `huffman.bat`
```
.\huffman.bat
```
### Obtención de resultados

Si todo sale bien, al realizar multiples experimentos se debe crear un archivo `resultados.csv` el cual indique
* Archivo
* Tamaño original  (en bytes)
* Header utilizado
* Tamaño del archivo comprimido (en bytes)
* Tiempo de compresión (en ms)