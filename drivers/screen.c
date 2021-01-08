#include "../include/screen.h"
#include "../include/io.h"

#define ROW 18
#define COL 0
#define VIDEO_MEMORY 0xb8000
#define FIRST_VIDEO (char*)(VIDEO_MEMORY+2*(ROW*80+COL))
//char* video_memory=(char*)(0xb8000+2*(16*80+0));

//通过行与列获取内存偏移量
int get_screen_offset(int col, int row) {
	unsigned char *vidmem = (unsigned char*) VIDEO_ADDRESS;
	return (int) (2 * (row * MAX_COLS + col));
}

//获取光标
int get_cursor() {
	outb(14,REG_SCREEN_CTRL);
	int offset = inb(REG_SCREEN_DATA) << 8; //光标的高8位
	outb(15,REG_SCREEN_CTRL); //光标的低8位
	offset += inb(REG_SCREEN_DATA);
	return offset * 2;

}

void set_cursor(int offset) {
	offset /= 2;
	outb(14,REG_SCREEN_CTRL);
	outb((unsigned char) (offset >> 8),REG_SCREEN_DATA); //光标高8位设置
	outb(15,REG_SCREEN_CTRL);
	outb((unsigned char) (offset & 0x00ff),REG_SCREEN_DATA); //光标的低8位
}

void print_num_(int num) {
	if (num / 10) {
		print_num_(num / 10);
	}
	unsigned char *vidmem = (unsigned char*) VIDEO_ADDRESS;
	int offset = get_cursor();
	vidmem[offset] = num % 10 + 48;
	vidmem[offset + 1] = WHITE_ON_BLACK;
	offset += 2;
	offset = handle_scrolling(offset);
	set_cursor(offset);
}

void print_num(int num) {
	//char* video_memory=FIRST_VIDEO;
	print_num_(num);
}

void memory_copy(char *source, char *dest, int no_bytes) {
	int i;
	for (i = 0; i < no_bytes; i++) {
		*(dest + i) = *(source + i);
	}
}

int handle_scrolling(int cursor_offset) {
	if (cursor_offset < MAX_ROWS * MAX_COLS * 2) {
		return cursor_offset;
	}
	int i;
	for (i = 1; i < MAX_ROWS; i++) {
		memory_copy((char*) (get_screen_offset(0, i) + VIDEO_ADDRESS),
				(char*) (get_screen_offset(0, i - 1) + VIDEO_ADDRESS),
				MAX_COLS * 2);
	}
	char* last_line = (char*) (get_screen_offset(0, MAX_ROWS - 1)
			+ VIDEO_ADDRESS);
	for (i = 0; i < MAX_COLS * 2; i++) {
		last_line[i] = 0;
	}
	cursor_offset -= 2 * MAX_COLS;
	return cursor_offset;
}

void print_char(char character, int col, int row, char attribute_type) {

	unsigned char *vidmem = (unsigned char*) VIDEO_ADDRESS;
	if (!attribute_type) {
		attribute_type = WHITE_ON_BLACK;
	}

	int offset;
	if (col >= 0 && row >= 0) {
		offset = get_screen_offset(col, row);
	} else {
		//获取光标位置
		offset = get_cursor();
	}

	if (character == '\n') {
		int rows = offset / (2 * MAX_COLS);
		offset = get_screen_offset(79, rows);
	} else {
		vidmem[offset] = character;
		vidmem[offset + 1] = attribute_type;
	}

	offset += 2;
	//print_num(offset);
	offset = handle_scrolling(offset);
	//设置光标
	set_cursor(offset);
}

void print_at(char* message, int col, int row) {
	if (col >= 0 && row >= 0) {
		set_cursor(get_screen_offset(col, row));

	}
	int i = 0;
	while (message[i] != 0) {
		print_char(message[i++], col, row, WHITE_ON_BLACK);

	}

}

void print(char * message) {
	print_at(message, -1, -1);
}

void clear_screen() {
	int row = 0;
	int col = 0;
	for (row = 0; row < MAX_ROWS; row++) {
		for (col = 0; col < MAX_COLS; col++) {
			print_char(' ', col, row, WHITE_ON_BLACK);
		}

	}
	set_cursor(get_screen_offset(0, 0));

}
