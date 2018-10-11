/* 
 * Nathan Hamilton, Patrick Hough 
 * CISC 361-010
 * 3/10/2016
 * Project 2
 */

#include "get_path.h"

/* This is the data structure that holds the previously issued command
 * Has fields for the currently issued command, the previous command,
 * and the next command as well
 */
struct prev_cmd {
    char *cmd;
    struct prev_cmd *next;
    struct prev_cmd *prev;
};

/* The data structure for used for the 'alias' built in function
 * has fields (name, cmd) for the name of the alias, the command for the alias to execute
 * a pointer (next) to the next alias
 * and a flag (used) used for determining alias loops
 */
struct alias {
    char *name;
    char *cmd;
    struct alias *next;
    int used;
};

//the process id of the child process
pid_t cpid;

//the integer representation of the process id
int pid;

/* function call to start the shell 
 * argc: the argument count 
 * argv: the array of the arguments given 
 * envp: the array of pointers to environment variables
 */
int sh(int argc, char **argv, char **envp);

/* function call for the which command
 * command: the command that you're looking for 
 * pathlist: the path data structure
 * alist: the alias data structure
 */
char *which(char *command, struct pathelement *pathlist, struct alias *alist);

/* function call for the where command
 * command: the command that you're checking
 * pathlist: the path data structure
 * alist: the alias data structure
 */
char *where(char *command, struct pathelement *pathlist, struct alias *alist);

/* function call for the list command
 * dir: the directory that was given to the list command
 */
void list(char *dir);

/* function call for the kill command
 * argsct: the number of arguments given
 * args: the arguments given to the command
 */
void my_kill(int argsct, char **args);

/* function call for the prompt command
 * argsct: the number of arguments given
 * args: the arguments given to the command
 */
char *reset_prompt(int argsct, char **args);

/* function call for the printenv command
 * argsct: the number of arguments given
 * args: the arguments given to the command
 * envp: array of pointers to environment variables
 */
void print_envir(int argsct, char **args, char **envp);

/* function call that adds any entered commands into the history of the shell
 * head: the top of the history data structure
 * cmd: the command that will be put into the history data structure
 */
struct prev_cmd *add_to_history(struct prev_cmd *head, char *cmd);

/* function call that prints the history of previously given commands
 * argsct: number of arguments given
 * args: the actual arguments that were input
 * head: the head of the history data structure
 */
void print_hist(int argsct, char **args, struct prev_cmd *head);

/* function that calls the change directory built in command
 * argsct: the number of arguments given
 * args: the actual arguments that were input
 * prev: the previous directory
 */
int cd(int argsct, char **args, char *prev);

/*function that adds an alias to the shell
 * argsct: the number of arguments
 * args: the given arguments
 * alist: the linked list of aliases
 */
struct alias *add_alias(int argsct, char **args, struct alias *alist);

/* function that adds user specified variables to the environment
 * argsct: the number of arguments
 * args: the given arguments
 * pathlist: linked list representation of current path
 * envp: the array containing pointers to the environment variables
 */
char **set_envir(int argsct, char **args, struct pathelement *pathlist, char **envp);

/* recursive function used for wildcard matching
 * s1: the first string to compare
 * s2: the second string to compare
 */
int matches(char *s1, char *s2);

#define PROMPTMAX 32
#define MAXARGS 1000
