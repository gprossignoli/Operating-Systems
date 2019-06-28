#!/bin/bash

ScheludersList=( "RR" "SJF" "FCFS" "PRIO" "RR_DYNQ" ) #lista de simuladores

#Pedimos al usuario un número de CPUs válido
echo "max CPUs --> "
read maxCPUs
while [ $maxCPUs -gt 8 ]; 
do
    echo "CPUs can not be more than 8 or less than 0"
    echo "max CPUs -->"
    read maxCPUs
done

echo "-----------------"

#Pelimos al usuario un nombre de fichero válido (ruta)
echo "File name --> "
read fileName
while [ ! -f $fileName ]; 
do
    echo "$fileName not exist"
    echo "File name --> "
    read fileName
done

echo "-----------------"

#Creamos el directorio de resultados donde irán los .log y los diagramas. Si existe lo eliminamos y lo reemplazamos por otro
if [ -d results ]; 
then
    rm -r results
fi
mkdir results

#Ejecutamos los diferentes modos de simulación con el fichero que el usuario nos indicó y movemos los resultados al nuevo directorio
for item in "${ScheludersList[@]}"; 
do
    for (( cpus = 1 ; $cpus<=$maxCPUs; cpus++ )) 
    do
        echo "===================================="
        echo "Executing $item model"
        echo "===================================="

        ./schedsim -n "$cpus" -s "$item" -i "$fileName"
        for (( i=0 ; $i<$cpus ; i++ )) 
        do
            mv CPU_$i.log results/"$item"-CPU-$i.log
        done
    done

    cd ../gantt-gplot

    for (( j=0 ; $j<$maxCPUs ; j++ )) 
    do
        ./generate_gantt_chart ../schedsim/results/"$item"-CPU-"$j".log
    done
    cd ../schedsim
done
