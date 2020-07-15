/**
 *  @file: http_responses.c
 *  @brief: Biblioteca que cria respostas HTTP
 * 
 *  @author: Isabella Bologna - 9267161
 *  @author: Renato Freitas   - 9837708
*/


#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "http_responses.h"
#include "utils.h"

void ok(char* response, char* type) {
    sprintf(response, "HTTP/1.0 200 OK\r\n");
    sprintf(response+strlen(response), "Content-type: %s\r\n", type);
    sprintf(response+strlen(response), "\r\n");
    // sprintf(response, "<P>Hello</P>\r\n");
}

void bad_request(char* response) {
    sprintf(response, "HTTP/1.0 400 BAD REQUEST\r\n");
    sprintf(response+strlen(response), "Content-type: text/html\r\n");
    sprintf(response+strlen(response), "\r\n");
    sprintf(response+strlen(response), "<P>Your browser sent a bad request.</P>\r\n");


}

void not_found(char* response) {
    sprintf(response, "HTTP/1.0 404 NOT FOUND\r\n");
    sprintf(response+strlen(response), "Content-type: text/html\r\n");
    sprintf(response+strlen(response), "\r\n");
    sprintf(response+strlen(response), "<P>File not found.</P>\r\n ");
}

void not_supported(char* response) {
    sprintf(response, "HTTP/1.0 505 HTTP VERSION NOT SUPPORTED\r\n");
    sprintf(response+strlen(response), "Content-type: text/html\r\n");
    sprintf(response+strlen(response), "\r\n");
    sprintf(response+strlen(response), "<P>HTTP Version Not Supported.</P>\r\n ");
}


static const char* get_file_ext(const char *filename) {
   const char *ext = strrchr(filename, '.');
   return (ext && ext != filename) ? ext : (filename + strlen(filename));
}

int send_response(char* img, int sd) {
    char _resp[100];

    if (strcmp(get_file_ext(img), ".png") == 0) {
        ok(_resp, "image/png");
    } else if (strcmp(get_file_ext(img), ".jpg") == 0) {
        ok(_resp, "image/jpeg");
    } else if (strcmp(get_file_ext(img), ".gif") == 0) {
        ok(_resp, "image/gif");
    } else if (strcmp(get_file_ext(img), ".txt") == 0) {
        ok(_resp, "text/plain");
    } else {
        ok(_resp, "text/html");
    }

    write(sd, _resp, strlen(_resp));
    fsync(sd);
    bzero(_resp, sizeof(_resp));


    if (transferfile(img, sd) < 0) {
        // close(fd);
        return 0;
    }

    return 1;
}