/* Project 1 -- SISH -- Brian Dunlay / William Moy */

#include <stdio.h>

#define BUFF_MAX 1025 // 1024 + null terminating char

int main()
{
	char cmd_buff[BUFF_MAX];
	
	while(1){
		printf("sish:> ");
		fgets(cmd_buff, BUFF_MAX, stdin);
		printf(" -- bleep bloop: %s", cmd_buff);
	}
	
	return 0;
}

