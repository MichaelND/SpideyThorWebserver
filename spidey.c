/* spidey: Simple HTTP Server */

#include "spidey.h"

#include <errno.h>
#include <stdbool.h>
#include <string.h>

#include <unistd.h>

/* Global Variables */
char *Port	      = "9898";
char *MimeTypesPath   = "/etc/mime.types";
char *DefaultMimeType = "text/plain";
char *RootPath	      = "www";
mode  ConcurrencyMode = SINGLE;
char * PROGRAM_NAME = NULL;

/**
 * Display usage message.
 */
void
usage(const char *progname, int status)
{
    fprintf(stderr, "Usage: %s [hcmMpr]\n", progname);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "    -h            Display help message\n");
    fprintf(stderr, "    -c mode       Single or Forking mode\n");
    fprintf(stderr, "    -m path       Path to mimetypes file\n");
    fprintf(stderr, "    -M mimetype   Default mimetype\n");
    fprintf(stderr, "    -p port       Port to listen on\n");
    fprintf(stderr, "    -r path       Root directory\n");
    exit(status);
}

/**
 * Parses command line options and starts appropriate server
 **/
int
main(int argc, char *argv[])
{
    int sfd;

    /* Parse command line options */
    PROGRAM_NAME = argv[0];
    int argind = 1;
    while (argind < argc && strlen(argv[argind]) > 1 && argv[argind][0] == '-') {
        char * arg = argv[argind++];

        if (arg[1] == 'h')
            usage(PROGRAM_NAME, 0);
        else if (arg[1] == 'c') {
            char * temp = argv[argind++];
            if (streq(temp, "single")) {
                ConcurrencyMode = SINGLE;
            }
            else if (streq(temp, "forking")) { 
                ConcurrencyMode = FORKING;
            }
            else
                ConcurrencyMode = UNKNOWN;
        }
        else if (arg[1] == 'm')
            MimeTypesPath = argv[argind++];
        else if (arg[1] == 'M')
            DefaultMimeType = argv[argind++];
        else if (arg[1] == 'p')
            Port = argv[argind++];
        else if (arg[1] == 'r')
            RootPath = argv[argind++];
        else 
            usage(PROGRAM_NAME, 1);
    }

    /* Listen to server socket */
    sfd = socket_listen(Port);
    if (sfd < 0) {
        printf("Failed socket listen");
        return EXIT_FAILURE;
    }

    /* Determine real RootPath */
    RootPath = realpath(RootPath, NULL); // Issues here 

    log("Listening on port %s", Port);
    debug("RootPath        = %s", RootPath);
    debug("MimeTypesPath   = %s", MimeTypesPath);
    debug("DefaultMimeType = %s", DefaultMimeType);
    debug("ConcurrencyMode = %s", ConcurrencyMode == SINGLE ? "Single" : "Forking");

    /* Start either forking or single HTTP server */
    if (ConcurrencyMode == SINGLE) {
        single_server(sfd);
    }
    else if (ConcurrencyMode == FORKING) {
        forking_server(sfd);
    }
    else {
        printf("UNKNOWN ConcurrencyMode");
    }
    return EXIT_SUCCESS;
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
