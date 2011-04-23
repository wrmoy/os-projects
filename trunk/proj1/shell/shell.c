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
	char** args = NULL; //malloc(sizeof(char*)*ARG_MAX);
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
		char* pipe_tok;
		pipe_tok = strtok(buffcopy, "|");
		for(procno = 0; pipe_tok != NULL; procno++) {
			// prepare command
			char* cmd = malloc(sizeof(char)*(strlen(pipe_tok)+1));
			strcpy(cmd, pipe_tok);
			
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
			for (numargs=0; t != NULL; numargs++) {
				args = realloc(args, sizeof(char*)*(numargs+1));
				args[numargs] = realloc(args[numargs], sizeof(char)*ARGLEN_MAX);
				strcpy(args[numargs], t);
				t = strtok(NULL, " \n\t");
			}
			args = realloc(args, sizeof(char*)*(numargs+1));
			args[numargs] = NULL;
			
			printf("starting to fork proc%i\n", procno);
			// fork it.  fork it all.
			switch (lastpid = fork()) {
				case 0:
					if (pipedcnt > 1) {
						printf("starting to redirect the pipes in proc%i\n", procno);
						if (procno != pipedcnt-1) {
							//close(STDOUT_FILENO);
							//dup2(pfds[procno][1], STDOUT_FILENO);
							//close(pfds[procno][1]);
							printf("redirecting proc%i out\n", procno);
						}
						if (procno != 0) {
							//close(STDIN_FILENO);
							//dup2(pfds[procno-1][0], STDIN_FILENO);
							//close(pfds[procno-1][0]);
							printf("redirecting proc%i in\n", procno);
						}
						printf("finished redirecting the pipes in proc%i\n", procno);
						
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
//					waitpid(lastpid, &status, 0); 
					break;
			}
	
			
			// get new command sequence
			pipe_tok = strtok(NULL, "|");
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
		free(args[i]);
	}
	free(args);
	
	return 0;
}


