#include "plc.h"

int main (){

	PLC_init();

	strcpy(plc_write_buf,"hello!");

	while(1){
		send2plc();
		sleep(1);
	}
	
	return 0;
}