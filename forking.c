/* forking.c: Forking HTTP Server */

#include "spidey.h"

#include <errno.h>
#include <signal.h>
#include <string.h>

#include <unistd.h>

/**
 * Fork incoming HTTP requests to handle the concurrently.
 *
 * The parent should accept a request and then fork off and let the child
 * handle the request.
 **/
void
forking_server(int sfd)
{
    struct request *request;
    pid_t pid;

    /* Accept and handle HTTP request */
    while (true) {
    	/* Accept request */
        
	/* Ignore children */

	/* Fork off child process to handle request */
        pid_t child = fork();
        if (child < 0) {
            fprintf(stderr, "Unable to fork request: %s\n" strerror(errno));
        }
    }

    /* Close server socket and exit*/
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
