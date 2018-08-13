// file: TwoPipesTwoChildren.cpp
// author: Parijat Singh
// date: 04/02/2017
// purpose: CS3376
// description:
// this program executes "ls -ltr | grep 3376 | wc -l", by dividing the three command among the two child and parent process
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char **argv) {
int status;
char *cat_args[] = {"ls", "-ltr", NULL};
char *grep_args[] = {"grep", "3376", NULL};
char *wc_args[] = {"wc", "-l", NULL};

// create two pipes to send the output of "ls" process to "grep" process and to "wc" process

int pipes1[2];
int pipes2[2];
pipe(pipes1);
pipe(pipes2);
int childpid;

//first child
if((childpid = fork()) == -1){
	perror("Error creating a child process");
	exit(1);
}
if( childpid == 0 ) {
	dup2(pipes1[1], 1); /* redirect standard output to pipes1 write end */
	close(pipes1[0]);
	close(pipes1[1]);
	execvp(*cat_args, cat_args);
	exit(0);
}
//parent
else {	
	//second child
	if((childpid = fork()) == -1){
		perror("Error creating a child process");
		exit(1);
	}
	if( childpid == 0 ) {
		dup2(pipes1[0], 0); /* redirect standard input to pipes1 read end */
		dup2(pipes2[1], 1); /* redirect standard output to pipes2 write end */
		close(pipes1[0]);
		close(pipes1[1]);
		close(pipes2[0]);
		close(pipes2[1]);
		execvp(*grep_args, grep_args);
		exit(0);
    }else{
		dup2(pipes2[0], 0); /* redirect standard input to pipes2 read end */
		close(pipes1[0]);
		close(pipes1[1]);
		close(pipes2[0]);
		close(pipes2[1]);
		execvp(*wc_args, wc_args);
	}
}
return(0);

}