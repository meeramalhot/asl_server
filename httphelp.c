#include "httphelp.h"
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

//get file descriptor to read and write to fd, will return NULL if file does not exist
FILE * get_file_descriptor(const char *file_name){
  FILE * fd;
  fd = fopen(file_name, "r");

  return fd;
}

//use fseek to get file size
long int get_file_size(FILE * f) {
  long int old = ftell(f);
  fseek(f, 0L, SEEK_END);
  long int result = ftell(f);
  fseek(f, old, SEEK_SET);

  return result;
}

const char * get_mime(char *file)
{
  //find where filetype is and incrament by one to not include dot
  const char *dot = strrchr(file, '.');
  dot++;
  //if period not found
  if (dot == NULL) {
    return "text/html"; 
  }
  else if (strcasecmp(dot, "html") == 0) {
    return "text/html"; 
  }
  else if (strcasecmp(dot, "txt") == 0) {
    return "text/plain"; 
  }
  else if (strcasecmp(dot, "css") == 0) {
    return "text/css";
  }
  else if (strcasecmp(dot, "js") == 0) {
    return "application/js";
  }
  else if (strcasecmp(dot, "jpg") == 0 || strcasecmp(dot, "jpeg") == 0) {
    return "image/jpeg";
  }
  else if (strcasecmp(dot, "png") == 0) {
    return "image/png";
  }
  else if (strcasecmp(dot, "gif") == 0) {
    return "image/gif";
  }
  else if (strcasecmp(dot, "cgi") == 0) {
    return "text/html"; //changed from image/gif to text/html
  }
  else {
    return "text/html"; 
  }
}
