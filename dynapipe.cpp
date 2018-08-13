// file: dynapipe.cpp
// author: Parijat Singh
// date: 04/05/2017
// purpose: CS3376
// description:
// this program executes "ls -ltr | grep 3376 | wc -l", by dividing the three command among the two child and parent process
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

void closePipes(int** pipes, int pcount){
	int i;
	for(i=0; i<pcount; i++){
		close(pipes[i][0]);
		close(pipes[i][1]);	
	}
}

void execute(char** pCommands, int k){
	char* _args[] = {pCommands[2*k], pCommands[2*k+1], NULL};
	execvp(*_args, _args);
}

int main(int argc, char **argv) {
//int status, 
int i, k=0;
int pcount = argc-1;
//check for argument count
if(pcount <2 || pcount > 5){
	perror("Error : Must have at least 2 and at most 5 arguments");
	exit(1);
}
//lets parse now, set space as token character
const char s[2] = " ";
char **pCommands = new char* [2*(argc-1)];
for (argc--,argv++,i=1 ; argc > 0; argc--,argv++,i++) {
		char *token;
		token = strtok(argv[0], s);  
		pCommands[2*(i-1)] = token;
		/* get the first token */
		token = strtok(NULL, s);  
		pCommands[2*(i-1)+1] = token;		
    }
//printf ("%s \n", "Executing ->");
for (argc = 0; argc < 2*pcount; argc++){
	if(argc%2 == 0) printf ("%s ", pCommands[argc]);
	else printf ("%s | ", pCommands[argc]);
}
printf ("\n%s\n ", "Result:");

int loop = pcount;

int **pipes;
pipes = (int**)malloc(sizeof(int*) * pcount);
for (int i = 0 ; i != pcount ; i++) {
    pipes[i] = new int[2];
	pipe(pipes[i]);
}

int childpid = 0;
if((childpid = fork()) == -1){
	perror("Error creating a child process");
	exit(1);
}
else if(childpid == 0){
	//first child
	dup2(pipes[0][1], 1); /* redirect standard output to pipes1 write end */
	closePipes(pipes, pcount);
	execute(pCommands, k);
	exit(0);
}
else {
	k++;
	loop--;	
	//second child
	if(loop > 1){
		if((childpid = fork()) == -1){
			perror("Error creating a child process");
			exit(1);
		}
		else if( childpid == 0 ) {
			//second child
			dup2(pipes[0][0], 0); /* redirect standard input to pipes1 read end */
			dup2(pipes[1][1], 1); /* redirect standard output to pipes2 write end */
			closePipes(pipes, pcount);
			execute(pCommands, k);
			exit(0);		
		}else{
			k++;
			loop--;
			if(loop > 1){
				if((childpid = fork()) == -1){
				perror("Error creating a child process");
				exit(1);
				}
				else if( childpid == 0 ) {
					//third child
					dup2(pipes[1][0], 0); /* redirect standard input to pipes1 read end */
					dup2(pipes[2][1], 1); /* redirect standard output to pipes2 write end */
					closePipes(pipes, pcount);
					execute(pCommands, k);
					exit(0);		
				}else{
					//parent
					k++;
					loop--;
					if(loop > 1){
						if((childpid = fork()) == -1){
						perror("Error creating a child process");
						exit(1);
						}
						else if( childpid == 0 ) {
							//fourth child
							dup2(pipes[2][0], 0); /* redirect standard input to pipes read end */
							dup2(pipes[3][1], 1); /* redirect standard output to pipes write end */
							closePipes(pipes, pcount);
							execute(pCommands, k);
							exit(0);		
						}else{
							// parent when loop 5
							dup2(pipes[3][0], 0); /* redirect standard input to pipes2 read end */
							closePipes(pipes, pcount);
							execute(pCommands, k);						
						}
					}else{
						//parent
						dup2(pipes[2][0], 0); /* redirect standard input to pipes2 read end */
						closePipes(pipes, pcount);
						execute(pCommands, k);
					}
				}
			}else{
				//parent
				dup2(pipes[1][0], 0); /* redirect standard input to pipes2 read end */
				closePipes(pipes, pcount);
				execute(pCommands, k);				
			}
		}
	}else{
		//parent	
		dup2(pipes[0][0], 0); /* redirect standard input to pipes2 read end */
		closePipes(pipes, pcount);
		execute(pCommands, k);
	}
}
return(0);
}


