
#include "http2.h"
int ret;//所接受数据的长度


int Send_init(char* host_addr,int host_port){//连接服务器，返回是否成功

	struct hostent *host;

	if((host=gethostbyname(host_addr))==NULL)/*取得主机IP地址*/
	{
    		printf("Gethostname error,%s \n",strerror(errno));//strerror(errno)
    		exit(1);
	}
	//in_addr_t server_ip = inet_addr("127.0.0.1");
	/* 创建socket */
	int m_socket_id = socket(AF_INET, SOCK_STREAM, 0);

	if(m_socket_id < 0)
	{
		printf("init socket failed!\n");
		return -1;
	}

	/* 设置socket选项，地址重复使用，防止程序非正常退出，下次启动时bind失败 */
	int is_reuse_addr = 1;
	setsockopt(m_socket_id, SOL_SOCKET, SO_REUSEADDR, (const char*)&is_reuse_addr, sizeof(is_reuse_addr));


	/* 客户程序填充服务端的资料 */
	struct sockaddr_in server_addr;
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(host_port);//
	//server_addr.sin_addr.s_addr = server_ip;
	server_addr.sin_addr=*((struct in_addr *)host->h_addr);

	 //printf("IP Address: %s\n",inet_ntoa(*((struct in_addr *)host->h_addr)));
   	 //printf( "portnumber: %d\n\n ", host_port);

	/* 客户程序发起连接请求*/
	while(connect(m_socket_id, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	{
		printf("connect failed(),try again...");
		usleep(100*1000); /* sleep 100ms, 然后重试，保证无论server先起还是后起，都不会有问题 */
	}
	//printf("finish connect!\n");
	return m_socket_id;
}

int report_packet(int *psocket_id,char* host_addr,char* host_file,int host_port,int device_id,char data[][20]){//上传数据包到服务器，返回response信息
	printf("************************************************************\n");
	int m_socket_id=*psocket_id;
	/*准备POST request，将要发送给主机*/
	char request[1024];
	char message[1024];
	//sprintf(message,"device_id=%d&report_id=%d",8,report_id);
	if(device_id==4)//air condidtioner
		sprintf(message,"{\"auth_id\":41,\"auth_key\":\"19e8b1674012fb8c69699c99cf6e2f94\",\"device_id\":%d,\"payload\":{\"temperature\":\"%s\",\"humidity\":\"%s\",\"state\":\"%s\"}}",device_id,data[0],data[1],data[2]);
	if(device_id==28)//face detection
		sprintf(message,"{\"auth_id\":41,\"auth_key\":\"19e8b1674012fb8c69699c99cf6e2f94\",\"device_id\":%d,\"payload\":{\"type\":\"%c\",\"pid\":\"%s\",\"time\":\"%s\",\"result\":\"%c\"}}",device_id,data[0][0],data[1],data[2],data[3][0]);

	int len=strlen(message);
	sprintf(request, "POST /%s HTTP/1.1\r\nAccept: */*\r\nAccept-Language: zh-cn\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE 5.01; Windows NT 5.0)\r\nCache-Control: no-cache\r\nContent-Type: application/json\r\nHost: %s:%d\r\nConnection: Close\r\nContent-Length: %d\r\n\r\n%s",host_file,  host_addr, host_port,len,message);
	printf("%s", message);

	/* 向server(主机)发送request */
	int send = 0;int totalsend = 0;
	int nbytes=strlen(request);
	while(totalsend < nbytes) {
  	  ret = write(m_socket_id, request + totalsend, nbytes - totalsend);
  	  if(ret==-1) {
		printf("send error!%s\n", strerror(errno));
		return -1;
	  }
  	  totalsend+=ret;
  	  printf("%d bytes send OK!\n", totalsend);
	}


	//* 接收server消息 */
	FILE * fp;
	fp = fopen("httpreport", "a");
	if(!fp) {
  	  printf("create file error! %s\n", strerror(errno));
   	 return 0;
	}
	//printf("\nThe following is the response header:\n");
	int i=0;
	/* 连接成功了，接收http响应，response */
	char buffer[256];
	char resHead[256]="";
	int ii=0;
	while((nbytes=read(m_socket_id,buffer,1))==1)
	{
	    if(i < 4) {
	      if(buffer[0] == '\r' || buffer[0] == '\n') i++;
	      else i = 0;
	      ii++;
	      if(ii>=10&&ii<=12){
		resHead[ii%10]=buffer[0];
	      }
	      //printf("%c", buffer[0]);/*把http头信息打印在屏幕上*/
	    }
	    else {
	      i++;
	    }
		fwrite(buffer, 1, 1, fp);/*将http信息写入文件*/
		if(i%1024 == 0) fflush(fp);/*每1K时存盘一次*/
	}
	fclose(fp);
	//
	resHead[3]='\0';
	if(strcmp(resHead,"200")==0) {
		printf("succeed:%s\n",resHead);return 0;
	}
	else {
		printf("failed:%s\n",resHead);return -1;

	}

}
int control_packet(int *psocket_id,char* host_addr,char* host_file,int host_port,int device_id,char *recv_json){//获取服务器传递的信息，并存于str，返回-1表示无信息
	printf("----------------------------------------------------------------\n");
	int m_socket_id=*psocket_id;
	/*准备POST request，将要发送给主机*/
	char request[1024];
	char message[1024];
	sprintf(message,"{\"auth_id\":41,\"auth_key\":\"19e8b1674012fb8c69699c99cf6e2f94\",\"device_id\":%d}",device_id);
	int len=strlen(message);
	sprintf(request, "POST /%s HTTP/1.1\r\nAccept: */*\r\nAccept-Language: zh-cn\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE 5.01; Windows NT 5.0)\r\nCache-Control: no-cache\r\nContent-Type: application/Json\r\nHost: %s:%d\r\nConnection: Close\r\nContent-Length: %d\r\n\r\n%s",host_file,  host_addr, host_port,len,message);
	printf("%s", message);

	/* 向server(主机)发送request */
	int send = 0;int totalsend = 0;
	int nbytes=strlen(request);
	while(totalsend < nbytes) {
  	  ret = write(m_socket_id, request + totalsend, nbytes - totalsend);
  	  if(ret==-1) {
		printf("send error!%s\n", strerror(errno));
		return -1;
	  }
  	  totalsend+=ret;
  	  printf("\n%d bytes send OK!\n", totalsend);
	}


	//* 接收server消息 */
	FILE * fp;
	fp = fopen("httpcontrol", "a");
	if(!fp) {
  	  printf("create file error! %s\n", strerror(errno));
   	 return 0;
	}
	//printf("\nThe following is the response header:\n");
	int i=0;
	/* 连接成功了，接收http响应，response */
	char buffer[256];
	char resHead[256]="";
	int ii=0;int iii=0;
	while((nbytes=read(m_socket_id,buffer,1))==1)
	{
	    if(i < 4) {
		if(buffer[0] == '\r' || buffer[0] == '\n') i++;
		else i = 0;
		ii++;
		if(ii>=10&&ii<=12){
		resHead[ii%10]=buffer[0];
		}
	    }
	    else {
		recv_json[iii++]=buffer[0];/*把http主体信息写入recv_json*/
		//printf("%d,%c  ",iii,buffer[0]);
		i++;
	    }
		fwrite(buffer, 1, 1, fp);/*将http信息写入文件*/
		if(i%1024 == 0) fflush(fp);/*每1K时存盘一次*/
	}
	fclose(fp);
	//
	resHead[3]='\0';
	if(strcmp(resHead,"200")==0) {
		printf("succeed:%s\n",resHead);return 0;
	}
	else {
		printf("failed:%s\n",resHead);return -1;

	}

}
int send2server(int device_id,char senddata[][20]){
/* 获取输入参数 */
	char host_addr[256]="115.29.112.57";//"10.110.34.143";//"115.29.112.57";
	char host_file[1024]="testEmbed";//"mysite/manage1.php";//"testEmbed";
	int host_port=3000;//81;
	GetHost("nya.fatmou.se/api/report", host_addr, host_file, &host_port);/*分析网址、端口、文件名等*/
	int m_socket_id=Send_init(host_addr,host_port);
	int res=report_packet(&m_socket_id,host_addr,host_file, host_port,device_id,senddata);
	close(m_socket_id);
	return res;//0 succeed  //-1 failed
}
int receive4server(int device_id,char *ret_msg){
/* 获取输入参数 */
	char host_addr[256]="115.29.112.57";//"10.110.34.143";//"115.29.112.57";
	char host_file[1024]="testEmbed";//"mysite/manage1.php";//"testEmbed";
	int host_port=3000;//81;
	char recv_json[1000]="";
	GetHost("nya.fatmou.se/api/poll", host_addr, host_file, &host_port);/*分析网址、端口、文件名等*/
	int m_socket_id=Send_init(host_addr,host_port);
	int res=control_packet(&m_socket_id,host_addr,host_file,host_port,device_id,recv_json);
	if(res==0){
		if(parsejson(recv_json,ret_msg)==0){
			printf("return message:%s\n",ret_msg);
		}
		else{
			printf("no message return\n");
			res=-1;
		}
	}
	close(m_socket_id);
	return res;//0 succeed  //-1 failed
}
int parsejson(char *json,char *ret_msg){
	int i=0;int res=0;
	for(i=0;json[i]!='\0';i++){
		if(json[i]=='c'&&json[i+1]=='o'&&json[i+2]=='d'&&json[i+3]=='e') res=json[i+6]-'0';
		if(json[i]=='s'&&json[i+1]=='t'&&json[i+2]=='a'&&json[i+3]=='r'&&json[i+4]=='t')sprintf(ret_msg,"{%c}",json[i+8]);
	}
	return res;//0 have message; -1 no message
}


/*
int main(int argc, char *argv[]){


	char ret_msg[1024];//
	int i;

	char data[5][20];
	//test for air conditioner
	sprintf(data[0],"3.42");//temperature
	sprintf(data[1],"3.42");//humidity
	sprintf(data[2],"7");
	data[3][0]='f';
	send2server(4,data);

	for(i=0;i<4;i++)
	 receive4server(4,ret_msg);

	//test for face detection
	data[0][0]='3';
	sprintf(data[1],"3842");
	sprintf(data[2],"34:42:65");
	data[3][0]='0';
	send2server(28,data);
	return 0;
}*/
/**************************************************************
功能：从字符串src中分析出网站地址和端口，并得到用户要下载的文件
***************************************************************/
void GetHost(char* src, char* web, char* file, int* port)
{
    char* pA;
    char* pB;
    memset(web, 0, sizeof(web));
    memset(file, 0, sizeof(file));
    *port = 0;
    if(!(*src))
    {
        return;
    }
    pA = src;
    if(!strncmp(pA, "http://", strlen("http://")))
    {
        pA = src+strlen("http://");
    }
    else if(!strncmp(pA, "https://", strlen( "https://")))
    {
        pA = src+strlen( "https://");
    }
    pB = strchr(pA, '/');
    if(pB)
    {
        memcpy(web, pA, strlen(pA)-strlen(pB));
        if(pB+1)
        {
            memcpy(file, pB+1, strlen(pB)-1);
            file[strlen(pB)-1] = 0;
        }
    }
    else
    {
        memcpy(web, pA, strlen(pA));
    }
    if(pB)
    {
        web[strlen(pA) - strlen(pB)] = 0;
    }
    else
    {
        web[strlen(pA)] = 0;
    }
    pA = strchr(web, ':');
    if(pA)
    {
        *port = atoi(pA + 1);
    }
    else
    {
        *port = 80;
    }
}
