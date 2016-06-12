#include "plc.h"

int main (){

	PLC_init();

	strcpy(plc_write_buf,"hello world!");

	while(1){
		send2plc(16);
		sleep(1);
	}
	
	return 0;
}