#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <time.h>
#include <dirent.h>

#define MAX_LEN_LINE    100
#define maxlen 100

void welcome();
void shell_prompt();
int builtin_command(char *args[]);


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
//    printf("%s", s);

    if (s == NULL)
    {
      fprintf(stderr, "fgets failed\n");
      exit(1);
    }

    if (!strcmp(command, "\n"))
      continue;

    len = strlen(command);
//    printf("%d\n", len);
    if (command[len - 1] == '\n')
    {
      command[len - 1] = '\0';
    }

    if (builtin_command(args))
      continue;


//    printf("[%s]\n", command);
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
  char uname[maxlen], hname[maxlen], dname[maxlen];

  getlogin_r(uname, maxlen);
  gethostname(hname, maxlen);
  getcwd(dname, maxlen);

  printf("\e[93m%s@%s\x1b[0m in \e[94m%s\x1b[0m: ", uname, hname, dname);
}

int builtin_command(char *args[])
{
  char *args2, *temp;
  char *delim = " ";

  temp = strtok_r(args[0], delim, &args2);

  if (!strcmp(temp, "exit")) //exit command
  {
    exit(1);
  }

  if (!strcmp(temp, "clear")) //clear command
  {
    system("clear");
    return 1;
  }

  if (!strcmp(temp, "pwd")) //pwd command
  {
    char wd[maxlen];
    getcwd(wd, maxlen);
    printf("%s\n", wd);
    return 1;
  }

  if (!strcmp(temp, "cd")) //cd command
  {
    temp = strtok_r(NULL, delim, &args2);

    if (temp == NULL) //command without dir
    {
      chdir(getenv("HOME"));
      return 1;
    }

    else //command with dir
    {
      if (chdir(temp)==-1)
      {
        fprintf(stderr, "failed to Change Directory\n");
        return 1;
      }
      chdir(temp);
      return 1;
    }
  }

  if (!strcmp(temp, "ls")) //ls command
  {
    DIR *dir;
    char cwd[maxlen];
    struct dirent *ent;

    temp = strtok_r(NULL, delim, &args2);

    if (temp == NULL) //command without option
    {
      getcwd(cwd, maxlen);
      dir = opendir(cwd);

      if (dir != NULL)
      {
        while ((ent = readdir(dir)) != NULL)
        {
          if (ent->d_name[0] == '.')
            continue;
          printf ("%s   ", ent->d_name);
        }
        printf("\n");
        closedir(dir);
      }
      return 1;
    }

    else if (!strcmp(temp, "-a")) //command with option -a
    {
      getcwd(cwd, maxlen);
      dir = opendir(cwd);

      if (dir != NULL)
      {
        while ((ent = readdir(dir)) != NULL)
        {
          printf ("%s   ", ent->d_name);
        }
        printf("\n");
        closedir(dir);
      }
      return 1;
    }
    else
    {
      fprintf(stderr, "invalid option\n");
      return 1;
    }
  }
  return 0;
}
