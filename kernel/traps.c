#include "../include/head.h"
#include "../include/system.h"
#include "../include/screen.h"

extern void keyboard_interrupt(void);
extern void page_fault(void);

void divide_error(void) {
}
void debug(void) {
}
void nmi(void) {
}
void int3(void) {
}
void overflow(void) {
}
void bounds(void) {
}
void invalid_op(void) {
}
void device_not_available(void) {
}
void double_fault(void) {
}
void coprocessor_segment_overrun(void) {
}
void invalid_TSS(void) {
}
void segment_not_present(void) {
}
void stack_segment(void) {
}
void general_protection(void) {
}
void coprocessor_error(void) {
}
void reserved(void) {
}
void parallel_interrupt(void) {
}
void irq13(void) {
}
void alignment_check(void) {
}

void trap_init() {
	set_trap_gate(0, &divide_error);
	set_trap_gate(1, &debug);
	set_trap_gate(2, &nmi);
	set_system_gate(3, &int3); /* int3-5 can be called from all */
	set_system_gate(4, &overflow);
	set_system_gate(5, &bounds);
	set_trap_gate(6, &invalid_op);
	set_trap_gate(7, &device_not_available);
	set_trap_gate(8, &double_fault);
	set_trap_gate(9, &coprocessor_segment_overrun);
	set_trap_gate(10, &invalid_TSS);
	set_trap_gate(11, &segment_not_present);
	set_trap_gate(12, &stack_segment);
	set_trap_gate(13, &general_protection);
	set_trap_gate(14, &page_fault);
	set_trap_gate(15, &reserved);
	set_trap_gate(16, &coprocessor_error);
	set_trap_gate(17, &alignment_check);
	set_trap_gate(45, &irq13);
	set_trap_gate(39, &parallel_interrupt);


	/*set_trap_gate(0x21, &keyboard_interrupt);
	register unsigned char a;
	outb_p(inb_p(0x21)&0xfd, 0x21);
	a = inb_p(0x61);
	outb_p(a|0x80, 0x61);
	outb_p(a, 0x61);*/
}
