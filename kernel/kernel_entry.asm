[bits 16]
[extern switch_to_pm]
global _start
 _start:

call switch_to_pm

jmp $
