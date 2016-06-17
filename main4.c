#include "flagdef.h"
#include "http2.h"
#include "plc.h"
#include "binary.h"
#include "mouse.h"
#define D_ID_875 15
#define R_ID_875 23
#define D_ID_872 23
#define R_ID_872 21
#define D_ID_plc 27
#define R_ID_plc 24

#define C_ID_872 6
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
    res[0][0] = src[5]; res[0][1] = 0;
    res[1][0] = src[6]; res[1][1] = 0;
    res[2][0] = src[7]; res[2][1] = 0;
    res[3][0] = src[8]; res[3][1] = 0;
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
}

void *thread_4ble(void *tmp){
    /*ble_fd = BLE_init();
    if( ble_fd == -1 ){
        perror("SerialInit Error!\n");
        return;
    }*/
    while(1){
      ble_read(ble_fd);
    }
    return;
}

void *thread_2ble(void *tmp){
    while(1){
    }
    return;
}

//主程序
int main(int argc, char ** argv){
    //Controller Data Structure 控制器数据结构
    if(argc!=2||argv[1][0]!='1'&&argv[1][0]!='2'){
        printf("Invalid input!\n");
        return 0;
    }
    binary_init();
    dt = argv[1][0];
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
        if(get4ble(buf)){
            //Get device ID
            printf("Data from plc bluetooth device\n");
            //875 face detection
            device_id = D_ID_plc;
            report_id = R_ID_plc;
            plc_parse(res,buf);
            printf("%s\n",buf);
            //Send data to the server
            if(dt=='1')
		            send2server(device_id,res);
	          else
		           binary_send(device_id,report_id,res);
       }
        //Deal with the HTTP data recieve
        if(flag_rec){
        }
    }
}
