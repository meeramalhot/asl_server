#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "httphelp.h"


#define BUFFER_SIZE 900
#define UNDEFINED 501
#define NOT_FOUND 404
#define OK 200

#define MAX_FIELDS 32

typedef struct {
  char * key;
  char * value;
} header_field;

void make_https_response(int status_code, int desc, char *content, const char *content_type, long int content_length);
void *request_deal(void *socket_descriptor) ;

void myServ(int port) {

  int sd, new_sd;
  struct sockaddr_in name, client_name;
  int sock_opt_val = 1;
  unsigned int client_len;
  char data[80];		/* Our receive data buffer. */
  
  //create socket
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Server: socket() error");
    exit (-1);
  }

  //create an address to bind the socket to
  if (setsockopt (sd, SOL_SOCKET, SO_REUSEADDR, (char *) &sock_opt_val, sizeof(sock_opt_val)) < 0) {
    perror ("Server: Failed to set SO_REUSEADDR on INET socket");
    exit (-1);
  }

  name.sin_family = AF_INET;
  name.sin_port = htons (port);
  name.sin_addr.s_addr = htonl(INADDR_ANY);

  //bind socket to port
 if (bind (sd, (struct sockaddr *)&name, sizeof(name)) < 0) {
    perror ("Server: bind() error");
    exit (-1);
  }

  //listen for connections
  listen (sd, 5);

  //indefinite loop
  for (;;) {
      client_len = sizeof (client_name);
      new_sd = accept (sd, (struct sockaddr *) &client_name, &client_len);
      printf ("Assigning new socket descriptor:  %d\n", new_sd);
      
      if (new_sd < 0) {
	      perror ("Server: accept() error");
	      exit (-1);
      }

      //thread to handle new request
      pthread_t request_thread;
      pthread_create(&request_thread, NULL, request_deal, (void *)new_sd);
      pthread_detach(request_thread);
  }

}

//parse HTTPS requests here
void *request_deal(void *socket_descriptor) {
  //make sure desc is an int
  int desc = (int)socket_descriptor;

  char buffer[BUFFER_SIZE];
  memset(buffer, 0, BUFFER_SIZE);
  //number of characters recieved, stored into buffer
  ssize_t buff_rec = recv(desc, buffer, BUFFER_SIZE, 0);

  char * get_http = strtok(buffer, " ");
  if(get_http == NULL) {
    return NULL;
  }

  //not equal
  if(strcmp(get_http, "GET") != 0) {
    make_https_response(UNDEFINED, desc, NULL, "text/html", 0);
    return NULL;
  }

  char * file_name = strtok(NULL, " ");
  //check if filename starts with a slash and ignore it
  if(file_name[0] == '/') {
    //start at the second char
    file_name = &file_name[1];
  }

  char * version = strtok(NULL, "\n");

  //check if HTTP versions are corrext
  if(strncmp(version, "HTTP/1.1", 8) != 0 && (strncmp(version, "HTTP/1.0", 8) != 0)) {
    printf("version compare failed\n");
    make_https_response(UNDEFINED, desc, NULL, "text/html", 0);
    return NULL;
  }

  //ADD FLAG FOR TYPE OF HTTP

  //can handle 5 rows of headers
  header_field headers[MAX_FIELDS];

  //populate array with header value and content
  for(int i = 0; i < MAX_FIELDS; i++) {
    headers[i].key = strtok(NULL, ":");
    if(headers[i].key == NULL || strcmp(headers[i].key, "\n") == 0) {
      break;
    }
    headers[i].value = strtok(NULL, "\n");
    if(headers[i].value == NULL || strcmp(headers[i].value, "\n") == 0) {
      break;
    }
  }

  FILE * fd = get_file_descriptor(file_name);
  if (fd == NULL) {
    //not found
    make_https_response(NOT_FOUND, desc, NULL, "text/html", 0);
    return NULL;
  } else {
    //found
    long int size = get_file_size(fd);
    //stores file contents in content buffer
    char * content_buffer = malloc(size);
    fread(content_buffer, 1, size, fd);
    make_https_response(OK, desc, content_buffer, get_mime(file_name), size);

    return NULL;
  }
  
}

void make_https_response(int status_code, int desc, char *content, const char *content_type, long int content_length) {
  char * status_name;
  char header[40];

  switch(status_code) {
    case OK:
      status_name = "OK";
    case NOT_FOUND:
      status_name = "Not Found";
    case UNDEFINED:
      status_name = "Not Implemented";
    default:
      status_name = "";
  }

  sprintf(header, "HTTP/1.1 %d %s\n", status_code, status_name);
  send(desc, header, strlen(header), 0);
  sprintf(header, "Content-type: %s\n\n", content_type);

  send(desc, header, strlen(header), 0);
  if(content != NULL) {
    send(desc, content, content_length, 0);
  }
  send(desc, "\n", strlen("\n"), 0);
  close(desc);
}

int main (int argc, char *argv[]) {
  int port;
  port = atoi(argv[1]);
  
  myServ(port);		/* Server port. */

  return 0;
}