#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <time.h>

#define MAX_LEN_LINE    100
#define maxlen 100


void welcome()
{
  struct tm *t;
  time_t now;
  now = time(NULL);
  t = localtime(&now);

  printf("\n -------------------------\n");
  printf("|  welcome to sun shell!  |\n");
  printf("|       %d.%02d.%02d.       |\n", t->tm_year+1900, t->tm_mon + 1, t->tm_mday);
  printf(" -------------------------\n\n");
}

void shell_prompt()
{
  char uname[maxlen], dname[maxlen];
  getlogin_r(uname, maxlen);
  getcwd(dname, maxlen);
  printf("\e[93m%s\x1b[0m in \e[94m%s\x1b[0m: ", uname, dname);
}

int builtin_commend(char *args[])
{
  if (strcmp(args[0],"exit")==0)
  {
    exit(1);
  }

  if (strcmp(args[0], "cd")==0)
  {
    chdir("/");
    return 1;
  }
  return 0;
}

int main(void)
{
  char command[MAX_LEN_LINE];
  char *args[] = {command, NULL};
  int ret, status;
  pid_t pid, cpid;

  welcome();

    while (true) {
        char *s;
        int len;
        
        shell_prompt();
        s = fgets(command, MAX_LEN_LINE, stdin);
//        printf("%s", s);
        if (s == NULL) {
            fprintf(stderr, "fgets failed\n");
            exit(1);
        }

        len = strlen(command);
//        printf("%d\n", len);
        if (command[len - 1] == '\n') {
            command[len - 1] = '\0'; 
        }

        args[0] = command;
        if (builtin_commend(args))
          continue;

//        printf("[%s]\n", command);
        pid = fork();
        if (pid < 0) {
            fprintf(stderr, "fork failed\n");
            exit(1);
        } 
        if (pid != 0) {  /* parent */
            cpid = waitpid(pid, &status, 0);
            if (cpid != pid) {
                fprintf(stderr, "waitpid failed\n");        
            }
            printf("Child process terminated\n");
            if (WIFEXITED(status)) {
                printf("Exit status is %d\n", WEXITSTATUS(status)); 
            }
        }
        else {  /* child */
            ret = execve(args[0], args, NULL);
            if (ret < 0) {
                fprintf(stderr, "execve failed\n");   
                return 1;
            }
        } 
    }
    return 0;
}
