#ifndef __PLC_H__
#define __PLC_H__

#include     <stdio.h>  
#include     <stdlib.h>   
#include     <unistd.h>    
#include     <sys/types.h>  
#include     <sys/stat.h>  
#include     <fcntl.h>   
#include     <termios.h>  
#include     <errno.h>  

#define PLC_SIZE 16

//the serial device
int fd;

//for controller to check
char plc_write_buf[PLC_SIZE];
char plc_read_buf[PLC_SIZE];

//init serial 
int serial_init(char *serial);

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