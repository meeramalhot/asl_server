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
#define MAX_PARAMETERS 32


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

void * submit_letter(int desc, char *offset){
  //after headers, extra /n
  strtok(offset, "\n");

  //parsing letter=char values
  header_field parameters[MAX_PARAMETERS];
  char *key, *value;
  int i = 0;

  while ((key = strtok(NULL, "=")) && (value = strtok(NULL, "&\n")) && i < MAX_PARAMETERS) {
        parameters[i].key = key;
        parameters[i].value = value;
        //point at the byte after the null byte of value string
        i++;
    }
    //check is the first parameter is letter, then check send that letter
    if(strcmp(parameters[0].key, "letter")==0) {
      setenv("letter", parameters[0].value, 1);
    }
}

void * handle_post(int desc) {
    char *function = strtok(NULL, " ");
    char *version = strtok(NULL, "\r\n");
    if (strncmp(version, "HTTP/1.1", 8) != 0 && strncmp(version, "HTTP/1.0", 8) != 0) {
        make_https_response(UNDEFINED, desc, NULL, "text/html", 0);
        return NULL;
    }

    // Header parsing
    header_field headers[MAX_FIELDS];
    int i = 0;
    char *key, *value;
    char *offset;
    while ((key = strtok(NULL, ": ")) && (value = strtok(NULL, "\r\n")) && i < MAX_FIELDS) {
        headers[i].key = key;
        headers[i].value = value;
        //point at the byte after the null byte of value string
        offset = value + strlen(value) + 1;
        i++;
    }

    if (strcmp(function, "submit_letter")==0) {
      return submit_letter(desc, offset);
    } else {
      make_https_response(NOT_FOUND, desc, NULL, "text/html", 0);
      return NULL;
    }
}

void * handle_get(int desc) {
    char *file_name = strtok(NULL, " ");
    if (file_name[0] == '/') {
        file_name = &file_name[1]; // Ignore the leading '/'
    }

    char *version = strtok(NULL, "\r\n");
    if (strncmp(version, "HTTP/1.1", 8) != 0 && strncmp(version, "HTTP/1.0", 8) != 0) {
        make_https_response(UNDEFINED, desc, NULL, "text/html", 0);
        return NULL;
    }

    // Header parsing
    header_field headers[MAX_FIELDS];
    int i = 0;
    char *key, *value;
    while ((key = strtok(NULL, ": ")) && (value = strtok(NULL, "\r\n")) && i < MAX_FIELDS) {
        headers[i].key = key;
        headers[i].value = value;
        i++;
    }

    // Checking if the request is for a CGI script
    if (strstr(file_name, ".cgi") != NULL) {
        char *queryString = strchr(file_name, '?');
        if (queryString) {
            *queryString = '\0'; // Terminate the file_name at '?'
            queryString++; // Start of the query string
        }

        // Set CGI environment variables
        setenv("QUERY_STRING", queryString ? queryString : "", 1);
        setenv("REQUEST_METHOD", "GET", 1);

        // Execute CGI script
        char command[1024];
        snprintf(command, sizeof(command), "./%s", file_name);
        FILE *pipe = popen(command, "r");
        if (!pipe) {
            make_https_response(NOT_FOUND, desc, NULL, "text/html", 0);
            return NULL;
        }

        // Read the output of the script and send it as the response
        char scriptOutput[BUFFER_SIZE];
        size_t bytesRead = fread(scriptOutput, 1, BUFFER_SIZE, pipe);
        pclose(pipe);

        make_https_response(OK, desc, scriptOutput, "text/html", bytesRead);
        return NULL;
    }

    // Handling static files
    FILE *fd = get_file_descriptor(file_name);
    if (fd == NULL) {
        make_https_response(NOT_FOUND, desc, NULL, "text/html", 0);
        return NULL;
    } else {
        long int size = get_file_size(fd);
        char *content_buffer = malloc(size);
        fread(content_buffer, 1, size, fd);
        fclose(fd);
        make_https_response(OK, desc, content_buffer, get_mime(file_name), size);
        free(content_buffer);
        return NULL;
    }

}

//parse HTTPS requests here
//changes the requrest_deal to handle cgi files - ahmad 
void *request_deal(void *socket_descriptor) {
    int desc = (int)socket_descriptor;

    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    recv(desc, buffer, BUFFER_SIZE, 0);

    char *get_http = strtok(buffer, " ");
    if (get_http == NULL) {
        return NULL;
    }

    if (strcmp(get_http, "GET") == 0) {
      return handle_get(desc);
    }else if(strcmp(get_http, "POST") == 0){
      return handle_post(desc);
    }else{
      make_https_response(UNDEFINED, desc, NULL, "text/html", 0);
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
