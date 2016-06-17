#include "flagdef.h"
#include "http.h"
#include "ble.h"
#include "binary.h"
#include "mouse.h"
#define D_ID_875 15
#define R_ID_875 23
#define D_ID_872 23
#define R_ID_872 21
#define C_ID_872 6
#define D_ID_plc 27
#define R_ID_xxx 7

//PLC
//int flag_plc;                        //for controller to check
int head4plc = -1, rear4plc = -1;
int head2plc = -1, rear2plc = -1;
char buf4plc[MAXSIZE][PLC_SIZE];
char buf2plc[MAXSIZE][PLC_SIZE];
//Http
int flag_rec;                        //轮询到数据时置1，由主控制器读取检查
//int flag_sen;                        //需要发送时置1，由http发送线程检查
int bufID;
char bufrec[MAXSIZE];       //由主控读取
char dt;
//char bufsen[MAXSIZE];      //由发送线程读取


//face detection
//device id 15
//report id 7
void ble_1_parse(char res[][20], char* src){
    res[0][0] = src[0]; res[0][1] = 0;//type
    res[1][0] = src[1];
    res[1][1] = src[2];
    res[1][2] = src[3];
    res[1][3] = src[4]; res[1][4] = 0;//pid
    res[2][0] = src[5];
    res[2][1] = src[6];
    res[2][2] = src[7];
    res[2][3] = src[8];
    res[2][4] = src[9];
    res[2][5] = src[10];
    res[2][6] = src[11];
    res[2][7] = src[12]; res[2][8] = 0;//time
    res[3][0] = src[13]; res[3][1] = 0;//result
}

//     air condition
void ble_2_parse(char res[][20], char* src){
    res[1][0] = src[1];
    res[1][1] = src[2];
    res[1][2] = src[3];
    res[1][3] = src[4];
    res[1][4] = src[5]; res[1][5] = 0;
    res[0][0] = src[6];
    res[0][1] = src[7];
    res[0][2] = src[8];
    res[0][3] = src[9];
    res[0][4] = src[10]; res[0][5] = 0;
    res[2][0] = src[9];  res[2][1] = 0;
}

//     plc
void plc_parse (char res[][20], char* src){
    res[0][0] = src[0]; res[0][1] = 0;
    res[1][0] = src[1];
    res[1][1] = src[2];
    res[1][2] = src[3];
    res[1][3] = src[4];
    res[1][4] = src[5];
    res[1][5] = src[6];
    res[1][6] = src[7];
    res[1][7] = src[8];
    res[1][8] = src[9];
    res[1][9] = src[10]; res[1][10] = 0;
    res[2][0] = src[11]; res[4][1] = 0;
}

/*
 *   从字符串中解析出当前的id编号或者类型
 *  参数说明：
 *    buf：                 字符串
 *    device_type：   设备类型
 *    返回值：返回当前需要操作的目标设备类型
 */
int getDevID(char *buf, int device_type){
    if(device_type==NET){
        return PLC;
        return BLT;
    }
    else if(device_type==PLC||device_type==BLT){
        return buf[0]-'0';
    }
    return ERR;
}

void *listener(void *tmp){
    char recv_json[1024];
    int device_id = D_ID_872;
    int control_id = C_ID_872;
    //char* cu = "http://fat.fatmou.se/api/control";
    while(dt=='2'){
      if(receive4server(device_id,control_id,recv_json)!=-1){
          //do something
          send2ble(recv_json);
      }
    }
    while(dt=='4'){
      if(binary_recv(device_id,control_id,recv_json)!=-1){
          //do something
          send2ble(recv_json);
      }
    }
}

void *thread_4ble(void *tmp){
    /*ble_fd = BLE_init();
    if( ble_fd == -1 ){
        perror("SerialInit Error!\n");
        return;
    }*/
    while(1){
      if(dt=='1')
        ble_read_872(ble_fd);
      else if(dt=='2')
        ble_read_872(ble_fd);
    }
    return;
}

void *thread_2ble(void *tmp){
    while(1){
        ble_write(ble_fd);
    }
    return;
}

//主程序
int main(int argc, char ** argv){
    //Controller Data Structure 控制器数据结构
    if(argc!=2&&argv[1][0]!='1'&&argv[1][0]!='2'&&argv[1][0]!='3'&&argv[1][0]!='4'){
        printf("Invalid input!\n");
        return 0;
    }
    dt = argv[1][0];
    char* server = "IP";
    pthread_t th_listen,th_4ble,th_2ble,th_plc;
    char buf[DEV_SIZE];
    char res[5][20];
    int device_id, report_id;
    int device_type;
    //BLE_init();
    printf("%d",BLE_init());
    //PLC_init();
    printf("Begin the program\n");
    ble_fd = BLE_init();
    if( ble_fd == -1 ){
        perror("SerialInit Error!\n");
        return;
    }
    pthread_create(&th_listen,NULL,listener,0);
    pthread_create(&th_4ble, NULL, thread_4ble,0);
    pthread_create(&th_2ble, NULL, thread_2ble,0);
    //pthread_create(&th_plc, NULL, thread_plc,0);
    printf("Create thread sucessfully\n");
    while(1){
        //Deal with the blueteeth data recieve
      if(dt=='1' || dt=='3'){//875
        if(get4ble(buf)){
            //Get device ID
            printf("Data from 875 bluetooth device\n");
            //875 face detection
            device_id = D_ID_875;
            report_id = R_ID_875;
            ble_1_parse(res,buf);
            printf("%s\n",buf);
            //Send data to the server
            if(dt=='1')
		send2server(device_id,report_id,res);
	    else
		binary_send(device_id,report_id,res);
        }
      }
      if(dt=='2'){//872
        if(get4ble(buf)){
            //Get device ID
            printf("Data from 872 bluetooth device\n");
            //875 face detection
            device_id = D_ID_872;
            report_id = R_ID_872;
            ble_2_parse(res,buf);
            printf("%s\n",buf);
            //Send data to the server
	    if(dt=='2')
	        if(!send2server(device_id,report_id,res)){
                    printf("Failed to send to server!\n");
            	}
  	    else
                if(!binary_send(device_id,report_id,res)){
                    printf("Failed to send to server!\n");
                }
        }
        //Deal with the HTTP data recieve
        if(flag_rec){
        }
      }
    }
}
