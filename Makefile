C_SOURCES = $(wildcard init/*.c kernel/*.c drivers/*.c mm/*.c)
HEADERS = $(wildcard include/*.h)

OBJ = ${C_SOURCES:.c=.o}

all: os-image

os-image: boot/bootsect.bin kernel.bin
	cat $^ > os-image

kernel.bin: head/kernel_entry.o head/head.o  head/setup_page.o kernel/sys_call.o  ${OBJ}
	ld -m elf_i386 -s -o $@ -Ttext 0x8000 $^ --oformat binary

%.o : %.c ${HEADERS}
	gcc -m32 -ffreestanding -c $< -o $@

%.o : %.asm
	nasm $< -f elf -o $@

%.bin : %.asm
	nasm $< -f bin -o  $@

clean:
	rm -fr *.bin *.dis *.o os-image
	rm -fr kernel/*.o boot/*.bin drivers/*.o head/*.o


