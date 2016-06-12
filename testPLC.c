#include "plc.h"

int main(){
	int n=0;
	int len=0;
	char tmp[16];
	int flag=0;
	PLC_init();
	// sleep(2);
	while(1){
		flag=get4plc(tmp);
		if(flag) printf("%s\n", tmp);
		sleep(1);
	}

	return 0;

}