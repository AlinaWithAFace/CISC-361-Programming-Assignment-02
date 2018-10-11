
#include "get_path.h"

int pid;

int sh(int argc, char **argv, char **envp);

void printenv(int num_args, char **envp, char **args);

char *which(char *command, struct pathelement *pathlist);

char *where(char *command, struct pathelement *pathlist);

void *watchmail_thread(void *arg);

void list(char *dir);


#define PROMPTMAX 32
#define MAXARGS 100
