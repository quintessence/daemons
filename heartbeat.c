/*
 * heartbeat.c
 * This is a simple heartbeat daemon process.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>

static void heartbeat_daemon()
{
    pid_t pid;

    /* Fork off the parent process */
    pid = fork();

    /* An error occurred */
    if (pid < 0)
        exit(1);

    /* Success: Let the parent terminate */
    if (pid > 0)
        exit(0);

    /* On success: The child process becomes session leader */
    if (setsid() < 0)
        exit(1);

    /* Catch, ignore and handle signals */
    //TODO: Implement a working signal handler */
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    /* Fork off for the second time*/
    pid = fork();

    /* An error occurred */
    if (pid < 0)
        exit(1);

    /* Success: Let the parent terminate */
    if (pid > 0)
        exit(0);

    /* Set new file permissions */
    umask(0);

    /* Change the working directory to the root directory */
    /* or another appropriated directory */
    chdir("/");

    /* Close all open file descriptors */
    int x;
    for (x = sysconf(_SC_OPEN_MAX); x>0; x--)
    {
        close (x);
    }

    /* Open the log file */
    openlog ("heartbeat", LOG_PID, LOG_DAEMON);
}

int main()
{
    heartbeat_daemon();

    syslog (LOG_NOTICE, "heartbeat started");
    while (1)
    {
        syslog (LOG_NOTICE, "process pulse");
        sleep(60);
    }

    syslog (LOG_NOTICE, "heartbeat terminated");
    closelog();

    return EXIT_SUCCESS;
}
