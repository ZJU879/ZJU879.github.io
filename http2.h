#ifndef __HTTP2_H__
#define __HTTP2_H__
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
void plc_debug();
int Send_init(char* host_addr,int host_port);
int report_packet(int *psocket_id,char* host_addr,char* host_file,int host_port,int device_id,char senddata[][20]);
int control_packet(int *psocket_id,char* host_addr,char* host_file,int host_port,int device_id,char *recv_json);
int send2server(int device_id,char senddata[][20]);//public  return:0 success;-1 failed
int receive4server(int device_id,char *ret_msg);//public	return:0 have message in ret_msg; -1 no message returned
void GetHost(char* src, char* web, char* file, int* port);
int parsejson(char *json,char *ret_msg);
#endif
