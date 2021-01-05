#include "../include/screen.h"
#include "../include/sys_call.h"

int sys_write(char *msg) {
	print(msg);
}

//static char printbuf[1024];
void sys_print(buf){
	__asm__("int $0x80" ::"a"(4),"b"(buf));
}

int fork() {
	long res;
	__asm__("int $0x80" :"=a"(res):"a"(0));
	return res;
}
/*
static int printf(const char *msg) {
	int i = 0;
	for (i = 0; i < 1024; i++) {
		printbuf[i] = NULL;
	}
	i = 0;
	while (1 == 1) {
		char temp = *msg;
		if (!temp) {
			break;
		}
		printbuf[i++] = temp;
		msg++;
	}
	sys_print(printbuf);
}*/
