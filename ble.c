#include "ble.h"
//The BLE device
int ble_fd = 0;

//For controller to check
//blueteeth中断
//extern int flag_bt;                        //for controller to check
int head4ble, rear4ble;
int head2ble, rear2ble;
char buf4ble[MAX_BUF_SIZE][BLE_SIZE];
char buf2ble[MAX_BUF_SIZE][BLE_SIZE];
struct termios stNew;
struct termios stOld;
//Open Port & Set Port
int serial_init_ble(){
    ble_fd = open(BLE_DEVICE, O_RDWR|O_NOCTTY|O_NDELAY);
    if(-1 == ble_fd)
    {
        perror("Open Serial Port Error!\n");
        return -1;
    }
    if( (fcntl(ble_fd, F_SETFL, 0)) < 0 )
    {
        perror("Fcntl F_SETFL Error!\n");
        return -1;
    }
    if(tcgetattr(ble_fd, &stOld) != 0)
    {
        perror("tcgetattr error!\n");
        return -1;
    }
    stNew = stOld;
    cfmakeraw(&stNew);//将终端设置为原始模式，该模式下所有的输入数据以字节为单位被处理
    //set speed
    cfsetispeed(&stNew, BLE_BAUDRATE);//9600
    cfsetospeed(&stNew, BLE_BAUDRATE);
    //set databits
    stNew.c_cflag |= (CLOCAL|CREAD);
    stNew.c_cflag &= ~CSIZE;
    stNew.c_cflag |= CS8;
    //set parity
    stNew.c_cflag &= ~PARENB;
    stNew.c_iflag &= ~INPCK;
    //set stopbits
    stNew.c_cflag &= ~CSTOPB;
    stNew.c_cc[VTIME]=0;    //指定所要读取字符的最小数量
    stNew.c_cc[VMIN]=1; //指定读取第一个字符的等待时间，时间的单位为n*100ms
                //如果设置VTIME=0，则无字符输入时read（）操作无限期的阻塞
    tcflush(ble_fd,TCIFLUSH);  //清空终端未完成的输入/输出请求及数据。
    if( tcsetattr(ble_fd,TCSANOW,&stNew) != 0 )
    {
        perror("tcsetattr Error!\n");
        return -1;
    }
    return ble_fd;
}

int ble_read_872(int ble_fd){
   int nRet=0;
   int nRet1=0;
   int nRet2=0;
    int flag=1;
    char str[BLE_SIZE];
    int first=0;
    while(flag||nRet!=0)
    {
        nRet = read(ble_fd, str+nRet1+nRet2, BLE_SIZE);
        if(-1 == nRet)
        {
            perror("Read Data Error!\n");
            break;
        }
        if(0 < nRet)
        {
            if(first==0)
            {
                first=1;
		nRet1=nRet;
            }
            else if(first==1)
            {
                first=2;
                nRet2=nRet;
            }
            else
            {
                str[15+nRet]=0;
                strcpy(buf4ble[head4ble],str+1);
                printf("str Data: %s\n", str+1);
                if(head4ble==MAX_BUF_SIZE)
                    head4ble=0;
                else
                    head4ble++;
                if(head4ble==rear4ble)
                    rear4ble++;
                first=0;
            }
        }
    }
    return 1;
}

//return the len of the data
int ble_read(int ble_fd){
    int nRet=0;
    int flag=1;
    char str[BLE_SIZE];
    int first=0;
    while(flag||nRet!=0)
    {
        nRet = read(ble_fd, str+nRet, BLE_SIZE);
        if(-1 == nRet)
        {
            perror("Read Data Error!\n");
            break;
        }
        if(0 < nRet)
        {
        	if(first==0)
        	{
        		first=1;
        	}
        	else
        	{
        		str[7+nRet]=0;
        		strcpy(buf4ble[head4ble],str+1);
            	printf("str Data: %s\n", str);
            	if(head4ble==MAX_BUF_SIZE)
                	head4ble=0;
            	else
                	head4ble++;
            	if(head4ble==rear4ble)
                	rear4ble++;
                first=0;
        	}
        }
    }
    return 1;
}

//write to the serial port
int ble_write(int ble_fd)
{
	if(rear2ble!=head2ble)
	{
    	write(ble_fd, buf2ble[rear2ble], strlen(buf2ble[rear2ble]));
    	rear2ble++;
    	if(rear2ble==MAX_BUF_SIZE)
    		rear2ble=0;
	}
}

void print_buf()
{
    int i=0;
    for(i=0;i<8;i++)
    {
        printf("[%d]:%s\n", i, buf4ble[i]);
    }
}

//BLE module
int BLE_init()
{
    bzero(buf4ble, BLE_SIZE);
    bzero(buf2ble, BLE_SIZE);
    return serial_init_ble();
}

//send data to ble
int send2ble(char *sdata)
{
    	int len=strlen(sdata);
        strcpy(buf2ble[head2ble],sdata);
        if(head2ble==MAX_BUF_SIZE)
            head2ble=0;
        else
        	head2ble++;
        if(head2ble==rear2ble)
        	rear2ble++;
        return len;
}

//get data from ble
int get4ble(char* rdata)
{
    if(rear4ble!=head4ble)
    {
        strcpy(rdata, buf4ble[rear4ble++]);
        if(rear4ble==MAX_BUF_SIZE)
            rear4ble=0;
        return 0;
    }
    else
        return 0;
}
