#include "sched.h"
#include "sys_call.h"
#include "../drivers/screen.h"

union task_union {
	struct task_struct task;
	char stack[PAGE_SIZE];
};

static union task_union init_task = {INIT_TASK,};
struct task_struct *current = &(init_task.task);

struct task_struct * task[NR_TASKS] = {&(init_task.task), };

/**
 * 任务调度
 */
void schedule(void){
    if(current->counter==0){
    	print("schedule\n");
        current->counter=10000;
    }else{
    	current->counter--;
    } 	
}

/**
 *
 * 创建进程
 */
int sys_fork(){
   print("fork\n");  
}

int sys_write(char *msg){
   print(msg);
}

void sched_init(void){
  print_num(current->counter);
  print("\n");
  print_num(current->state);
  print("\n");
  int i;
  struct desc_struct *p;
  //设置0号任务的tss和ldt段描述符
  set_tss_desc(gdt+FIRST_TSS_ENTRY,&(init_task.task.tss));
  set_ldt_desc(gdt+FIRST_LDT_ENTRY,&(init_task.task.ldt));
  p = gdt+2+FIRST_TSS_ENTRY;
  //__asm__("pushfl ; andl $0xffffbfff,(%esp) ; popfl");
  ltr(0);//加载任务0 tss到tr寄存器 此时并不会引发任务切换
  lldt(0);
}
