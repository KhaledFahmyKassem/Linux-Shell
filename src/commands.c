#include <unistd.h>
#include "commands.h"
#include <string.h>
char* currentDirictory;

void cd(const char* path) {

	chdir(path);

}


void echo(const char* message) {
	printf("%s", message);
}

void pwd(char* str){
	char arr[1024];
	getcwd(arr,1024);
	printf("%s",arr);
}


