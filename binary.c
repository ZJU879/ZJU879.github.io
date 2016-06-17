#include "mouse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "binary.h"
// #define MOUSE_HOST "localhost"
#define MOUSE_HOST "106.2.108.194"
#define MOUSE_PORT 10659

//md5(md5(device_id)+device_name)
#define KEY_FOR_LOGIN "ae027b7d42b34a173e94dfcbdc207766"

// Report Sample for  face detection
typedef struct reportData1 {
    int device_id;
    int report_id;
    unsigned char* type;
    unsigned char*  pid;
    unsigned char*  time;
    unsigned char* result;
} reportData1;
packet* pack_report1(packet* p, void* data)
{
    reportData1* dataptr = (reportData1*)data;
    packet_put_int(p, dataptr->device_id);
    packet_put_int(p, dataptr->report_id);

    packet_put_buffer(p, dataptr->type,1);
    packet_put_buffer(p, dataptr->pid,4);
    packet_put_buffer(p, dataptr->time, 8);
    packet_put_buffer(p, dataptr->result,1);
    return p;
} 
// Report Sample  for air conditioner
typedef struct reportData2 {
    int device_id;
    int report_id;
    float humidity;
    float temperature;
    int state;
  
} reportData2;
packet* pack_report2(packet* p, void* data)
{
    reportData2* dataptr = (reportData2*)data;
    packet_put_int(p, dataptr->device_id);
    packet_put_int(p, dataptr->report_id);

    packet_put_float(p, dataptr->humidity);
    packet_put_float(p, dataptr->temperature);
    packet_put_int(p, dataptr->state);
    return p;
} 
typedef struct reportData3 {
    int device_id;
    int report_id;
    unsigned char* lamp;
    unsigned char*  voice;
    unsigned char*  body;
    unsigned char* light;
  
} reportData3;
packet* pack_report3(packet* p, void* data)
{
    reportData3* dataptr = (reportData3*)data;
    packet_put_int(p, dataptr->device_id);
    packet_put_int(p, dataptr->report_id);

    packet_put_buffer(p, dataptr->lamp,1);
    packet_put_buffer(p, dataptr->voice,1);
    packet_put_buffer(p, dataptr->body, 1);
    packet_put_buffer(p, dataptr->light,1);
    return p;
} 
/*typedef struct controlRecvData {
    int start;
   
} controRecvlData;

 //Send control Sample
typedef struct controlSendData {
    int device_id;
    int control_id;
    int target_id;
    int start;
   
} controlSendData;
packet* pack_control_send(packet* p, void* data)
{
    controlSendData* dataptr = (controlSendData*)data;
    packet_put_buffer(p, "S", 1);
    packet_put_int(p, dataptr->device_id);
    packet_put_int(p, dataptr->control_id);
    packet_put_int(p, dataptr->target_id);
    packet_put_int(p, dataptr->start);
    return p;
}*/
int binary_init(){
    mouse_init(MOUSE_HOST, MOUSE_PORT);
    if (mouse_login(8, KEY_FOR_LOGIN) == 0){
	return 0;
    } 
    else return -1;
}
int binary_send(int device_id,int report_id,char data[][20]){
    if(device_id==15){
        reportData1 data1 = {device_id, report_id, data[0], data[1], data[2],data[3]};
	mouse_report(pack_report1, (void*)&data1);
    }
    else if(device_id==23){
	
	float humi=(float)atof(data[0]);
	float temp=atof(data[1]);
	int state=atoi(data[2]);
	reportData2 data2 = {device_id, report_id, humi, temp, state};
	mouse_report(pack_report2, (void*)&data2);
    }
    else if(device_id==27){
	reportData3 data3 = {device_id, report_id, data[0], data[1], data[2],data[3]};
	mouse_report(pack_report3, (void*)&data3);
    }
    else{
	printf("unknown message source\n");
    }
    return 0;
}
int binary_recv(int device_id,int control_id,char *recv){
    packet* pptr = NULL;
    pptr=mouse_control_recv(device_id, control_id);
    if(pptr==NULL)return -1;
    else{
	
	sprintf(recv,"{%d}",pptr->payload[0]);
	//sprintf(recv,"{1}");
	packet_free(pptr);
	return 0;
    }
}
int binary_exit(){
    mouse_logout();
}
int main()
{
    printf("hello!\n");

  /* packet* p =packet_allocate();;
    packet_put_int(p, 3);
	//int a=pptr->payload[0];
	printf("recv:%2X\n",p->payload[0]);*/
   if (binary_init() == 0)
    {
	
	char ret_msg[30];
	char data[5][20];
	/*sprintf(data[0],"3.42");//temperature
	sprintf(data[1],"3.42");//humidity
	sprintf(data[2],"7");
	binary_send(23,21,data);
	

	 
	//test for face detection  device id 15	 report id 7
	sprintf(data[0],"3");
	sprintf(data[1],"3842");
	sprintf(data[2],"34:42:65");
	sprintf(data[3],"3");
	binary_send(15,23,data);

	//test for plc
	sprintf(data[0],"1");
	sprintf(data[1],"2");
	sprintf(data[2],"3");
	sprintf(data[3],"4");
	binary_send(27,24,data);*/
	int i;
	for(i=0;i<5;i++){
	 if(binary_recv(23,6,ret_msg)==0)
	 printf("recv mesg:%s",ret_msg);
	else
	  printf("no message return!\n");
	}

        binary_exit();
    }
	
	
    return 0;
}
