#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

// cmds is very complicated:
// it is an array of commands
// each commands is an array of strings
// for instance:
// cmds = { ls, wc }
// ls = { "ls", "-l", NULL }
// wc = { "wc", "-l", NULL }
// so this function will be exactly the same
// running ls -l | wc -l
// 呢個 function 尋晚係第個 program 試係得嘅，唔得先改啦
// pipe9.c 淨係得呢個 function，乜嘢都無
void construct_pipeline(char ***cmds) {
  int fds[2];
  pid_t pid;

  // it saves the last read fd, so that the even though
  // fds is written, the next child process can have this read fd
  // 呢個無可能解得清楚，要自己 trace 一次先知
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

// this splits char ** args into char *** cmds
// for example
// if args is {"ls", "-l", "|", "wc", "-l", NULL}
// then cmds should be
// { { "ls", "-l", NULL }, { "wc", "-l", NULL } }
// TODO: THIS ONE IS CURRENTLY NOT WORKING
char ***split_commands(char **args, int argc) {
  char ***cmds = malloc(sizeof(char **) * argc);
  int pipe_start = 0; // starting position of the next command
  int cmds_start = 0; // next cmd to be written to cmds
  int i = 0;
  for (i = 0; i < argc - 1; i++) {
    if (strcmp(args[i], "|") == 0) {
      args[i] = NULL;
      cmds[cmds_start++] = &args[pipe_start++];
    }
  }
  cmds[cmds_start] = NULL; // ending the cmds list
  return cmds;
}

int shell_execute(char **args, int argc) {
  int child_pid, wait_return, status;

  if (strcmp(args[0], "EXIT") == 0)
    return -1;

  if ((child_pid = fork()) < 0) {
    printf("fork() error \n");
  } else if (child_pid == 0) {
    // child
    int cmd_count = 0;
    char ***cmds = split_commands(args, argc);
    // since split_commands is not working right now, cmds is probably wrong
    // so I can't test whether construct_pipeline will work correctly
    // :(
    construct_pipeline(cmds);
    free(cmds);
  } else {
    if ((wait_return = wait(&status)) < 0)
      printf("wait() error \n");
  }

  return 0;
}
