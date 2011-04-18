/* Project 1 -- SISH -- Brian Dunlay / William Moy -- CS 170 W'11 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h> 
#include <sys/wait.h>

#define BUFF_MAX 1025 // 1024 + null terminating char

void parse_arg(char* arg); 

int main()
{
	char buffer[BUFF_MAX] = "";
	while(1){
		printf("sish:> ");
		fgets(buffer, BUFF_MAX, stdin);

		// need to add ctrl+d exit
		if (!strcmp(buffer, "exit\n")) {
			return 0;
		}

		char* args[BUFF_MAX] = {0};
		int i = 0;
		char* arg = strtok(buffer, " ");
		
		for (i = 0; arg != NULL; i++) {
			char x[BUFF_MAX] = "";
			strcpy(x, arg);
			args[i] = x;
			arg = strtok(NULL, " ");
			printf("PROC: %s\n", args[i]);
		}
		
		args[++i] = NULL;
		
	}
	
	return 0;
}
