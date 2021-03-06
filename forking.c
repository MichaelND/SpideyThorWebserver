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

    /* Accept and handle HTTP request */
    while (true) {
        /* Accept request */
        request = accept_request(sfd);
        if (request == NULL)
            continue;
        
    	/* Ignore children */
        pid_t child = fork();
        if (child > 0)
            debug("Child Process: %d", (int)child);

        if (child < 0) {
            fprintf(stderr, "Unable to fork request: %s\n", strerror(errno));
            fclose(request->file);
            continue;
        }
    	/* Fork off child process to handle request */
        if (child == 0) { // Child
            /* Handle client request */
            handle_request(request);
            free_request(request);
            exit(EXIT_SUCCESS);
        } 
        else {        // Parent
            free_request(request);
        }
    }
    /* Close server socket and exit*/
    close(sfd);
    return;
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
