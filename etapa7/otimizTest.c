#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/time.h>

int main(){
        struct timeval tv1, tv2;
        gettimeofday(&tv1, NULL);

        pid_t pid = fork();
        if(pid == 0){
                static char *argv[] = { "a.out" };
                execv("./a.out", argv);
                exit(127);
        }
        else{
                waitpid(pid, 0, 0);
                gettimeofday(&tv2, NULL);
                double time_spent = (double)(tv2.tv_usec - tv1.tv_usec) / 1000000 + (double)(tv2.tv_sec - tv1.tv_sec);
                fprintf(stderr, "Time spent: %lf seconds.\n", time_spent);
        }

        return 0;
}
