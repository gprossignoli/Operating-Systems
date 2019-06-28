#! /bin/bash

DirecTmp=tmp # directorio temporal
DirecOut=out 
MyTar_File=mytar.mtar  #archivo mytar

File1=file1.txt
File2=file2.txt
File3=file3.dat

echo "tester"

#1. Mytar esta en el directorio actual y es ejecutable

if ! [ -f mytar ]; 
then
    echo "No se ha encontrado el archivo Mytar"
    exit 1 #devuelve 1 = devuelve ERROR

elif ! [ -x mytar ];
then
    echo "No se ha podido ejecutar el archivo Mytar"


fi

#2. Borrar directorio temporal (si existe)

if [ -d "$DirecTmp" ];
then
    rm -r $DirecTmp
    echo "Se ha eliminado el directorio temporal"
fi

#3. Crear un nuevo directorio temporal (tmp) y nos movemos al el

mkdir $DirecTmp 
echo "Se ha creado el directorio $DirecTmp"
cd $DirecTmp

#4. Crear 3 ficheros de destino

if ! [ -f $File1 ];
then
    touch $File1
    echo "Hello World!" > $File1
    echo "Se ha creado $File1"
fi
if ! [ -f $File2 ];
then
    touch $File2
    head -10 /etc/passwd > $File2 #copia las 10 primeras lineas
    echo "Se ha creado $File2"
fi
if ! [ -f $File3 ];
then
    touch $File3
    head -c 1024 /dev/urandom > $File3
    #contenido binario aleatorio de 1024 Bytes
    echo "Se ha creado $File3"
fi

#5. Invocar programa para crear un archivo mtar con los 3 archivos creados

./../mytar -c -f $MyTar_File $File1 $File2 $File3

#6. Crear un directorio out (esta en tmp = actual) y copia el fichero mtar creado a ese nuevo directorio

if ! [ -d $DirecOut ];  #creamos el directorio
then
    mkdir $DirecOut
    echo "Se ha creado el directorio $DirecOut"
fi

    #copiamos el archivo .tar
cp ./$MyTar_File ./$DirecOut/$MyTar_File 

#7. Cambia al directorio out y ejecuta el programa mytar

cd $DirecOut
./../../mytar -x -f $MyTar_File

#8. Comparamos los nuevos archivos creados con los anteriores

if diff ../$File1 $File1 > /dev/null;
then
    echo "El fichero $File1 es igual en ambos directorios"
else
    echo "El fichero $File1 NO es igual en ambos directorios"
    exit 1
fi

if diff ../$File2 $File2 > /dev/null;
then
    echo "El fichero $File2 es igual en ambos directorios"
else
    echo "El fichero $File2 NO es igual en ambos directorios"
    exit 1
fi

if diff ../$File3 $File3 > /dev/null;
then
    echo "El fichero $File3 es igual en ambos directorios"
else
    echo "El fichero $File3 NO es igual en ambos directorios"
    exit 1
fi

#9. Fin del test

cd ../..
echo "El test ha finalizado correctamente"
exit 0

