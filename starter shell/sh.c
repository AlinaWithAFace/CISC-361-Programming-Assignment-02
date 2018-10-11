/*
 * Sample Project 2
 *
 */

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
#include <glob.h>
#include <sys/time.h>

pid_t cpid;

/* function that starts the shell
 * argc: argument count
 * argv: the array of arguments given
 * envp: the array of ponters to environment variables
 */
int sh(int argc, char **argv, char **envp) {
    //Setting up variables needed for the shell to function
    char *prompt = calloc(PROMPTMAX, sizeof(char));
    char *prefix = "";
    char *commandline = calloc(MAX_CANON, sizeof(char));
    char *command, *arg, *commandpath, *p, *pwd, *owd;
    char **args = calloc(MAXARGS, sizeof(char *));
    int uid, i, status, argsct, go = 1;
    struct passwd *password_entry;
    char *homedir;

    struct pathelement *pathlist;
    int count;
    char *token;
    struct prev_cmd *head = NULL;
    struct alias *alist = NULL;
    int space;
    int valid;
    uid = getuid();
    password_entry = getpwuid(uid);               /* get passwd info */
    homedir = password_entry->pw_dir;        /* Home directory to start
						  out with*/

    // store the current working directory
    if ((pwd = getcwd(NULL, PATH_MAX + 1)) == NULL) {
        perror("getcwd");
        exit(2);
    }
    owd = calloc(strlen(pwd) + 1, sizeof(char));
    memcpy(owd, pwd, strlen(pwd));

    // Set up the initial prompt
    prompt[0] = ' ';
    prompt[1] = '[';
    prompt[2] = '\0';

    strcat(prompt, pwd);
    prompt[strlen(prompt) + 3] = '\0';
    prompt[strlen(prompt)] = ']';
    prompt[strlen(prompt)] = '>';
    prompt[strlen(prompt)] = ' ';

    /* Put PATH into a linked list */
    pathlist = get_path();

    while (go) {
        /* print your prompt */
        valid = 1;
        printf("%s%s", prefix, prompt);

        // Read in command line
        if (fgets(commandline, MAX_CANON, stdin) == NULL) {
            commandline[0] = '\n';
            commandline[1] = '\0';
            valid = 0;
            printf("\n");
        }
        int space = 1;

        // Remove newline character from end of input
        if (strlen(commandline) > 1) {
            commandline[strlen(commandline) - 1] = '\0';
        } else {
            valid = 0;
        }

        // Check command for special cases
        for (i = 0; i < strlen(commandline); i++) {
            if (commandline[i] != ' ') {
                space = 0;
            }
        }
        if (space) {
            commandline[strlen(commandline) - 1] = '\n';
            valid = 0;
        }

        // Parse the command line to separate the arguments
        count = 1;
        args[0] = strtok(commandline, " ");
        while ((arg = strtok(NULL, " ")) != NULL) {
            args[count] = arg;
            count++;
        }
        args[count] = NULL;
        argsct = count;

        // Check if command is an alias
        struct alias *curr = alist;
        int found = 0;
        int done = 0;
        if (argsct == 1) {
            while (curr != NULL && !done) {
                found = 0;
                if (strcmp(curr->name, args[0]) == 0) {
                    strcpy(commandline, curr->cmd);
                    found = 1;
                    count = 1;
                    args[0] = strtok(commandline, " ");
                    while ((arg = strtok(NULL, " ")) != NULL) {
                        args[count] = arg;
                        count++;
                    }
                    args[count] = NULL;
                    argsct = count;
                    if (curr->used == 1) {
                        args[0] = "\n\0";
                        printf("Alias Loop\n");
                        done = 1;
                    }
                    curr->used = 1;
                }
                curr = curr->next;
                if (found) {
                    curr = alist;
                }
            }
        }

        // Reset (used) aspect of each alias struct
        curr = alist;
        while (curr != NULL) {
            curr->used = 0;
            curr = curr->next;
        }

        // Check for each built in command
        command = args[0];
        if (strcmp(command, "exit") == 0) {
            // Exit the shell
            printf("Executing built-in exit\n");
            exit(0);
        } else if (strcmp(command, "which") == 0) {
            // Finds first alias or file in path directory that
            // matches the command
            printf("Executing built-in which\n");
            if (argsct == 1) {
                fprintf(stderr, "which: Too few arguments.\n");
            } else {
                // Checks for wildcard arguments
                glob_t globber;
                int i;
                for (i = 1; i < argsct; i++) {
                    int globreturn = glob(args[i], 0, NULL, &globber);
                    if (globreturn == GLOB_NOSPACE) {
                        printf("glob: Runnning out of memory.\n");
                    } else if (globreturn == GLOB_ABORTED) {
                        printf("glob: Read error.\n");
                    } else {

                        if (globber.gl_pathv != NULL) {
                            which(globber.gl_pathv[0], pathlist, alist);
                        } else {
                            which(args[i], pathlist, alist);
                        }
                    }
                }
            }
        } else {

            // If the command is not an alias or built in function, find it
            // in the path
            int found = 0;
            int status;
            char *toexec = malloc(MAX_CANON * sizeof(char));
            if (access(args[0], X_OK) == 0) {
                found = 1;
                strcpy(toexec, args[0]);
            } else {
                struct pathelement *curr = pathlist;
                char *tmp = malloc(MAX_CANON * sizeof(char));

                while (curr != NULL & !found) {
                    snprintf(tmp, MAX_CANON, "%s/%s", curr->element, args[0]);
                    if (access(tmp, X_OK) == 0) {
                        toexec = tmp;
                        found = 1;
                    }
                    curr = curr->next;
                }
            }

            // If the command if found in the path, execute it as a child process
            if (found) {

                printf("Executing %s\n", toexec);

                // Create a child process
                cpid = fork();

                struct itimerval timer;


                if (cpid == 0) {
                    // Child process executes command
                    execve(toexec, args, envp);
                } else if (cpid == -1) {
                    perror(NULL);
                } else {
                    // Parent process (shell) times child process
                    if (argc > 1) {
                        timer.it_value.tv_sec = atoi(argv[1]);
                        timer.it_interval.tv_sec = 0;
                        if (setitimer(ITIMER_REAL, &timer, NULL) == -1) {
                            perror(NULL);
                        }
                    }

                    // Parent process (shell) waits for child process to terminate
                    waitpid(pid, &status, 0);

                    // Disable timer after child process terminates
                    if (argc > 1) {
                        timer.it_value.tv_sec = 0;
                        if (setitimer(ITIMER_REAL, &timer, NULL) == -1) {
                            perror(NULL);
                        }
                    }

                    // Return non-normal exit status from child
                    if (WIFEXITED(status)) {
                        if (WEXITSTATUS(status) != 0) {
                            printf("child exited with status: %d\n", WEXITSTATUS(status));
                        }
                    }
                }
            }

                // Give error if command not found
            else if (valid) {
                fprintf(stderr, "%s: Command not found.\n", args[0]);
            }
        }
    }
    return 0;
} /* sh() */

/* function call for 'which' command
 * command: the command that you're checking
 * pathlist: the path list data structure
 * alist: the alias data structure
 */
char *which(char *command, struct pathelement *pathlist, struct alias *alist) {
    // Set up path linked list variables
    struct pathelement *curr = pathlist;
    char *path = malloc(MAX_CANON * (sizeof(char)));
    int found = 0;


    // Search aliases for command
    struct alias *curra = alist;
    while (curra != NULL) {
        if (strcmp(curra->name, command) == 0) {
            printf("%s:\t aliased to %s\n", curra->name, curra->cmd);
            found = 1;
        }
        curra = curra->next;
    }

    // Search path for command
    while (curr != NULL && !found) {
        strcpy(path, curr->element);
        path[strlen(path) + 1] = '\0';
        path[strlen(path)] = '/';
        strcat(path, command);
        if (access(path, F_OK) == 0) {
            printf("%s\n", path);
            found = 1;
        }
        curr = curr->next;
    }

    // Print error if command not found
    if (!found) {
        fprintf(stderr, "%s: command not found\n", command);
        return NULL;
    }

} /* which() */

