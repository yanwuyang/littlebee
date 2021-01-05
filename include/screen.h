#define VIDEO_ADDRESS 0xb8000
#define MAX_ROWS 25
#define MAX_COLS 80

#define WHITE_ON_BLACK 0x0f

#define REG_SCREEN_CTRL 0X3D4
#define REG_SCREEN_DATA 0x3d5

void print_char(char character, int col, int row, char attribute_type);

void print(char* message);
