#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "sh.h"

#define MAX_CANON 32
#define PATH_MAX 32
#define BUFFER_SIZE 100

int sh(int argc, char **argv, char **envp) {
    char BUFFER [BUFFER_SIZE];

    char *prompt = calloc(PROMPTMAX, sizeof(char));
    char *commandline = calloc(MAX_CANON, sizeof(char));
    char *command, *arg, *commandpath, *p, *pwd, *owd;
    char **args = calloc(MAXARGS, sizeof(char *));
    int uid, i, status, argsct, go = 1;
    struct passwd *password_entry;
    char *homedir;
    struct pathelement *pathlist;

    uid = getuid();
    password_entry = getpwuid(uid);               /* get passwd info */
    homedir = password_entry->pw_dir;        /* Home directory to start
						  out with*/

    if ((pwd = getcwd(BUFFER, BUFFER_SIZE + 1)) == NULL) {
        perror("getcwd");
        exit(2);
    }
    owd = calloc(strlen(pwd) + 1, sizeof(char));
    memcpy(owd, pwd, strlen(pwd));
    prompt[0] = ' ';
    prompt[1] = '\0';

    /* Put PATH into a linked list */
    pathlist = get_path();

    
    char prompt_char[BUFFER_SIZE] = "[🔥 🔥 🔥] >";
    while (go) {
        /* print your prompt */
        printf("%s", prompt_char);
        fgets(BUFFER, BUFFER_SIZE, stdin);

        /* get command line and process */

        /* check for each built in command and implement */

        /*  else  program to exec */
        //{
            /* find it */
            /* do fork(), execve() and waitpid() */

        //    else
        //    fprintf(stderr, "%s: Command not found.\n", args[0]);
        //}
    }
    return 0;
} /* sh() */

char *which(char *command, struct pathelement *pathlist) {
    /* loop through pathlist until finding command and return it.  Return
    NULL when not found. */

} /* which() */

char *where(char *command, struct pathelement *pathlist) {
    /* similarly loop through finding all locations of command */
} /* where() */

void list(char *dir) {
    /* see man page for opendir() and readdir() and print out filenames for
    the directory passed */
} /* list() */

