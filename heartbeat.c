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
#include <time.h>

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

    /* Catch, ignore/handle signals */
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

    /* Close all open file descriptors */
    int x;
    for (x = sysconf(_SC_OPEN_MAX); x>0; x--)
    {
        close (x);
    }

}

int main()
{
    heartbeat_daemon();

    /* Custom log file, a+ => create + append */
    FILE *pulseox;
    pulseox = fopen("/var/vcap/sys/log/sawmill/heartbeat.log", "a+");
    if (!pulseox) {
      fprintf(stdout, "Could not open log file for heartbeat daemon.\n" );
      error("Cannot open log file /var/vcap/sys/log/sawmill/heartbeat.log");
    }

    fprintf(pulseox, "heartbeat started\n");
    while (1) {
      /* getting current calendar time */
      time_t ltime;
      ltime=time(NULL);

      /* write to the log, flush the buffer, take a quick nap */
      fprintf(pulseox, "%s --- sawmill --- process pulse\n", asctime(localtime(&ltime)));
      fflush(pulseox);
      sleep(60);
    }
    fclose(pulseox);
    return EXIT_SUCCESS;
}
