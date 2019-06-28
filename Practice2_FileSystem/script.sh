#!/bin/bash

SRC="./src" # Archivo con el src del proyecto
TMP="./temp" # archivo temporal
MPOINT="./mount-point" #Nuestro SF

FILE1="myFS.h"
FILE2="fuseLib.c"

FILE3="MyFileSystem.c"

rmdir $TMP # directorio
mkdir $TMP # directorio

# a. Copiar 2 ficheros de más de 1 bloque(File 1 y 2) en el SF y en el directorio temporal

cp $SRC/$FILE1 $TMP/$FILE1	  #copiamos myFS.h en temp
cp $SRC/$FILE1 $MPOINT/$FILE1  #copiamos myFS.h en mi SF

cp $SRC/$FILE2 $TMP/$FILE2	  #copiamos fuseLib.c en temp
cp $SRC/$FILE2 $MPOINT/$FILE2  #copiamos fuseLib.c en mi SF

# b. Auditar el disco y diff entre ficheros originales y los copiados. Truncarlos(my_truncate) en mi SF y en Tmp para que ocupe un disco menos

./my-fsck virtual-disk #Auditar el disco

    #diff de los ficheros
if diff $MPOINT/$FILE1 $TMP/$FILE1 > /dev/null
then echo "El fichero $FILE1 es igual en ambos directorios"
else echo "El fichero $FILE1 NO es igual en ambos directorios"
fi

if diff $MPOINT/$FILE2 $TMP/$FILE2 > /dev/null
then echo "El fichero $FILE2 es igual en ambos directorios"
else echo "El fichero $FILE2 NO es igual en ambos directorios" 
fi

    #truncate
truncate --size=-4096 $TMP/$FILE1
truncate --size=-4096 $MPOINT/$FILE1

echo "El primer fichero ($FILE1) ha sido truncado"

# c. Auditar el disco y hacer un diff entre el original y el truncado

./my-fsck virtual-disk

if diff $SRC/$FILE1 $MPOINT/$FILE1 > /dev/null
then echo "El fichero truncado $FILE1 es igual al original"
else echo "El fichero truncado $FILE1 NO es igual al original"
fi

# d. Copiar el File3 al SF

cp $SRC/$FILE3 $MPOINT/$FILE3  #copiamos myFileSystem.c en mi SF

# e. Auditar el disco y comprobar que File3 es igual en SRC y en SF (diff)

./my-fsck virtual-disk

if diff $SRC/$FILE3 $MPOINT/$FILE3 > /dev/null
then echo "El fichero $FILE3 es igual en ambos directorios"
else echo "El fichero $FILE3 NO es igual en ambos directorios"
fi

# f. Truncar File2 en mi SF y en Tmp para que ocupe algun bloque de datos más

truncate --size=+4096 $TMP/$FILE2
truncate --size=+4096 $MPOINT/$FILE2

# g. Auditar el disco y hacer un diff entre el File2 original y el truncado

./my-fsck virtual-disk

if diff $SRC/$FILE2 $MPOINT/$FILE2 > /dev/null
then echo "El fichero truncado $FILE2 es igual al original"
else echo "El fichero truncado $FILE2 NO es igual al original"
fi

if diff $SRC/$FILE2 $TMP/$FILE2 > /dev/null
then echo "El fichero truncado $FILE2 es igual al original"
else echo "El fichero truncado $FILE2 NO es igual al original"
fi



