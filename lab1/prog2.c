/* 
Andre Reyes
Operating Systems
9/28/2023

Prog2.c:
    This program takes a single  argument n from the command
    line and creates a set of processes and then terminates.
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>



int main(int argc, char* argv[]){
    int counter = 0;
    pid_t pid;
    int n = atoi(argv[1]);
    
    if (argc != 2 || n < 1 || n > 4)
    {
        printf("Format must be: [file location] [int value 1-4]\n");
        exit(0);
    }
    
    for (int i = 0; i <= (2*n - 1); i++)
    {
        
        pid = fork();
        if (pid < 0){
            perror("fork");
            exit(0);
        }
        //This is the child block
        if (pid == 0){
            printf("I am process %d of process %d\n", getpid(), getppid());
            exit(0);
        }
        //This is the parent block
        else{
            int child_pid = pid;
            waitpid(child_pid, NULL, 0);
        }
        counter ++;

    }
    
    return 0;
}
