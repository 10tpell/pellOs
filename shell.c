#include "shell.h"
#include "peripherals/framebuffer.h"
#include "types.h"

#define INITIAL_X           15
#define INITIAL_Y           15
#define SHELL_ZOOM_LEVEL    2U
#define CHAR_WIDTH          8U
#define CHAR_HEIGHT         8U
#define SCALED_CHAR_WIDTH   CHAR_WIDTH * SHELL_ZOOM_LEVEL
#define SCALED_CHAR_HEIGHT  CHAR_HEIGHT * SHELL_ZOOM_LEVEL

#define SCREEN_WIDTH        1920U
#define SCREEN_HEIGHT       1080U

static uint16_t cursorX = INITIAL_X;
static uint16_t cursorY = INITIAL_Y;

void kernel_writeChar(char ch) {
    if(cursorX + SCALED_CHAR_WIDTH > SCREEN_WIDTH - 1) {
        cursorX = INITIAL_X;
        cursorY += SCALED_CHAR_HEIGHT;
    }
    
    if(cursorY + SCALED_CHAR_HEIGHT > SCREEN_HEIGHT - 1) {
        return; /* For now don't bother printing more than one page worth */
    }

    if(ch == '\n') {
        cursorX = INITIAL_X;
        cursorY += SCALED_CHAR_HEIGHT;
    } else if (ch == '\r') {
        cursorX = INITIAL_X;
    } else {
        framebuffer_drawChar(ch, cursorX, cursorY, 0x0F, SHELL_ZOOM_LEVEL);
        cursorX += SCALED_CHAR_WIDTH;
    }
}

void kernel_writeStr(char* str) {
    uint32_t i = 0;
    while(str[i] != 0) {
        kernel_writeChar(str[i]);
        i++;
    }
}