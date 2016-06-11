#ifndef __HTTP_H__
#define __HTTP_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <limits.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ctype.h>
int Send_init(char* host_addr,int host_port);
int postServer(int *psocket_id,char* host_addr,char* host_file,int host_port,char *payload,int device_id);
int getServer(int *psocket_id,char* host_addr,char* host_file,int host_port,char *str);
int post(char *webaddr,int deviceID,char *message);//public
int get(char *webaddr,int deviceID,char *message);//public
void GetHost(char* src, char* web, char* file, int* port);

#endif
