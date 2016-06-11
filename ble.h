#ifndef _BLE_H
#define _BLE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#define BLE_BAUDRATE B9600 ///Baud rate : 115200
#define BLE_DEVICE "/dev/ttyAMA0"
#define BLE_SIZE 512
#define MAX_BUF_SIZE 512

//The BLE device
extern int ble_fd;

//For controller to check
//blueteeth中断
//extern int flag_bt;                        //for controller to check
extern int head4ble, rear4ble;
extern int head2ble, rear2ble;
extern char buf4ble[MAX_BUF_SIZE][BLE_SIZE];
extern char buf2ble[MAX_BUF_SIZE][BLE_SIZE];

//init serial
int serial_init_ble();

//return the len of the data
int ble_read(int ble_fd);

//write to the serial port
int ble_write(int ble_fd, int len);

//BLE module
int BLE_init();

//send data to ble
void send2ble();

//get data from ble
int  get4ble(char *tmp);

void print_buf();

#endif
