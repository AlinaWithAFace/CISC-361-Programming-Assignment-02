/*
    James Skripchuk & Alina Christenbury
    CISC361
    Shell
*/

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <limits.h>
#include <stdlib.h>
#include <pwd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include "sh.h"
#include <glob.h>
#include "linked_list.h"

#define BUFFER_SIZE 1000
#define MAX_COMMAND_HISTORY 999
#define MAX_ALIAS 10

int sh(int argc, char **argv, char **envp) {

    //Defining variables
    //Temp buffer
    char BUFFER[BUFFER_SIZE];

    char *prompt = calloc(PROMPTMAX, sizeof(char));
    char *command, *arg, *commandpath, *p, *pwd, *owd, *cwd;
    char **args = calloc(MAXARGS, sizeof(char *));

    struct Node *history = NULL;
    struct Node *alias = NULL;

    int uid, i, status, argsct, go = 1;
    struct passwd *password_entry;
    char *homedir;
    struct pathelement *pathlist;

    uid = getuid();
    password_entry = getpwuid(uid);               /* get passwd info */
    homedir = password_entry->pw_dir;        /* Home directory to start out with*/

    if ((pwd = getcwd(BUFFER, BUFFER_SIZE + 1)) == NULL) {
        perror("getcwd");
        exit(2);
    }

    //Current working directory
    cwd = calloc(strlen(pwd) + 1, sizeof(char));
    owd = calloc(strlen(pwd) + 1, sizeof(char));
    memcpy(owd, pwd, strlen(pwd));
    memcpy(cwd, pwd, strlen(pwd));
    prompt[0] = ' ';
    prompt[1] = '\0';


    /* Put PATH into a linked list */
    pathlist = get_path();

    int len;
    char *string_input;

    sigignore(SIGINT);
    sigignore(SIGTERM);
    sigignore(SIGTSTP);
    char *prompt_prefix = (char *) malloc(0);

    //Enums used for switch statements
    typedef enum commands {
                EXIT,
                WHICH,
                WHERE,
                CD,
                GET_CWD,
                PWD,
                LIST,
                PID,
                KILL,
                PROMPT,
                PRINT_ENV,
                ALIAS,
                HISTORY,
                SET_ENV,
                command_count
            } commands;

    char *command_strings[] = {
            "exit", "which", "where", "cd", "getcwd", "pwd", "list", "pid", "kill", "prompt", "printenv",
            "alias", "history", "setenv"
    };


    /*
        Main Shell Loop
    */
    while (go) {

        //Null out our argument array
        for (int j = 0; j < MAXARGS; j++) {
                args[j] = NULL;
        }

        //Print prompt
        printf("%s[%s]>", prompt_prefix, cwd);

        fgets(BUFFER, BUFFER_SIZE, stdin);
        len = (int) strlen(BUFFER);

        //Empty input has length of 1, so check if we have more before continuing
        if (len >= 2) {

            //Set up strings for manipulation
            BUFFER[len - 1] = '\0';
            string_input = (char *) malloc(len);
            char* string_input_alias_find = (char*)malloc(len);
            strcpy(string_input, BUFFER);
            strcpy(string_input_alias_find, BUFFER);
            history = append(history, string_input, NULL);

            char *token = strtok(string_input_alias_find, " ");
            char* alias_find_result = find(alias, token);
            

            //What we want to do
            //Get the first command from the input
            //See if it is in the alias table
            //If so replace it with the one in the alias table

            int num_args = 0;

            //If we have an alias for this command
            if(alias_find_result != NULL){
                //We found it in our alias table, so we have to replace it.
                char* alias_token = strtok(alias_find_result, " ");

                //Fill up arg array with alias substitution
                //Preshifts argumets if we have a token
                while(alias_token){
                    len = (int) strlen(alias_token);
                    args[num_args] = (char *) malloc(len);
                    strcpy(BUFFER, alias_token);
                    strcpy(args[num_args], BUFFER);
                    alias_token = strtok(NULL, " ");
                    num_args++;
                }

                free(alias_token);
            }

            free(alias_find_result);
            free(string_input_alias_find);
            
            //Reset token to string
            token = strtok(string_input," ");

            if(alias_find_result != NULL){
                token = strtok(NULL, " ");
            }

            //Now we want to flesh out the rest of the array
            //Whether or not we had a token
            while (token) {

                //If we have a wildcard, expand it out
                if (strstr(token, "*") != NULL || strstr(token, "?") != NULL) {
                    glob_t paths;
                    int csource;

                    csource = glob(token, 0, NULL, &paths);

                    char **p;

                    if (csource == 0) {
                        for (p = paths.gl_pathv; *p != NULL; ++p) {
                            len = (int) strlen(*p);
                            args[num_args] = (char *) malloc(len);
                            strcpy(args[num_args], *p);
                            num_args++;
                        }
                        
                        globfree(&paths);
                    }
                
                //Else we just add it all to the array
                } else {
                    len = (int) strlen(token);
                    args[num_args] = (char *) malloc(len);
                    strcpy(args[num_args], token);
                }

                token = strtok(NULL, " ");
                num_args++;
            }

            
            
            

            //Compare to strings in the enum to get to the command
            int command_index = 0;

            for (command_index = 0; command_index < command_count; ++command_index) {
                if (strcmp(args[0], command_strings[command_index]) == 0) {
                    break;
                }
            }

            switch (command_index) {
                //Exit the shell
                case EXIT:
                    go = 0;
                    break;
                case WHICH:
                    if (args[1] == NULL) {
                        printf("%s", "which: Too few arguments.\n");
                    } else {
                        //Iterate though all following args
                        //Print out more than one if it exits
                        for (int i = 1; i < MAXARGS; i++) {
                            if (args[i] != NULL) {
                                char *result = which(args[i], pathlist);
                                if (result != NULL) {
                                    printf("%s\n", result);
                                    free(result);
                                } else {
                                    printf("%s not found\n", args[i]);
                                }
                            } else {
                                break;
                            }
                        }
                    }
                    break;
                case WHERE:
                    if (args[1] == NULL) {
                        printf("%s", "where: Too few arguments.\n");
                    } else {
                        //Iterate though all following args
                        //Print out more than one if it exits
                        for (int i = 1; i < MAXARGS; i++) {
                            if (args[i] != NULL) {
                                char *result = where(args[i], pathlist);
                                if (result != NULL) {
                                    printf("%s\n", result);
                                    free(result);
                                } else {
                                    printf("%s not found\n", args[i]);
                                }
                            } else {
                                break;
                            }
                        }
                    }
                    break;
                case CD:
                    //Change Directories
                    printf("");
                    char *cd_path = args[1];

                    if(num_args > 2){
                        perror("cd: Too many arguments");
                    }else{
                        //If we have one arg change to the home directory
                        if (num_args == 1) {
                            cd_path = homedir;
                        //Else change to the directory we give it 
                        } else if(num_args == 2) {
                            cd_path = args[1];
                        }
                        

                        //In theory we just want to swap the owd with the cwd
                        //But lots of code for swapping two strings because everyone loves C
                        if ((pwd = getcwd(BUFFER, BUFFER_SIZE + 1)) == NULL) {
                            perror("getcwd");
                            exit(2);
                        }

                        if (cd_path[0] == '-') {
                            if (chdir(owd) < 0) {
                                printf("Invalid Directory: %d\n", errno);
                            } else {
                                free(cwd);
                                cwd = malloc((int) strlen(owd));
                                strcpy(cwd, owd);


                                free(owd);
                                owd = malloc((int) strlen(BUFFER));
                                strcpy(owd, BUFFER);
                            }
                        } else {
                            if (chdir(cd_path) < 0) {
                                printf("Invalid Directory: %d\n", errno);
                            } else {
                                free(owd);
                                owd = malloc((int) strlen(BUFFER));
                                strcpy(owd, BUFFER);

                                if ((pwd = getcwd(BUFFER, BUFFER_SIZE + 1)) == NULL) {
                                    perror("getcwd");
                                    exit(2);
                                }

                                free(cwd);
                                cwd = malloc((int) strlen(BUFFER));
                                strcpy(cwd, BUFFER);
                            }
                        }
                    }
                    break;
                case PWD:
                    //Print working directory
                    printf("%s\n", cwd);
                    break;
                case LIST:
                    //List current directory for one arg
                    if (num_args == 1) {
                        list(cwd);
                    //List all of them for the others
                    } else {
                        for (int i = 1; i < MAXARGS; i++) {
                            if (args[i] != NULL) {
                                printf("[%s]:\n", args[i]);
                                list(args[i]);
                            }
                        }
                    }
                    break;
                case PID:
                    //Print pid of shell
                    printf("");
                    int pid = getpid();
                    printf("%d\n", pid);
                    break;
                case KILL:
                    //If three args, we have a flag and a pid
                    if (num_args == 3) {
                        char *pid_str = args[2];
                        char *signal_str = args[1];

                        char *end;
                        long pid_num;
                        long sig_num;

                        //using strtol because it supports error catching
                        pid_num = strtol(pid_str, &end, 10);
                        //converting pid
                        if (end == pid_str) {
                            printf("%s\n", "Cannot convert string to number");
                        }
                        //get rid of the - flag
                        signal_str[0] = ' ';
                        sig_num = strtol(signal_str, &end, 10);

                        if (end == signal_str) {
                            printf("%s\n", "Cannot convert string to number");
                        }

                        int id = (int) pid_num;
                        int sig = (int) sig_num;
                        kill(id, sig_num);
                    //If its two args just send the default SIGTERM
                    } else if (num_args == 2) {
                        char *pid_str = args[1];
                        char *end;
                        long num;
                        num = strtol(pid_str, &end, 10);
                        if (end == pid_str) {
                            printf("%s\n", "Cannot convert string to number");
                        }
                        int id = (int) num;
                        kill(id, SIGTERM);
                    } else {
                        printf("%s\n", "kill: Incorrect amount of arguments");
                    }
                    break;
                case PROMPT:
                    //Switch the prompt to a new string
                    free(prompt_prefix);
                    if (num_args == 1) {
                        fgets(BUFFER, BUFFER_SIZE, stdin);
                        len = (int) strlen(BUFFER);
                        BUFFER[len - 1] = '\0';
                        prompt_prefix = (char *) malloc(len);
                        strcpy(prompt_prefix, BUFFER);
                    } else if (num_args == 2) {
                        prompt_prefix = (char *) malloc(strlen(args[1]));
                        strcpy(prompt_prefix, args[1]);
                    }
                    break;
                case PRINT_ENV:
                    //Print our enviornment
                    printenv(num_args, envp, args);
                    break;
                case ALIAS:
                    //Set up alias table
                    if(num_args == 1){
                        traverse(alias, MAX_COMMAND_HISTORY, 1);
                    }else if(num_args == 2){
                    
                    }else{
                        char ALSBUF[BUFFER_SIZE];
                        strcpy(ALSBUF, "");
                        for(int i = 2; i < MAXARGS; i++){
                            if(args[i] != NULL){   
                                //Sprintf produced some strange results so use strcat
                                strcat(ALSBUF, args[i]);
                                strcat(ALSBUF, " ");
                            }else{
                                break;
                            }
                        }

                        int len = strlen(ALSBUF);
                        ALSBUF[len-1] = '\0';

                        alias = append(alias, args[1], ALSBUF);
                    }
                    break;
                case HISTORY:
                    //Convert str to int and print that many commands
                    if (num_args == 2) {
                        char *args_str = args[1];
                        long args_num;
                        char *end;

                        args_num = strtol(args_str, &end, 10);
                        if (end == args_str) {
                            printf("%s\n", "Cannot convert string to number");
                        } else {
                            int arg_int = (int) args_num;
                            traverse(history, arg_int, 0);
                        }
                    //Default print last 10
                    } else if (num_args == 1){
                        traverse(history, 10, 0);
                    } else {
                        printf("%s\n", "history: Invalid number of arguments");
                    }
                    break;
                case SET_ENV:
                    //Print env if zero args
                    if (num_args == 1) {
                        printenv(num_args, envp, args);
                    } else if (num_args == 2) {
                    //Set to empty
                        setenv(args[1], "", 1);
                    } else if (num_args == 3) {
                    //Reset vars
                        setenv(args[1], args[2], 1);

                    //special care for home and path
                        if (strcmp(args[1], "HOME") == 0) {
                            homedir = getenv("HOME");
                        } else if (strcmp(args[1], "PATH") == 0) {
                            pathlist = get_path();
                        }
                    } else {
                        printf("%s\n", "setenv: Incorrect amount of arguments");
                    }
                    break;
                default:
                    //Asumme user wants to run an actual command
                    printf("");
                    char *cmd_path;

                    //Check to see if we are an absolute
                    if (args[0][0] == '.' || args[0][0] == '/') {
                        cmd_path = (char *)malloc(strlen(args[0]));
                        strcpy(cmd_path, args[0]);
                    } else {
                        cmd_path = which(args[0], pathlist);
                    }

                    //If the command exits  and we can run it...
                    int access_result = access(cmd_path, F_OK|X_OK);

                    //Run it
                    struct stat path_stat;
                    stat(cmd_path, &path_stat);
                    
                    //Makes sure it's a file
                    if(access_result == 0 && S_ISREG(path_stat.st_mode)){
                        if (cmd_path != NULL) {
                            printf("[Executing built-in %s from %s...]\n", args[0], cmd_path);
                            pid_t child_pid = fork();
                            
                            if (child_pid == 0) {
                                int ret = execve(cmd_path, args, envp);
                            }
                            
                            int child_status;
                            waitpid(child_pid, &child_status, 0);
                            
                        } else {
                            printf("%s: Command not found\n", args[0]);
                        }
                    }else{
                        printf("%s\n", "Invalid Command");
                        printf("Access Error: %i\n", errno);
                    }

                    free(cmd_path);
            }
            free(token);

            //Null our array
            for (int j = 0; j < MAXARGS; j++) {
                free(args[j]);
                args[j] = NULL;
            }

            free(string_input);
        }
    }

    //Free ALL the variables
    freeAll(history);
    freeAll(alias);
    free(prompt);
    free(owd);
    free(cwd);
    free(args);
    free(prompt_prefix);

    struct pathelement *current;
    current = pathlist;

    //Very strange behavior here
    //Apparently you only have to free the first element of the path list and it's okay?
    //No memory leaks reported...
    free(current->element);

    //Free the rest of the nodes
    while (current != NULL) {
        free(current);
        current = current->next;
    }

    return 0;
} /* sh() */


//Print enviornment variables
void printenv(int num_args, char **envp, char **args) {
    if (num_args == 1) {
        int i = 0;
        while (envp[i] != NULL) {
            printf("%s\n", envp[i]);
            i++;
        }
    } else if (num_args == 2) {
        char *env_str = getenv(args[1]);
        if (env_str != NULL) {
            printf("%s\n", env_str);
        }
    }
}


char *which(char *command, struct pathelement *pathlist) {
    /* loop through pathlist until finding command and return it.  Return
    NULL when not found. */

    //A buffer for concatting strings
    char CAT_BUFFER[BUFFER_SIZE];
    struct pathelement *current = pathlist;

    DIR *dr;
    struct dirent *de;

    //Go though all our PATH_ENVs
    while (current != NULL) {

        char *path = current->element;

        //vars for looking though the directories
        dr = opendir(path);

        //in each path, look at all of it's files
        while ((de = readdir(dr)) != NULL) {


            //for each file in the directory, check if it's the one we want
            if (strcmp(de->d_name, command) == 0) {
                //cat together the full filename
                strcpy(CAT_BUFFER, path);
                strcat(CAT_BUFFER, "/");
                strcat(CAT_BUFFER, de->d_name);

                //create a string pointer and return it
                int len = (int) strlen(CAT_BUFFER);
                char *p = (char *) malloc(len);
                strcpy(p, CAT_BUFFER);

                closedir(dr);

                return p;
            }
        }
        closedir(dr);
        current = current->next;
    }


    //Return null if we haven't found one 
    return NULL;

}

char *where(char *command, struct pathelement *pathlist) {
    char CAT_BUFFER[BUFFER_SIZE];
    struct pathelement *current = pathlist;

    DIR *dr;
    struct dirent *de;
    strcpy(CAT_BUFFER, "");

    //Go though all our PATH_ENVs
    while (current != NULL) {

        char *path = current->element;

        //vars for looking though the directories
        dr = opendir(path);
        // printf("Dir closed");

        //in each path, look at all of it's files
        while ((de = readdir(dr)) != NULL) {


            //for each file in the directory, check if it's the one we want
            if (strcmp(de->d_name, command) == 0) {
                //If it is add it to the buffer
                strcat(CAT_BUFFER, path);
                strcat(CAT_BUFFER, "/");
                strcat(CAT_BUFFER, de->d_name);
                strcat(CAT_BUFFER, "\n");
            }
        }
        closedir(dr);

        current = current->next;
    }

    int len = (int) strlen(CAT_BUFFER);
    char *p = (char *) malloc(len);

    //replace last '\n' with null terminator
    CAT_BUFFER[len - 1] = '\0';
    strcpy(p, CAT_BUFFER);

    return p;
} /* where() */


//List all files in given dir
void list(char *dir) {

    DIR *dr;
    struct dirent *de;
    dr = opendir(dir);
    if (dr == NULL) {
        printf("Cannot open %s\n", dir);
    } else {
        while ((de = readdir(dr)) != NULL) {
            printf("%s\n", de->d_name);
        }
    }


    closedir(dr);
} /* list() */

