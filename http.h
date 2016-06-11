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
struct ms_type{
	//int device_id;
	int report_id;
	char fieldname[3][100];
	char fielddata[3][100];
};
int Send_init(char* host_addr,int host_port);
int report_packet(int *psocket_id,char* host_addr,char* host_file,int host_port,int device_id,int report_id,char senddata[][10]);
int control_packet(int *psocket_id,char* host_addr,char* host_file,int host_port,int device_id,int control_id,char *recv_json);
int send2server(char *webaddr,int device_id,int report_id,char senddata[][10]);//public
int receive4server(char *webaddr,int device_id,int control_id,char *recv_json);//public
void GetHost(char* src, char* web, char* file, int* port);

#endif
