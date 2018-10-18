
#include "get_path.h"

int pid;

int sh(int argc, char **argv, char **envp);

void printenv(int num_args, char **envp, char **args);

void execute_external(char** args, char* pathlist, int num_args, char** envp, int message);

char *which(char *command, struct pathelement *pathlist);

char *where(char *command, struct pathelement *pathlist);

void *watchmail_thread(void *arg);

void list(char *dir);

char *redirect(char *source, char *destination, int redirectError);

#define PROMPTMAX 32
#define MAXARGS 100
