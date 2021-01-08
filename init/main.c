#include "../include/screen.h"
#include "../include/sched.h"
#include "../include/system.h"


#define DRIVE_INFO ((unsigned char *)0x7f00)
void main() {
	trap_init();	//初始中断服务例程
	sched_init();	//初始调度读取和0号任务
	print_num(*DRIVE_INFO);
	sti();
	print("sched init done\n");
	move_to_user_mode();
	//切换到用户模式
	sys_print("to user mode\n");
	sys_print("go to frok\n");
	int pid = fork();
	if (pid == 1) {
		for (;;) {
			// printf("pid:0\n");
		}
	} else {
		for (;;) {
			// printf("pid:1\n");
		}
	}
	for (;;) {

	}

}
