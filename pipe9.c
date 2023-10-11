#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void pipeline(char ***cmds) {
  int fds[2];
  pid_t pid;
  int lastfd = 0;

  while (*cmds != NULL) {
    pipe(fds);
    if ((pid = fork()) == -1) {
      perror("fork");
      exit(1);
    } else if (pid == 0) {
      dup2(lastfd, 0);
      if (*(cmds + 1) != NULL) {
        dup2(fds[1], 1);
      }
      close(fds[0]);
      execvp((*cmds)[0], *cmds);
      exit(1);
    } else {
      wait(NULL);
      close(fds[1]);
      lastfd = fds[0];
      cmds++;
    }
  }
}

int main() {
  printf("execution starting...\n");
  char *ls[] = {"ls", "-la", NULL};
  char *tr[] = {"tr", "a-z", "A-Z", NULL};
  char *sort[] = {"sort", NULL};

  char **cmd[] = {ls, tr, NULL};
  // char *ls[] = {"ls", "-al", NULL};
  // char *rev[] = {"rev", NULL};
  // char *nl[] = {"nl", NULL};
  // char *cat[] = {"cat", "-e", NULL};
  // char **cmd[] = {ls, rev, nl, cat, NULL};

  pipeline(cmd);
}
