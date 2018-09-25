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

void do_nothing_handler(int sig){
    //printf("Caught signal %d\n", sig);
}

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
    //printf("%s", pathlist->next->element);
    
    char prompt_char[BUFFER_SIZE] = "[🔥 👉 😎 👉 🔥] >";
    int len;
    char* string_input;
    signal(SIGINT, do_nothing_handler);
    signal(SIGTERM, do_nothing_handler);
    signal(SIGTSTP, do_nothing_handler);

    while (go) {
        /* print your prompt */
        //todo, fix memory and garbage stuff

        //breaks when you input ntohing
        printf("%s", prompt_char);
        
        fgets(BUFFER, BUFFER_SIZE, stdin);
        len = (int)strlen(BUFFER);
        //printf("%d", len);
        BUFFER[len-1] = '\0';
        string_input = (char*)malloc(len);
        strcpy(string_input, BUFFER);
        //printf("%s", string_input);
        if(string_input != NULL){
            char* token = strtok(string_input, " ");
            char** args;
            args = calloc(10, sizeof(char*));
        //args[0] = token;
            int i = 0;
            while(token){
            //printf("%s\n",token);
                len = (int)strlen(token);
                args[i] = (char*)malloc(len);
                strcpy(args[i], token);
                token = strtok(NULL," ");
                i++;
            }
        

        //printf("%s", args[0]);

            if(strcmp(args[0], "exit") == 0){
                go = 0;
            }else if(strcmp(args[0], "which") == 0){
                if(args[1] == NULL){
                    printf("%s", "which: Too few arguments.\n");
                }else{
                    //TODO Impelemnt multiple args
                    char* result = which(args[1], pathlist);
                    printf("%s\n", result);
                }
            }

        //switch(S)

        
       // for(int i = 0; i < 10; i++){
         //   if(args[i] != NULL)
          //      printf("%s", args[i]);
        //}
       
        //    printf("%s",input_tokenized);
        //    input_tokenized = strtok(NULL, " ");
        //}

        //char* token = strtok(BUFFER, " ");    
        //while(token){
        //    printf("%s", token);
        //}

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
    }
    return 0;
} /* sh() */



char *which(char *command, struct pathelement *pathlist) {
    /* loop through pathlist until finding command and return it.  Return
    NULL when not found. */

    //A buffer for concatting strings
    char CAT_BUFFER[BUFFER_SIZE];
    struct pathelement* current = pathlist;
    
    //Go though all our PATH_ENVs
    while(current != NULL){
        char* path = current->element;

        //vars for looking though the directories
        DIR *dr;
        struct dirent *de; 
        dr = opendir(path);

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

                return p;
            }
        }
        current=current->next;
    }

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

