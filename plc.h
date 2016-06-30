#ifndef __PLC_H__
#define __PLC_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#define PLC_BAUDRATE B9600 ///Baud rate : 115200
#define PLC_DEVICE "/dev/ttyAMA0"
#define PLC_SIZE 512
#define MAX_BUF_SIZE 512

//The PLC device
extern int plc_fd;

//For controller to check
//blueteeth中断
//extern int flag_bt;                        //for controller to check
extern int head4plc, rear4plc;
extern int head2plc, rear2plc;
extern char buf4plc[MAX_BUF_SIZE][PLC_SIZE];
extern char buf2plc[MAX_BUF_SIZE][PLC_SIZE];

//init serial
int serial_init_plc();

//return the len of the data
int plc_read(int plc_fd);

//write to the serial port
int plc_write(int plc_fd, int len);

//PLC module
int PLC_init();

//send data to plc
void send2plc();

//get data from plc
int  get4plc(char *tmp);

void print_buf();

#endif
