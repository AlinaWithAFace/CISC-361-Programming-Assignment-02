/* 
 * Sample Project 2
 */

#include "sh.h"
#include <signal.h>
#include <stdio.h>
#include <sys/time.h>

void sig_handler(int signal);

/* 
 * This is the main function that calls
 * the sh function which starts the shell
 *
 */
int main(int argc, char **argv, char **envp) {

    // Set up signal handlers
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);
    signal(SIGTSTP, sig_handler);
    signal(SIGALRM, sig_handler);

    return sh(argc, argv, envp);
}

/* 
 * This is the function which handles signals that
 * are sent to it. (SIGTERM, SIGINT, SIGSTP, SIGALRM)
 *
 */
void sig_handler(int signal) {
    // Print out a newline if SIGINT, SIGTERM, or SIGTSTP are caught
    if (signal == SIGINT) {
        printf("\n");
    } else if (signal == SIGTERM) {
        printf("\n");
    } else if (signal == SIGTSTP) {
        printf("\n");
    }

        // Kill child process if SIGALRM signal is caught
    else if (signal == SIGALRM) {
        kill(cpid, 9);
        printf("command taking too long to execute\n");
    }
}
