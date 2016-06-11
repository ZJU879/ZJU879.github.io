#include <stdlib.h>
#include <pthread.h>
#define FREE 0
#define BLT 1
#define PLC 2
#define NET 3
#define RELEASE 4
#define ERR 99
#define MAXSIZE 100
#define PLC_SIZE 10
#define BT_SIZE 10
#define DEV_SIZE 512

//blueteeth中断
//extern int flag_bt;                        //for controller to check
extern int head4bt, rear4bt;
extern int head2bt, rear2bt;
extern char buf4bt[MAXSIZE][BT_SIZE];
extern char buf2bt[MAXSIZE][BT_SIZE];
//PLC
//extern int flag_plc;                        //for controller to check
extern int head4plc, rear4plc;
extern int head2plc, rear2plc;
extern char buf4plc[MAXSIZE][PLC_SIZE];
extern char buf2plc[MAXSIZE][PLC_SIZE];
//Http
extern int flag_rec;                        //轮询到数据时置1，由主控制器读取检查
//int flag_sen;                        //需要发送时置1，由http发送线程检查
extern int bufID;
extern char bufrec[MAXSIZE];       //由主控读取
//char bufsen[MAXSIZE];      //由发送线程读取

//Lucas
void bt_init();
//LiwenYong
void PLC_init();
//QiuYingyue
//void Send_init();

//RingBuffer API
int writebuf(char **buf, char *data, int *head, int *rear, int size);
int readbuf(char **buf, char *data, int *head, int *rear, int size);

int getDevID(char *buf, int device_type);
void BLT_init();
void PLC_init();
void NET_init();
