#include "plc.h"

#define PLC_STRLEN 9
//The PLC device
int plc_fd = 0;

//For controller to check
//blueteeth中断
//extern int flag_bt;                        //for controller to check
int head4plc, rear4plc;
int head2plc, rear2plc;
char buf4plc[MAX_BUF_SIZE][PLC_SIZE];
char buf2plc[MAX_BUF_SIZE][PLC_SIZE];
struct termios stNew;
struct termios stOld;
//Open Port & Set Port
int serial_init_plc(){
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
int plc_read(int plc_fd){
    int nRet=0;
    int flag=1;
    int len=0;
    char str[PLC_SIZE];
    char tmp[16];
    while(flag||nRet!=0)
    {
        nRet = read(plc_fd, tmp, 1);
        if(tmp[0]>='a'&&tmp[0]<='z')
            nRet=1;
        else if(tmp[0]>='0'&&tmp[0]<='9')
            nRet=1;
        else if(tmp[0]>='A'&&tmp[0]<='Z')
            nRet=1;
        //else if(tmp[0]==' ')
          //  nRet=1;
        else
            continue;
        str[len]=tmp[0];
        len+=nRet;
        if(-1 == nRet)
        {
            perror("Read Data Error!\n");
            break;
        }
        if(len>=PLC_STRLEN){
            strcpy(buf4plc[head4plc],str);
            printf("str Data: %s\n", str);
            if(head4plc==MAX_BUF_SIZE)
                    head4plc=0;
            else
                head4plc++;
            if(head4plc==rear4plc)
                rear4plc++;
            print_buf();
            len=0;
            nRet=0;
        }
        tmp[0]=0;
    }
}

//write to the serial port
int plc_write(int plc_fd, int len)
{
    return write(plc_fd, buf2plc, len);
}

void print_buf()
{
    int i=0;
    for(i=0;i<8;i++)
    {
        printf("[%d]:%s\n", i, buf4plc[i]);
    }
}

//PLC module
int PLC_init()
{
    bzero(buf4plc, PLC_SIZE);
    bzero(buf2plc, PLC_SIZE);
    return serial_init_plc();
}

//send data to plc
void send2plc()
{

}

//get data from plc
int get4plc(char* tmp)
{
    if(rear4plc!=head4plc)
    {
        strcpy(tmp, buf4plc[rear4plc++]);
        if(rear4plc==MAX_BUF_SIZE)
            rear4plc=0;
        return 1;
    }
    else
        return 0;
}

/*
int b(int argc, char **argv)
{
    plc_fd=PLC_init();
    if( plc_fd == -1 )
    {
        perror("SerialInit Error!\n");
        return -1;
    }

    plc_read(plc_fd);

    print_buf();

    close(plc_fd);
    return 0;
}*/
