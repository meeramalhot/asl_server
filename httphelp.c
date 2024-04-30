#include "httphelp.h"
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

//get file descriptor to read and write to fd, will return -1 if file does not exist
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
  /*FIX*/
  else if (strcasecmp(dot, "cgi") == 0) {
    return "text/html"; //changed from image/gif to text/html
  }
  else {
    return "text/html"; 
  }
}

/*
const char * format_time(void){
    char output[40];
    time_t rawtime;
    struct tm * timeinfo;
    
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    
    sprintf(output, "Date: %d %d %d %d:%d:%d GMT", timeinfo->tm_mday,
            timeinfo->tm_mon + 1, timeinfo->tm_year + 1900,
            timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    return output;
}
*/