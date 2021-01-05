#include "../include/sched.h"
#include "../include/io.h"
#include "../include/screen.h"
#include "../include/system.h"

extern int timer_interrupt(void);
extern int system_call(void);

union task_union {
	struct task_struct task;
	char stack[PAGE_SIZE];
};

static union task_union init_task = { INIT_TASK, };
struct task_struct *current = &(init_task.task);

struct task_struct * task[NR_TASKS] = { &(init_task.task), };

/**
 * 任务调度
 *
 * ljmp 该操作数由4字节偏移地址和2字节的段选择符组成。因此_temp中的a值是32位偏移地址，而b的低字节是新的TSS段的选择符（高2字节不用）
 * jmp 16位段选择符:32位偏移地址
 */
void schedule(void) {
	if (current->counter == 0) {
		//print("schedule\n");
		int i;
		struct {
			long a, b;
		} __tmp;
		for (i = 0; i < NR_TASKS; i++) {
			if (task[i] != NULL && current->pid != task[i]->pid) {
				current->counter = 10000;
				print_num(current->pid);
				print("\n");
				//current = task[i];
				__asm__("movw %%dx,%1\n\t"
						"xchgl %%ecx,current\n\t"
						"ljmp *%0"::"m" (*&__tmp.a),"m" (*&__tmp.b),"d"(_TSS(i)),"c"((long)task[i]));
				break;
			}
		}
		//current->counter=10000;
	} else {
		current->counter--;
	}
}


void sched_init(void) {
	int i;
	struct desc_struct *p;
	//设置0号任务的tss和ldt段描述符
	set_tss_desc(gdt + FIRST_TSS_ENTRY, &(init_task.task.tss));
	set_ldt_desc(gdt + FIRST_LDT_ENTRY, &(init_task.task.ldt));
	p = gdt + 2 + FIRST_TSS_ENTRY;
	for (i = 1; i < NR_TASKS; i++) {
		task[i] = NULL;
		p->a = p->b = 0;
		p++;
		p->a = p->b = 0;
		p++;
	}
	//__asm__("pushfl ; andl $0xffffbfff,(%esp) ; popfl");
	ltr(0);//加载任务0 tss到tr寄存器 此时并不会引发任务切换
	lldt(0);

	outb_p(0x36, 0x43);
	outb_p(LATCH & 0xff, 0x40);
	outb(LATCH >> 8, 0x40);
	set_intr_gate(0x20, &timer_interrupt);
	outb(inb_p(0x21) & ~0x01, 0x21);//开启中断 在初始中断芯片的时候默认屏蔽了所有中断

	set_system_gate(0x80, &system_call);
}
