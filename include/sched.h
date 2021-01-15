#include "head.h"

#define PAGE_SIZE 4096
#define NR_TASKS 64
#define TASK_SIZE	0x04000000   //64KB

#define HZ 100
#define LATCH (1193180/HZ)

/*
 *tss（task state struct）结构定义参考对应的CPU架构
 */
struct tss_struct {
	long back_link;	//上一个任务链接  高16位为0
	long esp0;		//0级别内核栈顶
	long ss0;		//0级别栈段选择符 高16位为0
	long esp1;		//1级别
	long ss1;
	long esp2;
	long ss2;
	long cr3;		//页目录寄存器
	long eip;		//指令寄存器
	long eflags;		//标记寄存器
	long eax;
	long ecx;
	long edx;
	long ebx;
	long esp;
	long ebp;
	long esi;
	long edi;
	long es;		//高16位为0
	long cs;
	long ss;
	long ds;
	long fs;
	long gs;
	long ldt;	        //ldt段选择符 高16位为0
	long trace_bitmap;    //有效位 16-31
};

struct task_struct {
	long pid;
	long state;
	long counter;
	struct desc_struct ldt[3];
	struct tss_struct tss;
};


#define FIRST_TSS_ENTRY 4
#define FIRST_LDT_ENTRY (FIRST_TSS_ENTRY+1)
#define _TSS(n) ((((unsigned long) n)<<4)+(FIRST_TSS_ENTRY<<3))
#define _LDT(n) ((((unsigned long) n)<<4)+(FIRST_LDT_ENTRY<<3))
#define ltr(n) __asm__("ltr %%ax"::"a" (_TSS(n)))
#define lldt(n) __asm__("lldt %%ax"::"a" (_LDT(n)))

/*
 *0000 0000 1100 0000  1111 1010 0000 0000
 *0000 0000 0000 0000  0000 0000 1001 1111
 *
 * limit 0-15 16-19   000010011111 (9f+1)*4= 640K
 * base 0x0
 * type 1010  可读可访问
 * DPL 3
 * P 1
 * G 1
 * D 1
 *
 */

#define INIT_TASK \
 { \
   0,0,15,{{0,0},{0x9f,0xc0fa00},{0x9f,0xc0f200}}, \
    { \
       0,PAGE_SIZE+(long)&init_task,0x10,0,0,0,0,(long)&pg_dir, \
       0,0,0,0,0,0,0,0, \
       0,0,0x17,0x17,0x17,0x17,0x17,0x17, \
       _LDT(0),0x80000000 \
    } \
}

#define NULL 0

extern struct task_struct *task[NR_TASKS];
extern struct task_struct *current;
