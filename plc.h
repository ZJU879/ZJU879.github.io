#ifndef __PLC_H__
#define __PLC_H__

#include     <stdio.h>  
#include     <stdlib.h>  
#include 	 <string.h> 
#include     <unistd.h>    
#include     <sys/types.h>  
#include     <sys/stat.h>  
#include     <fcntl.h>   
#include     <termios.h>  
#include     <errno.h>  

#define PLC_BAUDRATE B9600 ///Baud rate : 115200
#define PLC_DEVICE "/dev/ttyAMA0"
#define PLC_SIZE 512
#define MAX_BUF_SIZE 512

//The BLE device
extern int plc_fd;

//for controller to check
extern int head4plc, rear4plc;
extern char buf4ble[MAX_BUF_SIZE][PLC_SIZE];
extern char plc_write_buf[PLC_SIZE];

//init serial 
int serial_init();

//return the len of the data
int  serial_read(int fd);

//write to the serial port
void serial_write(int fd, int len);

//PLC module
void PLC_init();

//send data to plc
void send2plc();

//get data from plc
void get4plc();

#endif