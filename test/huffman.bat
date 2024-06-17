@echo off
REM Imprimimos una cabecera para nuestro archivo CSV
echo Archivo;Tamaño_original;Header;Tamaño_comprimido;Tiempo_de_compresion > resultados.csv

REM Repetiremos los experimentos 20 veces
for /l %%c in (1,1,20) do (  
    ejecutable.exe sources >> resultados.csv
)
