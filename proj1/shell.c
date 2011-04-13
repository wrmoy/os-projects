/* Project 1 -- SISH -- Brian Dunlay / William Moy -- CS 170 W'11 */

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
			return 0;
		}
	}
	
	return 0;
}

