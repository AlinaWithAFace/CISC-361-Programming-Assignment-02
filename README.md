Programming Assignment 2
========================

## Writing a Shell

### Objectives

The objectives of this project are to learn how a Unix shell works, to write a simple shell, to create processes, to handle signals, to use Unix system calls and C library function calls, and to become a better programmer.  

### Background readings

*   Wikipedia articles on [Unix shells](http://en.wikipedia.org/wiki/Unix_shell) and [tcsh](http://en.wikipedia.org/wiki/TENEX_C_shell).
*   Man pages for Unix system calls and C library function calls.
*   Official [tcsh man page](http://www.unix.com/man-page/OSX/1/tcsh/).
*   [shell skeleton code](https://www.eecis.udel.edu/~cshen/361/Proj_2/skeleton-code/) to get you started.
*   [my random sample code](https://www.eecis.udel.edu/~cshen/361/Proj_2/sample-code/) to get you to experiment more.

### The Assignment

You will write a simple shell in C with some "built-in" commands.  

#### Overview

What you need to do is to have a loop that will print a prompt (consisting of a "prefix," which is initially a blank (space), followed by ' `[cwd]>` ', where `cwd` is the "current working directory," and will execute commands until the `exit` command is entered. In the loop you should check if the command is one of your built-in commands (given later) or a program in the search paths. You also need to support passing arguments to the commands (i.e. you will need to build an argv array). If the command entered is neither one of your built-in commands nor in the search paths, you should print "`command`: Command not found." where `command` is the command name that was entered.

#### More Details

*   Obviously, you will need to do some parsing of the command line entered. A good way to do this would be to use `fgets(3C)` to read in the (entire) command line, then use `strtok(3C)` with a space as the delimiter. The first "word" will be the command (we'll ignore the possibility that a command could have a space in its name). All words after that will be arguments to be passed to the command (which you will need to put into a `char**`).
*   After you get the command, check if it is one of your built-in commands (explained below). If it is, then run code for that.
*   If it is not one of your built-in commands, check if it is an absolute path (a command starts with '`/`') or a path starts with '`./`', '`../`', _etc_., and run that if it is executable (use `access(2)` to check).
*   If the command is neither of the above cases then search for the command in the search path by looping through the path stored as a linked list as given in the [skeleton code](https://www.eecis.udel.edu/~cshen/361/Proj_2/skeleton-code/get_path.c). You may also use your own linked list code. Use `access(2)` in a loop to find the first executable in the path for the command. `snprintf(3C)` would be useful to use here (using `strcat()` has caused problems for several people).
*   Once you find the command you should execute it, using `execve(2)`. You also need to have the shell do a `waitpid(2)` and print out the return status of the command if it is nonzero like `tcsh` does when the `printexitvalue` shell variable is set. Look into using the WEXITSTATUS macro from [<sys/wait.h>](http://pubs.opengroup.org/onlinepubs/9699919799/functions/wait.html).
*   Before executing a command your shell should also print out what it is about to execute. (i.e. print "Executing \[_pathname_\]"; for built-ins print "Executing built-in \[_built-in command name_\]")
*   Ctrl-C (SIGINT) should be caught and ignored if the shell is prompting for a command, but sent to the running child process otherwise. Use `signal(3C)` and/or `sigset(3C)` for this. Ctrl-Z (SIGTSTP), and SIGTERM should be ignored using `sigignore(3C)` or `signal(3C)`. **Note that when you are running a command inside your shell and press control-C, signal SIGINT is sent to both your shell process and the running command process (_i.e._, all the processes in the _foreground_ process group)**. (Review Sections 9.4 (Process Groups), 9.5 (Sessions), and 9.6 (Controlling Terminal) of Stevens and Rago's [ebook](http://proquestcombo.safaribooksonline.com/book/programming/unix/9780321638014) for details.)
*   You need to support the `*` wildcard character when a single `*` is given. You do not need to handle the situation when `*` is given with a `/` in the string (i.e., /usr/include/\*.h). This should work just like it does in csh/tcsh when noglob is not set. You need only to support the possibility of one `*` on your commandline, but it could have chars prepended and/or appended. (i.e., `ls *` should work correctly as should `ls *.c`, `ls s*` and `ls p*.txt`.) Be sure to document how you do this. Hint: implement the `list` built-in command explained below before attempting this. You may use `glob(3C)` if you wish. **Note that it is YOUR shell's responsibility to expands wildcards to matching (file) names. (If there were not matches, the "original" arguments are passed to `execve()`.)**
*   You also need to support the `?` wildcard character which should match any single character (exactly one character match) in a filename (anywhere in the filename). The `*` and `?` should also work together.
*   Your code should do proper error checking. Again check man pages for error conditions, and call `perror(3C)` as needed. Also avoid memory leaks by calling `free(3C)` as needed.
*   Your shell should treat Ctrl-D and the [EOF](http://www.computerhope.com/jargon/e/eof.htm) char in a similar way csh/tcsh do when the `ignoreeof` [tcsh shell variable](http://www.ibm.com/developerworks/aix/library/au-tcsh/) is set, that is ignore it, instead of exiting or seg faulting. Note that Ctrl-D is not a signal, but the EOF char.
(Please review the **_difference_** between [Shell Variables and Environment Variables](http://sc.tamu.edu/help/general/unix/vars.html).)

#### Built-in Commands to support

*   **`exit`** - obvious
*   **`which`** - same as the tcsh one (hint: you can write a function to do this and use it for finding the command to execute)
*   **`where`** - same as the tcsh one (reports all known instances of _command_ in path)
*   **`cd`** - `chdir(2)` to directory given; with no arguments, chdir to the home directory, with a '-' as the only argument, chdirs to directory **previously in**, the same as tcsh does.
*   **`pwd`** - print the current working directory.
*   **`list`** - with no arguments, lists the files in the current directory one per line. With arguments, lists the files in each directory given as an argument, with a blank line then the name of the directory followed by a : before the list of files in that directory. You will need to use `opendir(3C)` and `readdir(3C)`. (Hint: read their man pages carefully, and refer to Fig. 1.3 of the Unix ebook)
*   **`pid`** - prints the pid of the shell
*   **`kill`** - When given just a pid sends a SIGTERM to it with `kill(2)`. When given a signal number (with a - in front of it) sends that signal to the pid. (e.g., `kill 5678`, `kill -9 5678`).
*   **`prompt`** - When ran with no arguments, prompts for a new prompt prefix string. When given an argument make that the new prompt prefix. For instance, let's assume `cwd` is `/usa/cshen`.

```
 \[/usa/cshen\]> prompt CISC361
CISC361 \[/usa/cshen\]> \_
CISC361 \[/usa/cshen\]> cd 361
CISC361 \[/usa/cshen/361\]> prompt YES
YES \[/usa/cshen/361\]> prompt
  input prompt prefix: hello
hello \[/usa/cshen/361\]> \_
```

*   **`printenv`** - Should work the same as the tcsh built-in one. When ran with no arguments prints the whole environment (This can be done in 2 lines of code, a `printf()` inside of a `while` loop, not counting a variable declaration). When ran with one argument, call `getenv(3C)` on it. When called with two or more args print the same error message to stderr that tcsh does.
*   **`alias`** \- Should work the same as the tcsh built-in one. When ran with no arguments prints the aliases the shell knows about. When ran with arguments it should install an alias into the alias table the shell knows about. Additionally, the shell needs to be able to run your aliases to receive full credit. Supply your own test cases to show that it works.
*   **`history`** \- Should print the last n commands (by default n is 10) executed when ran with no options. When ran with a numerical argument changes the number of commands to list to that number.
*   **`setenv`** \- Should work the same as the tcsh built-in one. When ran with no arguments, prints the whole environment, the same as `printenv`. When ran with one argument, sets that as an empty environment variable. When ran with two arguments, the second one is the value of the first. When ran with more args, print the same error message to stderr that tcsh does. You can use the `setenv(3C)` function for this command. Special care must be given when PATH and HOME are changed. When PATH is changed, be sure to update your linked list for the path directories (and free() up the old one). When HOME is changed `cd` with no arguments should now go to the new home. Provide your own test runs to show this works.
(Review Stevens and Rago's [Section 7.9 Environment Variables](http://proquestcombo.safaribooksonline.com/book/programming/unix/9780321638014/7dot-process-environment/ch07lev1sec9_html) for details.)

#### How to get started

It is recommended to first get the loop working to find a command, i.e., implement _which_ first. Then you will be able to create a new process with `fork(2)` and use `execve(2)` in the child process and `waitpid(2)` in the parent. Then process arguments and do the other built-ins. Remember to read man pages for system and library calls, include the corresponding header files.  
  
Skeleton code to get started with is [here](https://www.eecis.udel.edu/~cshen/361/Proj_2/skeleton-code/). Example code of using `fork(2)` and `exec(2)` can be found [here](https://www.eecis.udel.edu/~cshen/361/Proj_2/sample-code/).  

#### Some more library functions that may be helpful

`atoi(3C)`, `fprintf(3C)`, `index(3C)`, `calloc(3C)`, `malloc(3C)`, `memcpy(3C)`, `memset(3C)`, `getcwd(3C)`, `strncmp(3C)`, `strlen(3C)`.

### Test Runs

Test your shell by running the following commands in it (in order):  
```
\[return\]
Ctrl-D
Ctrl-Z
Ctrl-C
which					; test which
which ls
ls					; execute it
\[return\]
Ctrl-D					; make sure still work
Ctrl-Z
Ctrl-C
ls -l					; test passing arguments
ls -l -a /kernel/genunix
ls -l -F /kernel/genunix
ls -l -F /etc/rpc
ls -l -a /etc/rpc
where					; test where
where ls
/usr/ucb/ls -l -g			; test absolutes and passing args
/usr/ucb/ls -l
/usr/bin/ls -l -g
/usr/bin/ls -l
file \*					; test out \* wildcard
ls \*
ls \*.c
ls -l sh.\*
ls -l s\*.c
ls -l s\*h.c
ls sh.?					; test out ? wildcard
ls ?h.c	
ls \*.?					; test combination of ? and \*	
blah					; try a command that doesn't exist
/blah					; an absolute path that doesn't exist
ls -l /blah
/usr/bin				; try to execute a directory
/bin/ls -la /
file /bin/ls /bin/rm
which ls rm				; test multiple args
where ls rm
list					; test list
list / /usr/proc/bin
cd 					; cd to home
pwd
cd /blah				; non-existant
cd /usr/bin /usr/ucb			; too many args
cd -					; should go back to project dir
pwd
more sh.c   (and give a Crtl-C)		; more should exit
cd /usr/bin
pwd
./ls /					; test more absolutes
../bin/ls /
history					; test history
history 15
history
history 5
history
pid					; get pid for later use
kill
kill pid-of-shell			; test default
kill -1 pid-of-shell			; test sending a signal, should kill
					; the shell, so restart a new one
prompt	    (and enter some prompt prefix)
prompt 361shell
(provide your own test cases to so that alias works)
printenv PATH
printenv
setenv
setenv TEST
printenv TEST
setenv TEST testing
printenv TEST
setenv TEST testing more
setenv HOME /
cd
pwd
exit
```

### Notes

If you are developing this project under Linux or another Unix system, some of the test paths above may not exist. In that case use similar or equivalent file names for that system. But be sure your program works on MLB (Linux) for your final run to turn in. The TA will be testing your shell on MLB. The script of the command testing should be done on MLB.

The next project builds upon this project. It is important to have minimal functionality of this project in order to complete the next project.

### Extra Credits

Turn your shell into one that will restrict the run time of executing processes. Your shell will execute a command as a new process, but if the process exceeds a 'timeout,' it will be killed. You will start your shell with one argument stating a timeout value. For instance,

```
$ ./myshell 5
mysh >> cat
!!! taking too long to execute this command !!!
mysh >>
```

**You may NOT use multi-threading to implement this feature.**

* * *

### Grading

*   90% correctly working shell ([checklist](https://www.eecis.udel.edu/~cshen/361/Proj_2/checklist.html))
*   10% documentation and code structure (remember to check error situations and avoid too much duplicate code)

### Turn In

You need to tar up your source code, test run script file, etc. To do this, do the following.

*   In the current working directory, create a (sub-)directory named `proj2` to store all your programs.
*   `tar cvf YourName\_proj2.tar proj2`
    

To verify that your files are in the tar file take a look at the table of contents of the tar file like:

`tar tvf YourName\_proj2.tar`