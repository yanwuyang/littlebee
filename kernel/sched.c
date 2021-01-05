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
 *
 * ljmp 该操作数由4字节偏移地址和2字节的段选择符组成。因此_temp中的a值是32位偏移地址，而b的低字节是新的TSS段的选择符（高2字节不用）
 * jmp 16位段选择符:32位偏移地址
 */
void schedule(void){
    if(current->counter==0){
    	//print("schedule\n");
        int i;
	struct {long a,b;} __tmp;
	for(i=0;i<NR_TASKS;i++){
	    if(task[i]!=NULL && current->pid!=task[i]->pid){
		current->counter=10000;
		print_num(current->pid);
		print("\n");
                //current = task[i];
		__asm__("movw %%dx,%1\n\t" \
			"xchgl %%ecx,current\n\t" \
			"ljmp *%0"::"m" (*&__tmp.a),"m" (*&__tmp.b),"d"(_TSS(i)),"c"((long)task[i]));
                break;
	    }
        }
        //current->counter=10000;
    }else{
    	current->counter--;
    } 	
}

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
int copy_process(long ebp,long edi,long esi,long gs,long none,
		long ebx,long ecx,long edx, long orig_eax, 
		long fs,long es,long ds,
		long eip,long cs,long eflags,long esp,long ss){
   print("fork\n"); 
   struct task_struct *new;
   //查找一个空的task_struct
   int i;
   for(i=1;i<NR_TASKS;i++){
       if(task[i]==NULL){
	  break;	
       }
   }
   if(!i){
       return -1;
   }
   //为task_struct分配空间 
   unsigned long basemem = 0xe000;
   unsigned int size = 0x5000;
   new = (struct task_struct *)(basemem+(i-1)*size);	
   struct desc_struct none_ = {.a=0,.b=0};
   struct desc_struct code_ = {.a=0x9f,.b=0xc0fa00};
   struct desc_struct data_ = {.a=0x9f,.b=0xc0f200};
   //复制当前任务
   *new = *current;
   new->pid = i;
   new->state = 0;
   new->counter = 15;
   new->tss.esp0= 4096+(long)new;
   new->tss.ss0 = 0x10;
   new->tss.eip = eip;
   new->tss.eflags = eflags;
   new->tss.eax = 0;//返回值
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
    
   set_tss_desc(gdt+(i<<1)+FIRST_TSS_ENTRY,&(new->tss));
   set_ldt_desc(gdt+(i<<1)+FIRST_LDT_ENTRY,&(new->ldt));
   task[i] = new;
   return i;	
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
  for(i=1;i<NR_TASKS;i++) {
	task[i] = NULL;
	p->a=p->b=0;
	p++;
	p->a=p->b=0;
	p++;
  }
  //__asm__("pushfl ; andl $0xffffbfff,(%esp) ; popfl");
  ltr(0);//加载任务0 tss到tr寄存器 此时并不会引发任务切换
  lldt(0);
}
