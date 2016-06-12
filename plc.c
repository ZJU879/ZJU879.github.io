#include "plc.h"
//The PLC device
int plc_fd = 0;

//For controller to check
//blueteeth中断
//extern int flag_bt;                        //for controller to check
int head4plc, rear4plc;
char buf4plc[MAX_BUF_SIZE][PLC_SIZE];

char plc_write_buf[PLC_SIZE];

struct termios stNew;
struct termios stOld;

//init serial 
int serial_init(){

    plc_fd = open(PLC_DEVICE, O_RDWR|O_NOCTTY|O_NDELAY);
    if(-1 == plc_fd)
    {
        perror("Open Serial Port Error!\n");
        return -1;
    }
    if( (fcntl(plc_fd, F_SETFL, 0)) < 0 )
    {
        perror("Fcntl F_SETFL Error!\n");
        return -1;
    }
    if(tcgetattr(plc_fd, &stOld) != 0)
    {
        perror("tcgetattr error!\n");
        return -1;
    }
    stNew = stOld;
    cfmakeraw(&stNew);//将终端设置为原始模式，该模式下所有的输入数据以字节为单位被处理
    //set speed
    cfsetispeed(&stNew, PLC_BAUDRATE);//9600
    cfsetospeed(&stNew, PLC_BAUDRATE);
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
    tcflush(plc_fd,TCIFLUSH);  //清空终端未完成的输入/输出请求及数据。
    if( tcsetattr(plc_fd,TCSANOW,&stNew) != 0 )
    {
        perror("tcsetattr Error!\n");
        return -1;
    }
    return plc_fd;
}

//return the len of the data
int serial_read(int fd){
    int nRet=0;
    int flag=1;
    char str[PLC_SIZE];
    while(flag||nRet!=0)
    {
        printf("wait\n");
        nRet = read(plc_fd, str, PLC_SIZE);
        if(-1 == nRet)
        {
            perror("Read Data Error!\n");
            break;
        }
        if(0 < nRet)
        {
            str[nRet]=0;
            if(strcmp("exit",str)==0)
                break;
            strcpy(buf4plc[head4plc], str);
            printf("str Data: %s\n", str);
            if(head4plc==MAX_BUF_SIZE)
                head4plc=0;
            else
                head4plc++;
            if(head4plc==rear4plc)
                rear4plc++;
        }
    }
    return nRet;
}

void serial_write(int fd, int len){
    int n;
    // int size=0;
    // while(len>0){
        n = write(fd, plc_write_buf, len);  
        // len -= n;
        // size += n;
    // }
    printf("write %d chars\n",n);  

}

//PLC module
void PLC_init(){
    serial_init();
}

//send data to plc
void send2plc(){
    serial_write(plc_fd,PLC_SIZE);
}

//get data from plc
int get4plc(char *tmp){
    int bytes;
    bytes=serial_read(plc_fd);
    printf("we get %d bytes data!", bytes);
    return bytes;
}
