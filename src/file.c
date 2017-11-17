#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

FILE* historyRead;
FILE* historyWrite;
FILE* logRead;
FILE* logWrite;

void readFile(FILE * file){
  char str[1000];
  //file = fopen( "history.txt" , "r");
	if (file) {
	    while (fscanf(file, "%s", str)!=EOF)
	        printf("%s \n",str);
	}

}

void writeFile(FILE * file, char* line){
	if (file != NULL) {
	       fprintf(file,"%s \n",line);
	}

}
