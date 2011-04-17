/* Project 1 -- SISH -- Brian Dunlay / William Moy -- CS 170 W'11 */

#include <stdio.h>
#include <string.h>

#define BUFF_MAX 1025 // 1024 + null terminating char

int main()
{
	char buffer[BUFF_MAX] = "";
	while(1){
		printf("sish:> ");
		fgets(buffer, BUFF_MAX, stdin);

		// should also exit on ctrl+d, aka EOF
		if (!strcmp(buffer, "exit\n")) {
			return 0;
		}

		// separate processes
		char* token = strtok(buffer, "|");	
		while(token != NULL) {
			printf("%s ", token);
			token = strtok(NULL, "|");
		}
	}
	
	return 0;
}