#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "commands.h"
#include <signal.h>

extern FILE* historyRead;
extern FILE* historyWrite;
extern FILE* logRead;
extern FILE* logWrite;

int indxInLarge = 0;
void readFile(FILE * file);
void writeFile(FILE * file, char* line);

char** splited(char* str, char* d) {
	int i = 0;
	char** spl = malloc(1000 * sizeof(char*));
	char* dub = malloc(sizeof(char) * (strlen(str) + 1));
	strcpy(dub, str);
	char* token = strtok(dub, d);

	while (token != NULL) {
		spl[i] = malloc((strlen(token) + 1) * sizeof(char));
		strcpy(spl[i], token);
		i++;
		token = strtok(NULL, d);
	}
	spl[i] = NULL;
	return spl;
}
int equallty(char* arr) {
	int i = 0;
	int n = strlen(arr);
	for (i = 0; i < n; i++) {
		if (arr[i] == '=') {
			return i;
		}
	}
	return -1;
}

char* dolarSin(char* line) {
	int i = 0;
	char* spl = malloc(1024);
	int j = 0;

	while (line[i] != '\0') {

		if (line[i] == '$') {
			i++;
			int k = 0;
			char* nameVar = malloc(100);

			while (line[i] != '\0' && line[i] != '\n' && line[i] != '/'
					&& line[i] != ' ' && line[i] != '$') {
				nameVar[k++] = line[i++];
			}
			nameVar[k++] = '\0';
			k = 0;
			char* val = getenv(nameVar);
			if (val != NULL)
				while (val[k] != '\0')
					spl[j++] = val[k++];

		} else if (line[i] == '~') {
			int z = 0;
			i++;
			char* home = getenv("HOME");
			if (home != NULL)
				while (home[z] != '\0')
					spl[j++] = home[z++];
		} else {
			spl[j++] = line[i++];

		}

	}
	spl[j] = '\0';
	return spl;
}

char** splitd(char* line) {
	indxInLarge = 0;
	int i = 0;
	int word = 0;
	char** arr_line = malloc(sizeof(char*));
	arr_line[0] = NULL;

	char* spl = malloc(1024);
	int j = 0;

	while (line[i] != '\0') {
		//word spaces
		if (line[i] == '\n') {
			i++;
			continue;
		}
		if (line[i] == ' ' || line[i] == '\t') {
			if (!word) {
				i++;
				continue;
			}

			spl[j] = '\0';
			arr_line[indxInLarge++] = spl;
			spl = malloc(1024);
			j = 0;
			word = 0;
		}

		else if (line[i] == '\"') {
			i++;
			word = 1;
			while (i < strlen(line) && line[i] != '\"')
				spl[j++] = line[i++];
		} else {
			word = 1;
			spl[j++] = line[i];
		}
		i++;
	}
	if (word) {
		spl[j] = '\0';
		arr_line[indxInLarge++] = spl;
	}
	arr_line[indxInLarge] = NULL;
	return arr_line;
}

void extv(char* fullPath, char** params) {
	writeFile(logWrite, "chiled has been created");
	int background = 0;
	if (strcmp(params[indxInLarge - 1], "&") == 0) {
		background = 1;
		params[indxInLarge - 1] = NULL;
	}

	pid_t pid = fork();
	//child
	if (pid == 0) {
		execv(fullPath, params);
		printf("error in child");
		exit(1);
	}
	//parent
	else if (pid > 0) {
		int status;
		if (!background) {
			do {
				waitpid(pid, &status, WUNTRACED);
			} while (!WIFEXITED(status) && !WIFSIGNALED(status));
		}
	} else {
		printf("can't fork");
	}

}

void ex(char* str) {
	char* dol = dolarSin(str);
	writeFile(historyWrite, dol);
	char** arr = splitd(dol);

	if (strcmp(arr[0], "cd") == 0) {
		if (arr[1] == '\0')
			cd(getenv("HOME"));
		else
			cd(arr[1]);
	} else if (strcmp(arr[0], "echo") == 0) {

		int i = 0, j = 0, k = 0;
		char* mass = malloc(1024);

		for (i = 1; i < indxInLarge; i++) {
			for (j = 0; j < strlen(arr[i]); j++) {
				mass[k++] = arr[i][j];
			}
			mass[k++] = ' ';
		}
		//printf("%s",mass);
		echo(mass);
	} else if (indxInLarge == 1 && equallty(arr[0]) > 0) {
		int x = equallty(arr[0]);
		char* key = malloc(50);
		char* val = malloc(100);
		int i = 0;
		for (i = 0; i < x; i++) {
			key[i] = arr[0][i];
		}
		for (i = x + 1; i < strlen(arr[0]); i++) {
			val[i - x - 1] = arr[0][i];
		}
		setenv(key, val, 1);

	} else if (strcmp(arr[0], "pwd") == 0 && indxInLarge == 1) {
		pwd(arr[0]);
	} else if (strcmp(arr[0], "exit") == 0) {
		fclose(historyRead);
		fclose(historyWrite);
		fclose(logRead);
		fclose(logWrite);
		exit(1);
	} else if (strcmp(arr[0], "history") == 0) {
		readFile(historyRead);

	} else {
		char** paths = splited(getenv("PATH"), ":");
		int i = 0;

		int len = 0;
		while (paths[len] != NULL) {
			len++;
		}

		for (i = 0; i < len; i++) {
			char* dub = malloc(
					sizeof(char) * (strlen(paths[i]) + 2 + strlen(arr[0])));

			strcpy(dub, paths[i]);
			strcat(dub, "/");
			strcat(dub, arr[0]);
			if ((access(dub, F_OK)) == 0) {
				extv(dub, arr);
				break;
			}
		}
	}
}

void sigChildF(int r) {
	writeFile(logWrite, "chiled finished!");
}


int main(int argc, char** argv) {

	historyRead = fopen("history.txt", "r");
	historyWrite = fopen("history.txt", "a");
	logWrite = fopen("log.txt", "a");
	logRead = fopen("log.txt", "r");

	struct sigaction sigchld;
	sigchld.sa_handler = sigChildF;
	sigemptyset(&sigchld.sa_mask);
	sigchld.sa_flags = 0;
	sigaction(SIGCHLD, &sigchld, NULL);

	char arr[512];

	if (argc == 2) {
		FILE* fp = fopen(argv[1], "r");
		if (!fp)
			return -1;
		while (fgets(arr, 512, fp)) {
			printf("shell>>%s\n", arr);
			ex(arr);
		}
		fclose(historyRead);
		fclose(historyWrite);
		fclose(logRead);
		fclose(logWrite);
		exit(0);
	}

	while (1) {
		printf("\n shell>>");
		fgets(arr, 512, stdin);
		ex(arr);

	}
	printf("bye bye");

	return 0;
}
