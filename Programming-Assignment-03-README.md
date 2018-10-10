Programming Assignment 3
========================

**Adding More Features to the Shell**  


Objective
---------

The main objective of this assignment is to adding more functionalities to your shell from Project 2. So the first step is to get your shell from Project 2 working, or at least enough to implement features for this project, if it doesn't already work. The assignment introduces programming with POSIX threads (pthreads), including _**mutual exclusion**_ with pthreads. Experience with some more system calls and C library functions will also be done to hone your programming skills. Reading of **man pages** will also be a must to do well.  
Try [sample code](https://www.eecis.udel.edu/~cshen/361/Proj_3/sample-code) to experiment more.

### The Assignment

*   Implement the ability to **background** a 'job' using '**&**'. This needs to only work for executing _external_ commands. What you need to do is to identify that the last thing given on the command line is a '&'. The backgrounded job is executed **concurrently** with the shell, rather than having the shell wait for it to complete. The shell should go and print another prompt to wait for the next command. It is possible that several backgrounded jobs could be running concurrently at the same time.  
    
    When a parent process does **not** (get the chance to) do a `wait(2)` call for its children, the children will become **zombie** processes (marked by <defunct> in Linux or Z) when they exit. To prevent this for your backgrounded jobs, you need to do a _nonblocking_ `waitpid(2)` call at some point. See the man page for options to use to reap an entry in a nonblocking fashion. It is suggested that you do this before printing the next prompt. If the parent has a zombie child, the call will reap the process entry, otherwise no harm is done either. Check out sample [zombie code](https://www.eecis.udel.edu/~cshen/361/Proj_3/zombie.c).
    

**Extra Credit (10 points)**: Implement the 'fg' built-in command to bring a backgrounded job into the foreground. With no arguments, a default backgrounded job will be chosen. It should also take an argument to specify which backgrounded job to bring to the foreground. Add your own test to show this works and clearly document if you implement this. Your shell should work just like when a job isn't backgrounded when a job is brought into the foreground. Refer to Sections 9.7 and 9.8 of Stevens and Rago's APUE book.

*   Add the **`watchuser`** built-in command. This command takes as its first argument a username to keep track of when they log on,  similar to the `watch` shell variable in tcsh (review [tcsh shell variables](http://www.ibm.com/developerworks/aix/library/au-tcsh/)). This command also takes a second optional argument of "off" to stop watching a user. The first time **`watchuser`** is ran, a (new) thread should be created/started via **`pthread_create(3)`**, which runs a `while (1)` loop with a `sleep(3)` call for 20 seconds in it to track logins of users. Only one **`watchuser`** thread should ever be running. The thread should get the list of users from a global linked list which the calling function (of the main thread) will modify by either inserting new users or turning off existing watched users. For instance,
    
    mysh % watchuser cshen
    mysh % watchuser smith
    mysh % watchuser joe
    mysh % watchuser smith off
    
    Sample code to obtain the list of currently logged in users can be found [here](https://www.eecis.udel.edu/~cshen/361/Proj_3/showusers.c) .
    
    Since the calling function (of the main thread) and the `watchuser` thread have access to share data (e.g., the global linked list of users to watch), the shared data must be protected with a **mutex** lock using `pthread_mutex_lock(3)` and `pthread_mutex_unlock(3)`. When a user from the watch list logs on to a new tty, this thread should printout something to the terminal of "USER has logged on TTY from HOST."
    
    **Extra Credit (5 points)**: Also have your thread notify when the users being watched log off of a tty. Make it clear that you do this if you do, and document it well.
    
*   Add the **`watchmail`** built-in command. This command takes as the first argument a name of a file, which must already exist (give an error if it doesn't), to watch for new \`mails' in. It can take an optional second argument of "off" to turn off of watching of mails for that file. It should start up a new thread using `pthread_create(3)` for each file to watch for mail in (the name of the file is passed as a parameter to the thread). This function, to watch for new mail, should run a `while (1)` loop, which will do a `stat(2)` to see if the file got bigger since the last check. A sleep for 1 second should be in the loop. The `st_size` member of a `stat` struct can be used to check the size of the file. When the file increases in size, the thread, for that file, should print out:  
      
    `BEEP You've Got Mail in FILE at TIME`  
      
    The BEEP should be done with a `\a` in `printf()`. FILE should be the name of the file that the thread is watching. And TIME should be the current time in `ctime(3)` format. To get the current time use `gettimeofday(3)` and pass `tv_sec` of the `timeval` struct to `ctime()`.

```
    mysh % watchmail mbox
    mysh % watchmail /usr/cshen/pobox
    mysh % watchmail mbox off
```
    
When the "off" parameter is given the thread that is watching that file needs to be canceled with `pthread_cancel(3)` . A linked list of active threads and files associated with each one should be maintained to support this. When an error condition happens in a thread, `pthread_exit(3)` should be used. Your program should support watching multiple files. A separate thread will run to check each file.  
    

Note that these two threaded "watch" commands will print messages when certain things happen, which could be annoying when using the shell for normal functioning. However if you have multiple windows open, they could be useful to run in one for notification. As always, document your code well and explain how things work.

*   Add support to your shell so the following file redirections will work: (review [OSTEP Chapter 5.4 Why? Motivating The API](http://pages.cs.wisc.edu/~remzi/OSTEP/cpu-api.pdf) for details)  
    
    *   **\>** - redirect standard output of command on the left to file on the right (i.e. command > file)
    *   **\>&** - redirect both standard output and standard error of command on the left to file on the right (i.e. command >& file).
    *   **\>>** - redirect standard output of command on the left to file on the right (i.e. command >> file), appending to the file.
    *   **\>>&** - redirect both standard output and standard error of command on the left to file on the right (i.e. command >>& file), appending to the file.
    *   **<** - redirect standard input of command on left to come from file on the right (i.e. command < file).
    
    To do the file redirection requires that you understand file descriptors well and how a child process inherits file descriptors from its parent. You will need to close and reopen file descriptors 0, 1 and 2. For this you will need to use the `open()`, `close()` and `dup()` system calls. (Refer to Section 3.12 of Stevens and Rago's APUE book for dectails of `dup()`.) Please use `dup(2)` instead of `dup2(3)`. This doesn't require a lot of code, just the right calls in the right places. To get the normal function of stdin, stdout and stderr back, opening of `/dev/tty` will be required. Be sure to use the right options for `open()` in each situation (i.e. read/write, truncate/append). Read man pages!
    
    To help out in testing both stdoue and stderr, there is a small program [here](https://www.eecis.udel.edu/~cshen/361/Proj_3/test-1+2.c). Compile as test-1+2 and use it in test runs.  
    Code example to redirect stdout to a file:
    
```
    	fid = open(filename,O\_WRONLY|O\_CREAT|O\_TRUNC);  
    	close(1);  
    	dup(fid);  
    	close(fid);  
```
    
To redirect stdout back to the screen, replace the `open()` with:
    
```
fid = open("/dev/tty",O\_WRONLY);  
```
    
and repeat the other 3 calls.
    
Additionally, add a **noclobber** command which will affect how these operators handle file creation. All it should do is change a variable ([tcsh shell variables](http://www.ibm.com/developerworks/aix/library/au-tcsh/)), `noclobber` (Prevent accidental overwriting of existing files), from 0 to 1 or from 1 to 0 and print out the new value of it. This variable should default to 0 and cause your shell to act the same way as csh/tcsh does with respect to the `noclobber` variable. That is when it is 0 > and >& will overwrite existing files and >> and >>& will create the file if it doesn't exist. When `noclobber` is 1 the shell should refuse to overwrite existing files, refuse to create a file for appending and print the same error messages csh/tcsh do in those situations.  

```
    \[cisc361:/usa/cshen/public\_html/361/Proj\_3 216\] tcsh
    cisc361\[31\] \[~/public\_html/361/Proj\_3/\]> touch out-file
    cisc361\[32\] \[~/public\_html/361/Proj\_3/\]> set noclobber
    cisc361\[33\] \[~/public\_html/361/Proj\_3/\]> echo hello > out-file
    out-file: File exists.
    cisc361\[34\] \[~/public\_html/361/Proj\_3/\]> unset noclobber
    cisc361\[35\] \[~/public\_html/361/Proj\_3/\]> echo hello > out-file
    cisc361\[36\] \[~/public\_html/361/Proj\_3/\]> more out-file
    hello
    cisc361\[37\] \[~/public\_html/361/Proj\_3/\]> 
```

*   Add support for interprocess communication (IPC). This is adding support for **|** and **|&**. The | operator should pipe standard output of the command on the left to standard input of the command on the right. (i.e. command1 | command2). For |& standard error should be piped as well as standard output. It is only required that this works when the command on the right is an external command, the one on the left could be either built-in or external. You also need to avoid generating zombie processes!

```
    \[cisc361.acad.ece.udel.edu:/usa/cshen 132\] tcsh
    cisc361\[29\] \[~/\]> which echo
    echo: shell built-in command.
    cisc361\[30\] \[~/\]> echo hello world | wc
           1       2      12
    cisc361\[31\] \[~/\]> 
    
    cisc361\[34\] \[~/361/2018/apue.3e/intro/\]> 
    cisc361\[34\] \[~/361/2018/apue.3e/intro/\]> ls X
    X: No such file or directory
    cisc361\[35\] \[~/361/2018/apue.3e/intro/\]> ls X | wc
    X: No such file or directory
           0       0       0
    cisc361\[36\] \[~/361/2018/apue.3e/intro/\]> ls X |& wc
           1       6      29
    cisc361\[37\] \[~/361/2018/apue.3e/intro/\]> 
```

To implement this, you will need to use `pipe(2)`. To implement pipes, file descriptor redirection will be required again by using `close()` and `dup()`. (Refer to Section 3.12 of Stevens and Rago's APUE book for dectails of `dup()`.) However you will be getting open file descriptors by using `pipe(2)` instead of `open()`. This task is a bit trickier than the previous, but again does not take much code. It will take some trials and thinking to get right. You will probably need to move your command running code to a new function which takes info about the style of the pipe (if any) and which side of the pipe this command will be on. The left side of the pipe needs to not cause the parent to do a blocking wait, while the right side does.
    
Once the parent process (your shell) has created a pipe and forked the two child processes that will write and read from the pipe, make sure the parent explicitly closes the file descriptors for its read and write access to the pipe. If you fail to do this, the child reading from the pipe will never terminate. This is because the child reading from the pipe will never get the END-OF-FILE (end-of-pipe) condition (and hence never terminate) as long as one process (in this case your shell program, by mistake) has an open write file descriptor for the pipe. The overall result will be a deadlock - your shell waiting for a child to terminate and the child waiting for the shell to close the pipe, which was inadvertently left open. (Review [Pipes](http://beej.us/guide/bgipc/html/multi/pipes.html) and Section 9.9 of Stevens and Rago's Unix ebook for dectails.)
    
This part will also require that your command line processing puts the arguments into two separate argument vectors. And do not use `popen()`, you will get no credit.  
    
*   As always document your code well and explain how things work. It is only necessary that one of &, the file redirections and piping work at a time for this project. Remember that man pages are your friends and you may find using `truss(1)` useful.

### Test Runs

To test '&' show running some commands in the background in your script. To test watchmail you can simply make the file larger in another window a couple of times. Also test for turning off watching a file. And to test watchuser, come up with your own tests. The TAs will test each feature.

Test your shell by running the following commands in it (in order):

```
    pwd  
    ls &  
    ls -l &  
    cd /  
    sleep 20 &  
    ls & 			; run before sleep is done  
    pid  
    tty  
    /bin/ps -lfu USERNAME	; replace USERNAME with your own    
    cd  
    cd \[project test dir of your choosing\]  
    pwd  
    ls -l  
    rm -f mail1 mail2  
    touch mail1			; create this file  
    watchmail mail1  
    echo hi >> mail1  
    echo HiThere > mail2	; create another file  
    watchmail mail2  
    echo there >> mail1  
    echo Subject: >> mail2  
    cat mail1  
    cat mail2  
    watchmail mail1 off  
    echo bye >> mail1  
    echo bye >> mail2		; still watching this one  
  
    rm -f test1 test2 test3 test4 test5 test6 test7 test8  
    test-1+2 > test1  
    test-1+2 >> test2  
    test-1+2 >& test3  
    test-1+2 >>& test4  
    cat test1 test2 test3 test4  
    test-1+2 > test1  
    test-1+2 >> test2  
    test-1+2 >& test3  
    test-1+2 >>& test4  
    cat test1 test2 test3 test4  
  
    noclobber				; turn noclobber on  
    test-1+2 > test5  
    test-1+2 >> test6  
    test-1+2 >& test7  
    test-1+2 >>& test8  
    cat test5 test6 test7 test8  
    test-1+2 > test5  
    test-1+2 >> test6  
    test-1+2 >& test7  
    test-1+2 >>& test8  
    cat test5 test6 test7 test8  
  
    grep hello < test8  
    grep error < test8  
  
    rm -f test9 test10 test11 test12  
    noclobber				; turn noclobber off  
    test-1+2 > test9  
    test-1+2 >> test10  
    test-1+2 >& test11  
    test-1+2 >>& test12  
    cat test9 test10 test11 test12  
  
    ls | fgrep .c                   ; show pipes works  
    ./test-1+2 | grep  hello  
    ./test-1+2 |& grep hello  
    ./test-1+2 | grep output  
    ./test-1+2 |& grep output  
    ./test-1+2 |& grep error  
  
    pid                                        ; zombie avoidance checking  
    /bin/ps -lfu USERNAME | grep defunct       ; replace USERNAME with your username  
```

If your file redirection doesn't work then use another shell to modify your mailfiles to test your watchmail command. The TA will also test each feature.

### Grading

*   20% for adding & support (including avoiding zombies)
*   15% for watchmail
*   15% for watchuser (for log ins and mutex locking)
*   20% for file redirection
*   20% for interprocess communications
*   10% documentation and code structure (remember to check error situations and avoid too much duplicate code)
*   10% extra credit for fg
*   5% extra credit for watchuser (for log outs)

### Turn In

You need to tar up your source code and submit the tar file so that your shell can be tested and graded. To do this,

1\. Put all the source files of your project #3 into a subdirectory named YourLoginName\_3.  
  
2\. At the current working directory, do  
      tar cvf YourLoginName\_3.tar YourLoginName\_3  

To verify that your files are in the tar file take a look at the table of contents of the tar file like:

tar tvf YourLoginName\_3.tar

Then submit your tar file to Canvas.

