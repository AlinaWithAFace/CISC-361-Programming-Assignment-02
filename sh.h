
#include "get_path.h"

void process_command(enum commands c, char *full_args);

int pid;

int sh(int argc, char **argv, char **envp);

void printenv(int num_args, char **envp, char **args);

char *which(char *command, struct pathelement *pathlist);

char *where(char *command, struct pathelement *pathlist);

void list(char *dir);

char *redirect(char *source, char *destination, int redirectError);

#define PROMPTMAX 32
#define MAXARGS 10
