#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>


int main(int argc, char *argv[]) {
    extern char **environ;
    while(1){
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s > ", getcwd(cwd, sizeof(cwd)));
        } else {
            perror("getcwd() error");
            return 1;
        }
        char str[50];
        char *pch;
        bool amper_flag = 0;
        fgets(str, 50, stdin);

        if(str[strlen(str) - 2] == '&'){
            amper_flag = 1;
            str[strlen(str) - 2] = '\0';
        }
        if(str[0] == '\n'){
            continue;
        }

        pch = strtok(str," \n");
        if(strcmp(pch, "exit") == 0){
            printf("See you soon \n");
            return 0;
        }else if(strcmp(pch, "vars") == 0){
            for (char **env = environ; *env != 0; env++){
                char *thisEnv = *env;
                printf("%s\n", thisEnv);
            }
        }else if(strcmp(pch, "set") == 0){
            pch = strtok (NULL, " \n");
            if(pch){
                char *varname = pch;
                pch = strtok (NULL, " \n");
                if(pch){
                    char *varvalue = pch;
                    setenv(varname, varvalue, 1);
                }else{
                    printf("*ERROR* Variable value needed but non given\n");
                }
            }else{
                printf("*ERROR* Arguments needed (set [name] [value])\n");
            }
        }else if(strcmp(pch, "del") == 0){
            pch = strtok (NULL, " \n");
            if(unsetenv(pch) != 0){
                printf("*ERROR* variable name needed\n");
            }
        }else if(strcmp(pch, "cd") == 0 || strcmp(pch, "cd") == 10){
            pch = strtok (NULL, " \n");
            if(pch){
                chdir(pch);
            }else{
                chdir(getenv("HOME"));
            }
        }else{
            if(amper_flag == 1){
                char *path = pch;
                pch = strtok (NULL, " \n");
                pid_t pid = fork();
                char *arguments[] = {path, pch, NULL };
                if(!pid){
                    if(execl(path, path, pch, NULL) == -1 ) {
                       if(execvp(path, arguments) == -1){
                            printf("*ERROR* No such program nor command\n");
                            return 102;
                        }
                    }
                }
            }else{
                char *path = pch;
                pch = strtok (NULL, " \n");
                if(pch == NULL){}
                pid_t pid = fork();
                char *arguments[] = {path, pch, NULL };
                if(!pid){
                    if(execl(path, path, pch, NULL) == -1 ) {
                       if(execvp(path, arguments) == -1){
                            return 102;
                        }
                    }
                }else{
                    int child;
                    waitpid(pid, &child, 0);
                    if(WEXITSTATUS(child) == 102){
                        printf("*ERROR* No such program nor command\n");
                    }else{
                        printf("EXIT CODE: %d\n", WEXITSTATUS(child));
                    }
                }
            }
        }
    }
    return 0;
}
