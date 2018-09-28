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

#define BUFFER_SIZE 100

void do_nothing_handler(int sig){
    //printf("Caught signal %d\n", sig);
}

int sh(int argc, char **argv, char **envp) {
    char BUFFER [BUFFER_SIZE];

    char *prompt = calloc(PROMPTMAX, sizeof(char));
    //char *commandline = calloc(MAX_CANON, sizeof(char));
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
    
    char prompt_char[BUFFER_SIZE] = "[🔥 👉 😎 👉 🔥] >";
    int len;
    char* string_input;

    //TODo: Segfaults on signal interrupt commmands
    signal(SIGINT, do_nothing_handler);
    signal(SIGTERM, do_nothing_handler);
    signal(SIGTSTP, do_nothing_handler);

    while (go) {
        /* print your prompt */
        //todo, fix memory and garbage stuff

        //TODO: breaks when you input nothing 
        printf("%s", prompt_char);
        
        fgets(BUFFER, BUFFER_SIZE, stdin);
        len = (int)strlen(BUFFER);

        BUFFER[len-1] = '\0';
        string_input = (char*)malloc(len);
        strcpy(string_input, BUFFER);

        if(string_input != NULL){
            char* token = strtok(string_input, " ");
            int i = 0;
            while(token){
                len = (int)strlen(token);
                args[i] = (char*)malloc(len);
                strcpy(args[i], token);
                token = strtok(NULL," ");
                i++;
            }

            if(strcmp(args[0], "exit") == 0){
                go = 0;
            }else if(strcmp(args[0], "which") == 0){
                if(args[1] == NULL){
                    printf("%s", "which: Too few arguments.\n");
                }else{
                    //TODO Impelemnt multiple args
                    char* result = which(args[1], pathlist);
                    if(result != NULL){
                        printf("%s\n", result);
                        free(result);
                    }else{
                        printf("%s not found\n", args[1]);
                    }
                }
            }
            free(token);
        }

        for(i=0;i<MAXARGS;i++){
            free(args[i]);
        }
        free(string_input);
    }

    //Free ALL the things!

    free(prompt);
    free(owd);
    free(args);

    struct pathelement *current;
    current = pathlist;
    //Okay what the hell is going on, if you free the first element
    //It frees like 200 bytes of stuff and if you try to free any
    //other it breaks????????????? But according to valgrind it's okay?????
    free(current->element);
    while(current != NULL){
        free(current);
        current=current->next;
    }


    return 0;
} /* sh() */



char *which(char *command, struct pathelement *pathlist) {
    /* loop through pathlist until finding command and return it.  Return
    NULL when not found. */

    //A buffer for concatting strings
    char CAT_BUFFER[BUFFER_SIZE];
    struct pathelement* current = pathlist;

    DIR *dr;
    struct dirent *de; 
    
    //Go though all our PATH_ENVs
    while(current != NULL){
        
        char* path = current->element;

        //vars for looking though the directories
        dr = opendir(path);
       // printf("Dir closed");

        //in each path, look at all of it's files
        while((de = readdir(dr)) != NULL){

           
            //for each file in the directory, check if it's the one we want
            if(strcmp(de->d_name, command) == 0){
                //cat together the full filename
                strcpy(CAT_BUFFER, path);
                strcat(CAT_BUFFER, "/");
                strcat(CAT_BUFFER, de->d_name);

                //create a string pointer and return it
                int len = (int)strlen(CAT_BUFFER);
                char* p = (char*)malloc(len);
                strcpy(p, CAT_BUFFER);

                closedir(dr);

                return p;
            }
        }
        closedir(dr);
        current=current->next;
    }

   // free(dr);
    //free(de);

    //Return null if we haven't found one 
    return NULL;

}

char *where(char *command, struct pathelement *pathlist) {
    /* similarly loop through finding all locations of command */
} /* where() */

void list(char *dir) {
    /* see man page for opendir() and readdir() and print out filenames for
    the directory passed */
} /* list() */

