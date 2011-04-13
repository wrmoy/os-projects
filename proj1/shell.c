/* Project 1 -- SISH -- Brian Dunlay / William Moy */

#include <stdio.h>

#define BUFF_MAX 1025 // 1024 + null terminating char

int main()
{
	char cmd_buff[BUFF_MAX];
	
	while(1){
		printf("sish:> ");
		fgets(cmd_buff, BUFF_MAX, stdin);
	
		// cmd_buff needs to be tokenized first, then
		// the command parsed, and then the args dealt
		// with accordingly
	
		// should also exit on ctrl+d, aka EOF
		if (!strcmp(cmd_buff, "exit\n")) {
			printf("Exit\n");
		}
		else if (!strcmp(cmd_buff, "ls\n")) {
			printf("List\n");
		}
		else if (!strcmp(cmd_buff, "cat\n")) {
			printf("Concatenate\n");
		}
		else if (!strcmp(cmd_buff, "pwd\n")) {
			printf("Print working directory\n");
		}	
		else if (!strcmp(cmd_buff, "cd\n")) {
			printf("Change directory\n");
		}	
		else if (!strcmp(cmd_buff, "rm\n")) {
			printf("Remove\n");
		}	
		else if (!strcmp(cmd_buff, "mv\n")) {
			printf("Move\n");
		}	
		else if (!strcmp(cmd_buff, "mkdir\n")) {
			printf("Make Directory\n");
		}	
		else if (!strcmp(cmd_buff, "echo\n")) {
			printf("Echo\n");
		}	
		else {
			printf("-sish: command not found: %s", cmd_buff);
		}	
	}
	
	return 0;
}

