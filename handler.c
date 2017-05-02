/* handler.c: HTTP Request Handlers */

#include "spidey.h"

#include <errno.h>
#include <limits.h>
#include <string.h>

#include <dirent.h>
#include <unistd.h>

/* Internal Declarations */
http_status handle_browse_request(struct request *request);
http_status handle_file_request(struct request *request);
http_status handle_cgi_request(struct request *request);
http_status handle_error(struct request *request, http_status status);

/**
 * Handle HTTP Request
 *
 * This parses a request, determines the request path, determines the request
 * type, and then dispatches to the appropriate handler type.
 *
 * On error, handle_error should be used with an appropriate HTTP status code.
 **/
http_status
handle_request(struct request *r)
{
    http_status result;
    request_type type;
    char * path;
    char * type_str;

    /* Parse request */
    parse_request(r); 

    /* Determine request path */
    path = determine_request_path(r->uri);
    debug("HTTP REQUEST PATH: %s", path);

    /* Dispatch to appropriate request handler type */
    type = determine_request_type(path);
    if (type == REQUEST_BROWSE)
        type_str = "BROWSE";
    else if (type == REQUEST_CGI)
        type_str = "CGI";
    else if (type == REQUEST_FILE)
        type_str = "FILE";
    else if (type == REQUEST_BAD)
        type_str = "BAD";

    debug("HTTP REQUEST TYPE: %s", type_str);

    if (strcmp(http_status_string(result), "200 OK") != 0) //error has occurred
        handle_error(r, result);

    log("HTTP REQUEST STATUS: %s", http_status_string(result));
    return result;
}

/**
 * Handle browse request
 *
 * This lists the contents of a directory in HTML.
 *
 * If the path cannot be opened or scanned as a directory, then handle error
 * with HTTP_STATUS_NOT_FOUND.
 **/
http_status
handle_browse_request(struct request *r)
{
    struct dirent **entries;
    int n;
    DIR * directory;
    char str[BUFSIZ];

    /* Open a directory for reading or scanning */
    //Dir * directory = opendir();

    directory = opendir(RootPath);

    // if (directory == NULL) { //check if directory is openable
    //     return HTTP_STATUS_NOT_FOUND;
    // }

    /* Write HTTP Header with OK Status and text/html Content-Type */
    fprintf(r->file,"HTTP/1.0 %s\nContent-Type: test/html\n<html>\n<body>\n", HTTP_STATUS_OK);
    //debug("HTTP HEADER %s = %s", HTTP_STATUS_OK, 
    /* For each entry in directory, emit HTML list item */
    n = scandir(RootPath, &entries, NULL, alphasort);
    if (n == -1) {
        
    }
    int i = 0;
    fprintf(r->file, "<ul>\n");
    while (i < n) {
        sprintf(str, "<li>%s</li>\n", entries[i]->d_name);
        fprintf(r->file,str);
        str[0] = 0; // resets str
        i++;
    }
    fprintf(r->file, "</ul>\n</body>\n</hmtl>\n");
    /* Flush socket, return OK */
    closedir(directory); //close
    shutdown(r->file, SHUT_WR);

    return HTTP_STATUS_OK;
}

/**
 * Handle file request
 *
 * This opens and streams the contents of the specified file to the socket.
 *
 * If the path cannot be opened for reading, then handle error with
 * HTTP_STATUS_NOT_FOUND.
 **/
http_status
handle_file_request(struct request *r)
{
    FILE *fs;
    char buffer[BUFSIZ];
    char *mimetype = NULL;
    size_t nread;

    /* Open file for reading */
    fs = fopen(RootPath, "r");
    if (fs == NULL) {
        return HTTP_STATUS_NOT_FOUND;
    }
    /* Determine mimetype */
    mimetype = determine_mimetype(RootPath);
    /* Write HTTP Headers with OK status and determined Content-Type */
    fprintf(r->file,"HTTP/1.0 %s\nContent-Type: %s\n<html>\n<body>\n", HTTP_STATUS_OK, mimetype); 
    /* Read from file and write to socket in chunks */
    while (nread = fread(buffer,BUFSIZ,1,fs) != 0) {
        fwrite(buffer, nread, 1, r->file);
    }
    fprintf(r->file, "</body>\n</hmtl>\n");

    /* Close file, flush socket, deallocate mimetype, return OK */
    fclose(fs);
    shutdown(r->file, SHUT_WR);
    free(mimetype);
    return HTTP_STATUS_OK;
}

/**
 * Handle file request
 *
 * This popens and streams the results of the specified executables to the
 * socket.
 *
 *
 * If the path cannot be popened, then handle error with
 * HTTP_STATUS_INTERNAL_SERVER_ERROR.
 **/
http_status
handle_cgi_request(struct request *r)
{
    FILE *pfs;
    char buffer[BUFSIZ];
    struct header *header;

    /* Export CGI environment variables from request:
    * http://en.wikipedia.org/wiki/Common_Gateway_Interface */

    /* Export CGI environment variables from request headers */

    /* POpen CGI Script */


    /* Copy data from popen to socket */

    /* Close popen, flush socket, return OK */
    return HTTP_STATUS_OK;
}

/**
 * Handle displaying error page
 *
 * This writes an HTTP status error code and then generates an HTML message to
 * notify the user of the error.
 **/
http_status
handle_error(struct request *r, http_status status)
{
    const char *status_string = http_status_string(status);

    /* Write HTTP Header */

    /* Write HTML Description of Error*/

    /* Return specified status */
    return status;
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
