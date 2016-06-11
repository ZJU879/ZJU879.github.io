#include "serial.h"

int main(){
	int n=0;
	int len=0;
	PLC_init();
	sleep(2);
	get4plc();
	printf("%s\n", plc_read_buf);

	plc_write_buf[0]='h';
	plc_write_buf[0]='e';
	plc_write_buf[0]='l';
	plc_write_buf[0]='l';
	plc_write_buf[0]='o';
	plc_write_buf[0]='\n';
	send2plc();

}