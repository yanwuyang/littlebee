#include "../drivers/screen.h"
#include "sched.h"

#define ROW 18
#define COL 0
#define VIDEO_MEMORY 0xb8000
#define FIRST_VIDEO (char*)(VIDEO_MEMORY+2*(ROW*80+COL))
#define EXT_MEM_K (*(unsigned short *)0x90002)

#define _set_gate(gate_addr,type,dpl,addr)\
__asm__("movw %%dx,%%ax\n\t" \
    "movw %0,%%dx\n\t" \
    "movl %%eax,%1\n\t" \
    "movl %%edx,%2" \
    : \
    : "i"((short)(0x8000+(dpl<<13)+(type<<8))),\
      "o"(*((char *)(gate_addr))),\
      "o"(*(4+(char *)(gate_addr))),\
      "d" ((char *) (addr)),"a" (0x00080000))


void keyboard(){
  print("abcd \n");
}


extern void keyboard_interrupt(void);
extern void  timer_interrupt(void);
extern void system_call(void);
#define HZ 100
#define LATCH (1193180/HZ)
#define outb(value,port) \
__asm__ ("outb %%al,%%dx"::"a" (value),"d" (port))


#define inb(port) ({ \
unsigned char _v; \
__asm__ volatile ("inb %%dx,%%al":"=a" (_v):"d" (port)); \
_v; \
})

#define outb_p(value,port) \
__asm__ ("outb %%al,%%dx\n" \
		"\tjmp 1f\n" \
		"1:\tjmp 1f\n" \
		"1:"::"a" (value),"d" (port))

#define inb_p(port) ({ \
unsigned char _v; \
__asm__ volatile ("inb %%dx,%%al\n" \
	"\tjmp 1f\n" \
	"1:\tjmp 1f\n" \
	"1:":"=a" (_v):"d" (port)); \
_v; \
})


#define set_intr_gate(n,addr) \
	_set_gate(&idt[n],14,0,addr)


#define set_trap_gate(n,addr) \
	_set_gate(&idt[n],15,0,addr)

#define set_system_gate(n,addr) \
	_set_gate(&idt[n],15,3,addr)

void page_exception(void){
  print("page_exception");
}

void divide_error(void){
  print("divide_error");
}
void debug(void){}
void nmi(void){}
void int3(void){
  print("int3");
}
void overflow(void){
  print("overflow");
}
void bounds(void){
  print("bounds");
}
void invalid_op(void){
  print("invalid_op");
}
void device_not_available(void){}
void double_fault(void){}
void coprocessor_segment_overrun(void){}
void invalid_TSS(void){}
void segment_not_present(void){}
void stack_segment(void){}
void general_protection(void){}
void page_fault(void){}
void coprocessor_error(void){
 print("error");
}
void reserved(void){
  print("reservued");
}
void parallel_interrupt(void){
  print("parallel");
}
void irq13(void){
  print("irq13");
}
void alignment_check(void){}

char site[] = "RUNOOB";
int b=15;
void trap_init(){
	print("\n\nhello world\n");
        //print_char(49,-1,-1,WHITE_ON_BLACK);
        //int *idt_addr =(int*) &idt;     
        //print_num(idt_addr);
        //print_num(EXT_MEM_K);
	//print("\n");
        set_trap_gate(0,&divide_error);
	set_trap_gate(1,&debug);
	set_trap_gate(2,&nmi);
	set_system_gate(3,&int3);	/* int3-5 can be called from all */
	set_system_gate(4,&overflow);
	set_system_gate(5,&bounds);
	set_trap_gate(6,&invalid_op);
	set_trap_gate(7,&device_not_available);
	set_trap_gate(8,&double_fault);
	set_trap_gate(9,&coprocessor_segment_overrun);
	set_trap_gate(10,&invalid_TSS);
	set_trap_gate(11,&segment_not_present);
	set_trap_gate(12,&stack_segment);
	set_trap_gate(13,&general_protection);
	set_trap_gate(14,&page_fault);
	set_trap_gate(15,&reserved);
	set_trap_gate(16,&coprocessor_error);
	set_trap_gate(17,&alignment_check);
	set_trap_gate(45,&irq13);
	set_trap_gate(39,&parallel_interrupt);
        outb_p(0x36,0x43);
        outb_p(LATCH & 0xff , 0x40);
        outb(LATCH >> 8 , 0x40); 
        set_intr_gate(0x20,&timer_interrupt);
        outb(inb_p(0x21)&~0x01,0x21);

        set_trap_gate(0x21,&keyboard_interrupt);
        register unsigned char a;
        outb_p(inb_p(0x21)&0xfd,0x21);
	a=inb_p(0x61);
	outb_p(a|0x80,0x61);
	outb_p(a,0x61);


        set_system_gate(0x80,&system_call);
        //print_num(&idt[0x21]);
        __asm__("sti" ::);
        //__asm__("int $0x80" ::);
}

void main(){
    trap_init();//初始中断服务例程
    sched_init();//初始调度读取和0号任务
    print("sched init done\n");
    print_num(&b);
    print("\n");
    print_num(b);
    move_to_user_mode();//切换到用户模式
    printf("to user mode\n");
    sys_print("go to frok\n");
    int pid = fork();
    if(pid==1){
        for(;;){
      	   // printf("pid:0\n");
	}
    }else{
        for(;;){
           // printf("pid:1\n");
        }
    }
    for(;;){

    }

}
