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
#include "httphelp.h"


#define BUFFER_SIZE 900
#define UNDEFINED 501
#define NOT_FOUND 404
#define OK 200

#define MAX_FIELDS 5

typedef struct {
  char * key;
  char * value;
} header_field;

void make_https_response(int status_code, int *desc, char *content, char *content_type);

void myServ(int port) {

  int sd, new_sd;
  struct sockaddr_in name, client_name;
  int sock_opt_val = 1;
  int client_len;
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

	    exit (0);
  }
  //FIGURE OUT HOW TO CLOSE
  //close(new_sd);
}

//parse HTTPS requests here
void *request_deal(void *socket_descriptor) {
  //make sure desc is an int
  int * desc = (int *)socket_descriptor;

  char buffer[BUFFER_SIZE];
  memset(buffer, 0, BUFFER_SIZE);
  //number of characters recieved, stored into buffer
  ssize_t buff_rec = recv(*desc, buffer, BUFFER_SIZE, 0);

  char * get_http = strtok(buffer, " ");
  printf("This is the HTTP request:%s\n", get_http);

  //not equal
  if(strcmp(get_http, "GET") != 0) {
    make_https_response(UNDEFINED, desc, NULL, "text/html");
    return NULL;
  }

  char * file_name = strtok(NULL, " ");
  printf("This is the filename:%s\n", file_name);

  char * version = strtok(NULL, "\n");
  printf("This is the version:%s\n", version);

  //check if HTTP versions are corrext
  if(strcmp(version, "HTTP/1.1") != 0 && (strcmp(version, "HTTP/1.0") != 0)) {
    make_https_response(UNDEFINED, desc, NULL, "text/html");
    return NULL;
  }

  //ADD FLAG FOR TYPE OF HTTP

  //can handle 5 rows of headers
  header_field headers[MAX_FIELDS];

  //populate array with header value and content
  for(int i = 0; i < MAX_FIELDS; i++) {
    headers[i].key = strtok(NULL, ":");
    printf("this is headers[i].key: %s, this is i: %d \n", headers[i].key, i);
    if(headers[i].key == NULL) {
      break;
    }
    headers[i].value = strtok(NULL, "\n");
    printf("this is headers[i].value: %s, this is i: %d \n", headers[i].value, i);
  }

  FILE * fd = get_file_descriptor(file_name);
  if (fd == NULL) {
    //not found
    make_https_response(NOT_FOUND, desc, NULL, "text/html");
    return NULL;
  } else {
    //found
    long int size = get_file_size(fd);
    //stores file contents in content buffer
    char content_buffer[size];
    fread(content_buffer, 1, size, fd);
    make_https_response(OK, desc, content_buffer, get_mime(file_name));

    return NULL;
  }
  
}

void make_https_response(int status_code, int *desc, char *content, char *content_type) {
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
  send(*desc, header, strlen(header), 0);
  sprintf(header, "Content-type: %s\n\n", content_type);

  //?
  memset(header, 0, sizeof(header));

  send(*desc, header, strlen(header), 0);
  if(content != NULL) {
    send(*desc, content, strlen(content), 0);
  }
}

int main (int argc, char *argv[]) {
  int port;
  port = atoi(argv[1]);
  
  myServ(port);		/* Server port. */

  return 0;
}