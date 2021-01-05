#include "../include/sched.h"
#include "../include/screen.h"
#include "../include/system.h"

/**
 *
 * 创建进程
 * 参数的顺序与压入栈的顺序相反
 * ss   用户空间栈段寄存器
 * esp  用户空间栈顶寄存器
 * eflags 用户空间的flags寄存器
 * cs   用户空间代码段寄存器
 * eip 用户空间调用fork下一条指令
 * ds
 * es
 * fs
 * eax
 * edx
 * ecx
 * ebx
 * eip sys_fork下一条指令
 * gs
 * esi
 * edi
 * ebp
 */
int copy_process(long ebp, long edi, long esi, long gs, long none, long ebx,
		long ecx, long edx, long orig_eax, long fs, long es, long ds, long eip,
		long cs, long eflags, long esp, long ss) {
	print("fork\n");
	struct task_struct *new;
	//查找一个空的task_struct
	int i;
	for (i = 1; i < NR_TASKS; i++) {
		if (task[i] == NULL) {
			break;
		}
	}
	if (!i) {
		return -1;
	}
	//为task_struct分配空间
	unsigned long basemem = 0xe000;
	unsigned int size = 0x5000;
	new = (struct task_struct *) (basemem + (i - 1) * size);
	struct desc_struct none_ = { .a = 0, .b = 0 };
	struct desc_struct code_ = { .a = 0x9f, .b = 0xc0fa00 };
	struct desc_struct data_ = { .a = 0x9f, .b = 0xc0f200 };
	//复制当前任务
	*new = *current;
	new->pid = i;
	new->state = 0;
	new->counter = 15;
	new->tss.esp0 = 4096 + (long) new;
	new->tss.ss0 = 0x10;
	new->tss.eip = eip;
	new->tss.eflags = eflags;
	new->tss.eax = 0; //返回值
	new->tss.ecx = ecx;
	new->tss.edx = edx;
	new->tss.ebx = ebx;
	new->tss.esp = esp;
	new->tss.ebp = ebp;
	new->tss.esi = esi;
	new->tss.edi = edi;
	new->tss.es = es & 0xffff;
	new->tss.cs = cs & 0xffff;
	new->tss.ss = ss & 0xffff;
	new->tss.ds = ds & 0xffff;
	new->tss.fs = fs & 0xffff;
	new->tss.gs = gs & 0xffff;
	//new->ldt = {{0,0},{0x9f,0xc0fa00},{0x9f,0xc0f200}};
	new->ldt[0] = none_;
	new->ldt[1] = code_;
	new->ldt[2] = data_;
	new->tss.ldt = _LDT(i);
	new->tss.trace_bitmap = 0x80000000;

	set_tss_desc(gdt + (i << 1) + FIRST_TSS_ENTRY, &(new->tss));
	set_ldt_desc(gdt + (i << 1) + FIRST_LDT_ENTRY, &(new->ldt));
	task[i] = new;
	return i;
}