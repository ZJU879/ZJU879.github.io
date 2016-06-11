#include "serial.h"

//init serial 
int serial_init(char *serial){
    int fd;         
    struct termios opt;   
      
    fd = open(serial, O_RDWR|O_NOCTTY|O_NDELAY);  
    if(fd == -1)  
    {  
        perror("open serial 0\n");  
        exit(0);  
    }  
  
    tcgetattr(fd, &opt);        
    bzero(&opt, sizeof(opt));  
      
    tcflush(fd, TCIOFLUSH);  
  
    cfsetispeed(&opt, B115200);  
    cfsetospeed(&opt, B115200);  
      
    opt.c_cflag &= ~CSIZE;    
    opt.c_cflag |= CS8;     
    opt.c_cflag &= ~CSTOPB;   
    opt.c_cflag &= ~PARENB;   
    opt.c_cflag &= ~CRTSCTS;  
    opt.c_cflag |= (CLOCAL | CREAD);  
   
    opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);  
   
    opt.c_oflag &= ~OPOST;  
      
    opt.c_cc[VTIME] = 0;  
    opt.c_cc[VMIN] = 0;  
      
    tcflush(fd, TCIOFLUSH);  
   
    printf("configure complete\n");  
      
    if(tcsetattr(fd, TCSANOW, &opt) != 0)  
    {  
        perror("serial error");  
        return -1;  
    } 

    return fd;
}

//return the len of the data
int  serial_read(int fd){
    int len = 0;
    int n = 0;

    while( (n = read(fd, plc_read_buf, sizeof(plc_read_buf))) > 0 )  
    {             
            len += n;  
    }

    plc_read_buf[len] = '\0';

    return len;
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
    char serial_port = "/dev/ttyS0";
    fd = serial_init(serial_port);
}

//send data to plc
void send2plc(){
    serial_write(fd,PLC_SIZE);
}

//get data from plc
void get4plc(){
    int bytes;
    bytes=serial_read(fd);
    printf("we get %d bytes data!", bytes);
}
