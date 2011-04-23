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

int main()
{
	char buffer[BUFF_MAX] = "";
	char* tokens = malloc(sizeof(char)*BUFF_MAX);
	char* rin_fname = malloc(sizeof(char)*ARGLEN_MAX);
	char* rout_fname = malloc(sizeof(char)*ARGLEN_MAX);
	char** args = NULL;
	int i = 0;
	int numargs = 0;
	
startloop:
	while(1){
		int background_flag = 0;
		int pipedcnt = 0;
		int fdout = 0;
		int fdin = 0;
		numargs = 0;
		
		printf("sish:> ");
		fgets(buffer, BUFF_MAX, stdin);

		// need to add ctrl+d exit
		if (!strcmp(buffer, "exit\n")) {
			goto end;
		}

		// check for the placement of special characters (<, >, and &)
		int rinflag = 0;  int routflag = 0;  int pipeflag = 0;
		for (i = 0; buffer[i] != '\0'; i++) {
			if (buffer[i] == '&' && buffer > 0 && buffer[i-1] == ' ') {
				if (buffer[i+1] == '\n') {
					background_flag = 1;
					buffer[i] = ' ';
				}
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
		
		
		// count the number of piped commands
		char* buffcopy = malloc(sizeof(char)*(strlen(buffer)+1));
		strcpy(buffcopy, buffer);
		strtok(buffcopy, "|");  pipedcnt++;
		while(strtok(NULL, "|") != NULL)
			pipedcnt++;
		strcpy(buffcopy, buffer);
		
		// put the commands in an array
		char** cmds = malloc(sizeof(char*)*pipedcnt);
		cmds[0] = strtok(buffcopy, "|");
		for (i=1; i<pipedcnt; i++)
			cmds[i] = strtok(NULL, "|");

		// create pipes
		int** pfds = malloc(sizeof(int*)*pipedcnt-1);
		for (i=0; i < pipedcnt-1; i++) {
			pfds[i] = malloc(sizeof(int)*2);
			pipe(pfds[i]);
		}
		
		// divide line by pipe chars
		int procno;
		pid_t lastpid;
		int status;
		for(procno = 0; procno < pipedcnt; procno++) {
			// prepare command
			char* cmd = malloc(sizeof(char)*(strlen(cmds[procno])+1));
			strcpy(cmd, cmds[procno]);
			printf("starting to work with %s\n", cmd);
			
			// get redirects
			if (procno == 0 && rinflag) {
				// get the filename
				char* tempstr;
				tempstr = strtok(cmd, "<");
				strcpy(cmd, tempstr);
				tempstr = strtok(NULL, " \n\t");
				strcpy(rin_fname, tempstr);
				// copy the rest of the args back in
				tempstr = strtok(NULL, " \n\t");
				while (tempstr != NULL) {
					strcat(cmd, " ");
					strcat(cmd, tempstr);
					tempstr = strtok(NULL, " \n\t");
				}
			}
			if (procno == pipedcnt-1 && routflag) {
				// get the filename
				char* tempstr;
				tempstr = strtok(cmd, ">");
				strcpy(cmd, tempstr);
				tempstr = strtok(NULL, " \n\t");
				strcpy(rout_fname, tempstr);
				// copy the rest of the args back in
				tempstr = strtok(NULL, " \n\t");
				while (tempstr != NULL) {
					strcat(cmd, " ");
					strcat(cmd, tempstr);
					tempstr = strtok(NULL, " \n\t");
				}
			}
		
			// separate args
			char* t = strtok(cmd, " \n\t");
			numargs = 0;
			while (t != NULL) {
				args = realloc(args, sizeof(char*)*(numargs+1));
				args[numargs] = realloc(args[numargs], sizeof(char)*ARGLEN_MAX);
				strcpy(args[numargs], t);
				t = strtok(NULL, " \n\t");
				numargs++;
			}
			args = realloc(args, sizeof(char*)*(numargs+1));
			args[numargs] = NULL;
			
			// fork it.  fork this project.
			switch (lastpid = fork()) {
				case 0:
					if (pipedcnt > 1) {
						if (procno != 0) { // redirect child's input to the last pipe
							dup2(pfds[procno-1][0], STDIN_FILENO);
							close(pfds[procno-1][1]);
						}
						if (procno != pipedcnt-1) { // redirect child's output to pipe
							dup2(pfds[procno][1], STDOUT_FILENO);
						}
						
					}
					if (procno == 0 && rinflag) {
						fdin = open(rin_fname, O_RDONLY);
						close(STDIN_FILENO);
						dup2(fdin, STDIN_FILENO);
						close(fdin);
					}
					if (procno == pipedcnt-1 && routflag) {
						fdout = open(rout_fname, O_WRONLY | O_CREAT, 0644);
						close(STDOUT_FILENO);
						dup2(fdout, STDOUT_FILENO);
						close(fdout);
					}
					execvp(args[0], args);
					return 0;
					break;
				case -1:
					printf("ERROR: could not create new process");
					goto startloop;
					break;
				default:
					if (pipedcnt > 1 && procno != 0) {
						close(pfds[procno-1][0]);
						close(pfds[procno-1][1]);
					}
					break;
			}
			
			// free memory
			free(cmd);
		}
		
		waitpid(lastpid, &status, 0);
		for (i=0; i < pipedcnt-1; i++) {
			free(pfds[i]);
		}
		free(pfds);
	}
	
end:
	free(tokens);
	free(rin_fname);
	free(rout_fname);
	for (i=0 ; i <= numargs; i++) {
		if (args != NULL && args[i] != NULL) free(args[i]);
	}
	if (args != NULL) free(args);
	
	return 0;
}


