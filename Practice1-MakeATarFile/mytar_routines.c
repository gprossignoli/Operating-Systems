#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mytar.h"


/** Copy nBytes bytes from the origin file to the destination file.
 *
 * origin: pointer to the FILE descriptor associated with the origin file
 * destination:  pointer to the FILE descriptor associated with the destination file
 * nBytes: number of bytes to copy
 *
 * Returns the number of bytes actually copied or -1 if an error occured.
 */
static int copynFile(FILE * origin, FILE * destination, unsigned int nBytes)
{
	int caughtChars = 0;
	int transferredChars = 0;
	int originChar = 0;
	int dstChar = 0;

	while(transferredChars < nBytes){
		originChar = getc(origin);
		if(originChar == EOF)
			break;
		caughtChars++;

		dstChar = putc(originChar,destination);
		if(dstChar == EOF)
			break;
		transferredChars++;
	}
	if(caughtChars != transferredChars){
		perror("fallo al copiar el fichero");
		return -1;
	}

	return transferredChars;
}

/** Loads a string from a file.
 *
 * file: pointer to the FILE descriptor 
 * 
 * The loadstr() function must allocate memory from the heap to store 
 * the contents of the string read from the FILE. 
 * Once the string has been properly built in memory, the function returns
 * the starting address of the string (pointer returned by malloc()) 
 * 
 * Returns: !=NULL if success, NULL if error
 */

static char* loadstr(FILE * file){

	//Guardamos la posicion de memoria del primer char del file
	FILE tmp = *file;
	int currentCharacter = -1;
	int fileNameLenght = 0;

	//Calcular longitud del string
	while(currentCharacter != '\0')
	{
		currentCharacter = getc(file);
		if(currentCharacter == EOF)
		{
			perror("error al leer el archivo");
			return NULL;
		}
		fileNameLenght++;
	}
	//Reservamos memoria para gaurdar el string
	char *str = malloc(fileNameLenght);
	if(str == NULL)
	{
		perror("error al reservar en memoria");
	}
	//Recuperamos la posicion de memoria del primer caracter
	*file = tmp;
	//Creamos el string
	for(int i = 0; i < fileNameLenght; i++)
	{
		str[i] = getc(file);
		if(str[i] == EOF)
		{
			perror("error al leer el archivo");
			return NULL;
		}
	}

	return str;
}

/** Read tarball header and store it in memory.
 *
 * tarFile: pointer to the tarball's FILE descriptor 
 * nFiles: output parameter. Used to return the number 
 * of files stored in the tarball archive (first 4 bytes of the header).
 *
 * On success it returns the starting memory address of an array that stores 
 * the (name,size) pairs read from the tar file. Upon failure, the function returns NULL.
 */
static stHeaderEntry* readHeader(FILE * tarFile, unsigned int *nFiles)
{
	stHeaderEntry * array = NULL ;
	unsigned int nr_files =0;

	if(fread(&nr_files,sizeof(unsigned int),1,tarFile) != 1){
		perror("error al leer el tarball buscando el numero de ficheros");
		return NULL;
	}

	array = malloc ( sizeof ( stHeaderEntry )* nr_files );
	if(array == NULL){
		perror("error al reservar memoria para el array de estructuras header");
		return NULL;
	}
	int i = 0;
	while(i < nr_files){
		char *str = loadstr(tarFile);
		array[i].name = str;
		if(fread(&array[i].size, sizeof(unsigned int),1,tarFile)<1){
			perror("error al leer el tamaño del fichero en el tarball");
			return NULL;
		}
		i++;
	}

	(* nFiles )= nr_files;
	return array;
}

/** Creates a tarball archive 
 *
 * nfiles: number of files to be stored in the tarball
 * filenames: array with the path names of the files to be included in the tarball
 * tarname: name of the tarball archive
 * 
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First reserve room in the file to store the tarball header.
 * Move the file's position indicator to the data section (skip the header)
 * and dump the contents of the source files (one by one) in the tarball archive. 
 * At the same time, build the representation of the tarball header in memory.
 * Finally, rewind the file's position indicator, write the number of files as well as 
 * the (file name,file size) pairs in the tar archive.
 *
 * Important reminder: to calculate the room needed for the header, a simple sizeof 
 * of stHeaderEntry will not work. Bear in mind that, on disk, file names found in (name,size) 
 * pairs occupy strlen(name)+1 bytes.
 *
 */
int createTar(int nFiles, char *fileNames[], char tarName[])
{
        //crea tarfile
        FILE *tarFile = fopen(tarName,"w+");
        if(tarFile == NULL){

                perror("el fichero tar no ha podido crearse");
                return EXIT_FAILURE;
        }


        //incializa array de estructuras de la cabecera
        stHeaderEntry *header = malloc(nFiles * sizeof(stHeaderEntry));
        if(header == NULL){
                perror("error al reservar memoria para el array de estructuras header");
                return EXIT_FAILURE;
        }

        int fileNamesSpace = 0;
        for(int i = 0; i < nFiles; i++){
                header[i].name = fileNames[i];
                fileNamesSpace += strlen(fileNames[i]) + 1;
        }

        //apunta a la posicion en la que se empiezan a escribir los archivos
        unsigned int offData = sizeof(int) + nFiles * sizeof(unsigned int) + fileNamesSpace;
        int successOnFseek = fseek(tarFile,offData,SEEK_SET);
        if(successOnFseek == -1){
        	perror("error al intentar colocar el indicador de posicion tarFile en offData");
        	return EXIT_FAILURE;
        }
        //volcado del contenido de ficheros
        int readBytes = 0; //almacena los bytes reales leidos
        for(int i = 0; i < nFiles; i++){
                FILE *inputFile = fopen(fileNames[i], "r");
                if(inputFile == NULL){
                	perror("error al intentar abrir un fichero de entrada");
                	return -1;
                }

                readBytes = copynFile(inputFile, tarFile, UINT_MAX);
                int successOnClose = fclose(inputFile);
                if(successOnClose != 0){
                	perror("error al intentar cerrar un fichero de entrada");
                	return EXIT_FAILURE;
                }

                header[i].size = readBytes;
        }

        //rellenar la cabecera

        int exitoRebobina = fseek(tarFile,0,SEEK_SET);
        if(exitoRebobina == -1){
        	perror("error al rebobinar hasta el inicio de tarFile al rellenar la cabecera");
            return EXIT_FAILURE;
        }
        const int itemsInsideNfiles = 1;
        int writeItems = fwrite(&nFiles, sizeof(nFiles), itemsInsideNfiles, tarFile);
        if(writeItems < itemsInsideNfiles){
            perror("la escritura del nFiles en el tarFile no se ha completado");
            return EXIT_FAILURE;
        }
        //fflush(tarFile);
        for(int i = 0; i < nFiles; i++){
        	const int itemsInsideHeaderName = 1;
        	writeItems = fwrite(header[i].name, strlen(header[i].name)+1,itemsInsideHeaderName,tarFile);
        	if(writeItems < itemsInsideHeaderName){
        		perror("la escritura del nombre del fichero en el tarFile no se ha completado");
                return EXIT_FAILURE;
        	}
        	//fflush(tarFile);
        	const int itemsInsideHeaderSize = 1;
        	writeItems = fwrite(&header[i].size, sizeof(header[i].size),itemsInsideHeaderSize,tarFile);
            if(writeItems < itemsInsideHeaderSize){
            	perror("la escritura del tamaño del fichero en el tarFile no se ha completado");
            	return EXIT_FAILURE;
            }
            //fflush(tarFile);
        }

        fclose(tarFile);
        return EXIT_SUCCESS;
}



/** Extract files stored in a tarball archive
 *
 * tarName: tarball's pathname
 *
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First load the tarball's header into memory.
 * After reading the header, the file position indicator will be located at the 
 * tarball's data section. By using information from the 
 * header --number of files and (file name, file size) pairs--, extract files 
 * stored in the data section of the tarball.
 *
 */
int extractTar(char tarName[])
{
	FILE *tarFile = fopen(tarName,"r");
	if(tarFile == NULL){
		perror("el fichero tar no ha podido crearse");
	    return EXIT_FAILURE;
	}
	unsigned int nFilesInTarball = 0;

	//extraer encabezado
	stHeaderEntry *header = readHeader(tarFile,&nFilesInTarball);

	//extraer ficheros
	for(int i = 0; i < nFilesInTarball; i++){
		FILE *fichero = fopen(header[i].name, "w+");
		if(fichero == NULL){
			perror("el fichero no ha podido crearse");
			return EXIT_FAILURE;
		}
		//extraer datos
		copynFile(tarFile,fichero,header[i].size);
	}

	if(fclose(tarFile) != 0){
		perror("no ha podido cerrarse el tarFile");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
