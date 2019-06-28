#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static int fileDescriptor;

#define PATH "/dev/leds" // Path to write
 
int menu();
int setLed(int filedesc, char *buf);
void fountain();
void binaryCounter();

void SleepMs(int ms) {
usleep(ms*1000); //convert to microseconds
return;
}

int main() {
	int finish= 0;
	int option;

	printf("\nStarting the program\n");

	if((fileDescriptor = open(PATH, O_RDWR)) < 0) {
		puts("Error: The file coudln't be opened\n");
		return -1;
	}

	while (!finish) {

		option = menu();
		puts("Starting execution\n");

		if (option==0) // exit program
			finish=1;
		else if (option == 1) // fountain
			fountain(); 
		else if (option == 2) // binary counter
			binaryCounter();			

		puts("Finished execution\n");
		
		// Turn off leds after execution finished
		if((setLed(fileDescriptor, "")) != 0) {
			puts("Problems setting the leds\n");
		}
	}

	if (close(fileDescriptor) < 0) {
		puts("Can not close the file\n");
		return -1;
	} 

	return 0;
}

int menu() {
	int option = -1;
	int valid = 0;

	puts("\n\n Option | Description");
	puts("   1    | The Fountain");
	puts("   2    | Binary counter");
	puts("   0    | Exit\n");

	while (!valid) {
		puts("Select an option please (type the number): \n");
		scanf("%d", &option);
		if (option >= 0 && option <= 2)
			valid=1;
		else
			puts("Wrong option! Try again\n");
	}

	return option;
}

int setLed(int filedesc, char *buf) {
	int len = strlen(buf);
    int bytes_writed;
	if((bytes_writed = write(filedesc, buf, len)) < 0) {
		puts("Can not write to file\n");
		return -1;
	}
	return 0;
}

void fountain() {
	int i=0, j=0, total = 3;
	char *ledsNumber = "123";
	char c;
	
	for (i=0; i < total; i++) {
		for (j = 0; j < 4; j++) {
			c = ledsNumber[j];
			if (j == 3) {
				c = "";
			} 
			if((setLed(fileDescriptor, &c))) {
				puts("Problems setting the leds\n");
			}
			SleepMs(190);
		}
	}
}

void binaryCounter() {
	char* buff;
	int i;
	
	for (i=0; i <= 7; i++) {
		
		if (i==0) buff = "0";
		else if (i==1) buff = "3"; 
		else if (i==2) buff = "2";
		else if (i==3) buff = "23";
		else if (i==4) buff = "1";
		else if (i==5) buff = "13";
		else if (i==6) buff = "12";
		else if (i==7) buff = "123";
		 
		if((setLed(fileDescriptor, buff)) != 0) {
			puts("Problems setting the leds\n");
		}
		
		SleepMs(1000);
	}
}
