/* Project 1 -- SISH -- Brian Dunlay / William Moy -- CS 170 W'11 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h> 
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#define BUFF_MAX 1025 // 1024 + null terminating char
#define ARG_MAX 16
#define ARGLEN_MAX 64

void parse_arg(char* arg); 

int main()
{
	char buffer[BUFF_MAX] = "";
	char* tokens = malloc(sizeof(char)*BUFF_MAX);
	char* rin_fname = malloc(sizeof(char)*ARGLEN_MAX);
	char* rout_fname = malloc(sizeof(char)*ARGLEN_MAX);
	//char* pipe_toks[2];
	char** args = NULL; //malloc(sizeof(char*)*ARG_MAX);
	int i = 0;
	//for (i=0; i < ARG_MAX; i++)
	//	args[i] = malloc(sizeof(char)*ARGLEN_MAX);
	
startloop:
	while(1){
		int background_flag = 0;
		//int pipedcnt = 0;
		int fdout = 0;
		int fdin = 0;
		
		printf("sish:> ");
		fgets(buffer, BUFF_MAX, stdin);

		// need to add ctrl+d exit
		if (!strcmp(buffer, "exit\n")) {
			goto end;
		}

		// check for the placement of special characters (<, >, and &)
		int rinflag = 0;  int routflag = 0;  int pipeflag = 0;
		for (i = 0; buffer[i] != '\0'; i++) {
			if (buffer[i] == '&') {
				if (buffer[i+1] == '\n')
					background_flag = 1;
				else {
					printf("ERROR: misplaced &\n");
					goto startloop;
				}
			}
			if (buffer[i] == '>')
				routflag = 1;
			if (buffer[i] == '|') {
				pipeflag = 1;
				if (routflag == 1) {
					printf("ERROR: misplaced >\n");
					goto startloop;
				}
			}
			if (buffer[i] == '<'){
				rinflag = 1;
				if (pipeflag == 1) {
					printf("ERROR: misplaced <\n");
					goto startloop;
				}
			}
		}
		
			
		/*
		// count the number of piped commands
		strtok(buffer, "|");  pipedcnt++;
		while(strtok(NULL, "|") != NULL)
			pipedcnt++;
		
		// divide line by pipes
		pipe_toks[0] = strtok(buffer, "|");
		pipe_toks[1] = strtok(NULL, "|");
		for(i = 1; i < pipedcnt; i++) {
			
			// get new command sequence
			pipe_toks[0] = pipe_toks[1];
			pipe_toks[1] = strtok(NULL, "|");
		}
		*/
		
		if (rinflag) {
			char* tempstr;
			strtok(buffer, "<");
			tempstr = strtok(NULL, " \n\t");
			tempstr = strtok(tempstr, " \n\t");
			strcpy(rin_fname, tempstr);
			printf("\n\nfilein: %s\n\n", rin_fname);
		}
		if (routflag) {
			char* tempstr;
			strtok(buffer, ">");
			tempstr = strtok(NULL, " \n\t");
			tempstr = strtok(tempstr, " \n\t");
			strcpy(rout_fname, tempstr);
			printf("\n\nfileout: %s\n\n", rout_fname);
		}
		int numargs = 0;
		char* t = strtok(buffer, " \n\t");
		for (numargs=0; t != NULL; numargs++) {
			args = realloc(args, sizeof(char*)*(numargs+1));
			args[numargs] = malloc(sizeof(char)*ARGLEN_MAX);
			strcpy(args[numargs], t);
			t = strtok(NULL, " \n\t");
		}
		numargs++;
		args = realloc(args, sizeof(char*)*(numargs+1));
		args[numargs] = 0;
		int pid;
		int status;
		switch (pid = fork()) {
			case 0:
				if (rinflag) {
					fdin = open(rin_fname, O_RDONLY);
					close(STDIN_FILENO);
					dup2(fdin, STDIN_FILENO);
					close(fdin);
				}
				if (routflag) {
					fdout = open(rout_fname, O_WRONLY);
					close(STDOUT_FILENO);
					dup2(fdout, STDOUT_FILENO);
					close(fdout);
				}
				printf("child starting\n");
				execvp(args[0], args);
				return 0;
				break;
			case -1:
				printf("ERROR: could not create new process");
				goto startloop;
				break;
			default:
				waitpid(pid, &status, 0); 
				break;
		}
		
		if (rinflag) {
			
		}
		
		
		for (i=0; i < numargs; i++)
			free(args[i]);
		free(args);
		args = 0;
	}
	
end:
	// free memory
	free(tokens);
	free(rin_fname);
	
	return 0;
}


