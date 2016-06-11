#include "flagdef.h"
#include "http.h"
#include "ble.h"

//blueteeth中断
//int flag_bt;                        //for controller to check
/*int head4ble = -1, rear4ble = -1;
int head2ble = -1, rear2ble = -1;
char buf4ble[MAXSIZE][BT_SIZE];
char buf2ble[MAXSIZE][BT_SIZE];*/
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
//char bufsen[MAXSIZE];      //由发送线程读取

/*  环形缓冲区API
     对于一个环形缓冲区，rear和head分别指着最新插入和最先插入的位置，初值为-1
*/

/*
 *   添加数据到缓冲区
 *  参数说明：
 *    buf：      缓冲区名字
 *    data：    插入数据
 *    head：   缓冲区头指针
 *    rear：     缓冲区尾指针
 *    size：     缓冲区大小
 *    返回值：1表示插入成功，0表示满了插入失败
 */
int writebuf(char **buf, char *data, int *head, int *rear, int size){
    if(*rear==*head)      return 0;
    *rear = *rear+1;
    if(*rear==size)          *rear = 0;
    strcpy(buf[*rear], data);
    return 1;
}

/*
 *   从缓冲区读取数据
 *  参数说明：
 *    buf：      缓冲区名字
 *    data：    读取数据的数组
 *    head：   缓冲区头指针
 *    rear：     缓冲区尾指针
 *    size：     缓冲区大小
 *    返回值：1表示读取成功，0表示满了读取失败（为空，一般来说先检查是否为空再读就好了）
 */
int readbuf(char **buf, char *data, int *head, int *rear, int size){
    if(*rear==*head)      return 0;
    *head = *head+1;
    if(*head==size)          *head = 0;
    strcpy(data, buf[*head]);
    return 1;
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


//网络模块初始化，打开发送端口
void NET_INIT(){

}

void listener(){
    while(1){
        sleep(100);
    }
}

void *thread_ble(void *tmp){
    ble_fd = BLE_init();
    if( ble_fd == -1 ){
        perror("SerialInit Error!\n");
        return;
    }

    ble_read(ble_fd);

    print_buf();

    close(ble_fd);
    return;
}

void *thread_plc(void *tmp){

}

//主程序
int main(){
    //Controller Data Structure 控制器数据结构
    char* server = "IP";
    pthread_t th_listen,th_ble,th_plc;
    char buf[DEV_SIZE];
    int device_id;
    int device_type;
    //BLE_init();
    printf("%d",BLE_init());
    //PLC_init();
    //NET_init();
    //创建监听线程
    //pthread_create(&th_listen,NULL,listener,0);
    printf("Begin the program\n");
    pthread_create(&th_ble, NULL, thread_ble,0);
    pthread_create(&th_plc, NULL, thread_plc,0);
    printf("Create thread sucessfully\n");
    while(1){
        //Deal with the blueteeth data recieve
        if(get4ble(buf)){
            //Get device ID
            printf("pengdiandongxi\n");
            device_id = getDevID(buf,BLT);
            printf("%s\n",buf);
            //readbuf(buf4ble, buf, &head4ble, &rear4ble, BT_SIZE);
            //Get device ID
            //device_id = getDevID(buf,BLT);
            //Send data to the server
            post(server, device_id, buf);
        }
        //Deal with the HTTP data recieve
        if(flag_rec){
          //Get device ID and judge type
          device_type = getDevID(buf,NET);
          //Send data to the ID
          switch(device_type){
              case PLC:   writebuf(buf2plc, bufrec, &head2plc, &rear2plc, PLC_SIZE); break;
              case BLT:   writebuf(buf2ble,  bufrec, &head2ble,   &rear2ble,  BT_SIZE);   break;
          }
        }
    }
}
