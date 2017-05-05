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
    if (parse_request(r) == -1) {
        handle_error(r, HTTP_STATUS_BAD_REQUEST);
    }
    
    /* Determine request path */
    path = determine_request_path(r->uri);
    if (path == NULL) {
        handle_error(r, HTTP_STATUS_NOT_FOUND);
    }
    debug("HTTP REQUEST PATH: %s", path);

    /* Dispatch to appropriate request handler type */
    type = determine_request_type(path);
    r->path = path;
    if (type == REQUEST_BROWSE) {
        type_str = "BROWSE";
        result = handle_browse_request(r);
    }
    else if (type == REQUEST_CGI) {
        type_str = "CGI";
        result = handle_cgi_request(r);
    }
    else if (type == REQUEST_FILE) {
        type_str = "FILE";
        result = handle_file_request(r);
    }
    else if (type == REQUEST_BAD) {
        type_str = "BAD";
        result = handle_error(r, HTTP_STATUS_NOT_FOUND);
    }

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

    /* Open a directory for reading or scanning */
    n = scandir(r->path, &entries, NULL, alphasort);

    if (n < 0) { //check if directory is openable
        return HTTP_STATUS_NOT_FOUND;
    }
    /* Write HTTP Header with OK Status and text/html Content-Type */
    fprintf(r->file,"HTTP/1.0 %i\nContent-Type: text/html\n\r\n<html>\n<body>\n", HTTP_STATUS_OK);

    /* For each entry in directory, emit HTML list item */
    int i = 0;
    fprintf(r->file, "<h4>Directory:</h4>\n<ul>\n");
    while (i < n) {
        if (strcmp(entries[i]->d_name, ".")!=0) {
            fprintf(r->file, "<li><a href=\"%s/%s\">%s</a></li>\n", streq(r->uri, "/") ? "" : r->uri, entries[i]->d_name, entries[i]->d_name);
        }
        free(entries[i]);
        i++;
    }
    free(entries);

    fprintf(r->file, "</ul>\n</body>\n</html>\n");
    /* Flush socket, return OK */
    fflush(r->file);

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
    fs = fopen(r->path, "r");
    if (fs == NULL) {
        return HTTP_STATUS_NOT_FOUND;
    }
    /* Determine mimetype */
    mimetype = determine_mimetype(r->path);

    /* Write HTTP Headers with OK status and determined Content-Type */
    fprintf(r->file,"HTTP/1.0 %i\nContent-Type: %s\n\r\n", HTTP_STATUS_OK, mimetype); 

    /* Read from file and write to socket in chunks */
    while ((nread = fread(buffer, sizeof(char), BUFSIZ, fs)) != 0) {
        fwrite(buffer, sizeof(char), nread, r->file);
    }

    /* Close file, flush socket, deallocate mimetype, return OK */
    fclose(fs);
    fflush(r->file);
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

    setenv("QUERY_STRING", r->query, 1);
    setenv("REMOTE_ADDR", r->host, 1);
    setenv("REMOTE_PORT", r->port, 1);
    setenv("REQUEST_URI", r->uri, 1);
    setenv("REQUEST_METHOD", r->method, 1);
    setenv("SCRIPT_FILENAME",r->path, 1);

    /* Export CGI environment variables from request headers */
    setenv("DOCUMENT_ROOT", RootPath, 1);
    header = r->headers;            

    while (header) {
        if (streq(header->name, "Port"))
            setenv("SERVER_PORT", header->value, 1);
        if (streq(header->name, "Host"))
            setenv("HTTP_HOST", header->value, 1);
        if (streq(header->name, "Accept"))
            setenv("HTTP_ACCEPT", header->value, 1);
        if (streq(header->name, "Accept-Language"))
            setenv("HTTP_ACCEPT_LANGUAGE", header->value, 1);
        if (streq(header->name, "Accept-Encoding"))
            setenv("HTTP_ACCEPT_ENCODING", header->value, 1);
        if (streq(header->name, "Connection"))
            setenv("HTTP_CONNECTION", header->value, 1);
        if (streq(header->name, "User-Agent"))
            setenv("HTTP_USER_AGENT", header->value, 1);
        header = header->next;
    }
    /* POpen CGI Script */

    //sprintf(command, ".%s", r->path);
    pfs = popen(r->path, "r");

    /* Copy data from popen to socket */
    while (fgets(buffer, BUFSIZ, pfs)) {
        write(r->fd, buffer, strlen(buffer));
    }
    /* Close popen, flush socket, return OK */
    pclose(pfs);
    fflush(r->file);
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
    fprintf(r->file,"HTTP/1.0 %s\nContent-Type: text/html\r\n<html>\n<body>\n", status_string);

    /* Write HTML Description of Error*/
    if (strcmp(status_string, "400 Bad Request") == 0)
        fprintf(r->file,"You made a bad request.");
    else if (strcmp(status_string, "404 Not Found") == 0)
        fprintf(r->file,"You can not find what you are looking for.");
    else if (strcmp(status_string, "500 Internal Server Error") == 0)
        fprintf(r->file,"Oops, there was a problem.");
    
    fprintf(r->file, "</body>\n</hmtl>\n");
    /* Return specified status */
    return status;
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
