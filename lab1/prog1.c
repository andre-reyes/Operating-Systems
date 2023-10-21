/* 
Andre Reyes
Operating Systems
9/28/2023

Prog1.c:
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
    int prog_repeat = atoi(argv[1]);
    //checks if number of arguments matches as intended to avoid conflict
    if (argc != 2)
    {
        printf("Format must be: [file location] [repeat (int) times]\n");
        exit(0);
    }
    

    do{
        pid = fork();
        if (pid < 0){
            perror("fork");
            exit(0);
        }
        //This is the child block
        if (pid == 0){
            
            char *args[] = {"echo","I am the grandchild", NULL};
            char *argp[] = {NULL};
            int res = execve("/bin/echo", args, argp);

            if(res == -1){
                perror("execv");
                exit(2);
            }
        }
        //This is the parent block
        else{

            int child_pid = pid;

            waitpid(child_pid, NULL, 0);
            // printf("Child %d exited from %d\n", child_pid, getpid());
        }
        counter ++;

    } while (counter < prog_repeat);
    
    return 0;
}
