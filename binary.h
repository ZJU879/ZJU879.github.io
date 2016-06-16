#ifndef __BINARY_H__
#define __BINARY_H__
#include "mouse.h"
#include <stdio.h>
#include <string.h>

int binary_init();
int binary_send(int device_id,int report_id,char data[][20]);
int binary_recv(int device_id,int control_id,char *recv);
int binary_exit();
#endif
