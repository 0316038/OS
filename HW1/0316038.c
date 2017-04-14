#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#define SIZE 10000

int main()
{
    while(1){
        
        sigset_t set;
        struct sigaction sig;
        sigemptyset(&set);
        sig.sa_handler = SIG_DFL;
        sig.sa_mask = set;
        sig.sa_flags = SA_NOCLDWAIT;
        sigaction(SIGCHLD, &sig, NULL);
        
        char buffer[SIZE];
        int length = 0;
        int wait = 1;
        
        //1.Display the prompt sign ">" and take a string from user
        printf(">");
        fgets(buffer, sizeof(buffer), stdin);
        length = strlen(buffer);
        if(buffer[length-2] == '&'){
            wait = 0;
            buffer[length-2] = '\0';
        }
        else{
            buffer[length-1] = '\0';
        }

        //2.Parse the string into a program name and arguments
        char* token;
        char* arg[SIZE];
        int i = 0;
        token = strtok(buffer," ");
        while(token != NULL){
            arg[i++] = token;
            token = strtok(NULL," ");
        }
        arg[i] = NULL;
        
        //3.Fork a child process
        int pfd[2];
        pid_t pid;
        pid=fork();
        
        if(pid < 0){  //error occured
            fprintf(stderr,"Fork Failed!");
            exit(-1);
        }
        else if(pid == 0){  //child process : Have the child process execute the program
            execvp(arg[0],arg);
        }
        else if(pid > 0){//parent process : Wait until the child terminates
            if(wait == 1){
                waitpid(pid,NULL,0);
            }
        }
    }
}
