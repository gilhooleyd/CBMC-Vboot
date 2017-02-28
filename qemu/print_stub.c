/* vga_x86.c
 *
 * Description: Module implementing the tty.h
 * interface for VGA on the x86 architecture.
 */


#include "tty.h"

#define TRUE 1
#define FALSE 0

/* VGA Screen constants */
enum {
    VGA_WIDTH = 80,
    VGA_HEIGHT = 25
};

/* Hardware text mode color constants. */
typedef enum {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
} vga_color;

/* Terminal interfacing constants */
enum {
    TAB_SPACES = 4,
};


/* Whether this module is initialized */
static int is_initialized;

/* Terminal screen variables */
static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t *terminal_buffer;


/* Forward declare methods */
int terminal_init(void);
int terminal_is_initialized(void);
void terminal_writestring(const char *);
void terminal_write(const char *, size_t);
void terminal_putchar(char);
void terminal_putentryat(char, size_t, size_t, uint8_t);
void terminal_setcolor(vga_color, vga_color);
static int inc_terminal_row();
static int inc_terminal_column();
static size_t to_nearest_multiple(size_t start, size_t multiple);
static inline uint8_t vga_entry_color(vga_color, vga_color);
static inline uint16_t vga_entry(unsigned char, uint8_t);
static size_t strlen(const char* str);
 
void printDigit(int dig){ 
    char codes[] = "0123456789ABCDEF";
    terminal_putchar(codes[dig]);
}

void printHex(int num) {
    int tmp = num;
    if (num < 0x10) {
        printDigit(num); 
        return;
    }
    else {
        printHex  (num / 0x10);
        printDigit (num % 0x10);
    }    
}

void printNum(int num) {
    int tmp = num;
    if (num < 10) {
        printDigit(num); 
        return;
    }
    else {
        printNum  (num / 10);
        printDigit(num % 10);
    }    

}


/* Initializes terminal module for use.      *
 * Must be called before all other methods.  *
 * Returns whether initialization succeeded. */
int terminal_init(void) {

    // Initialize screen variables
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t*) 0xB8000;

    // Initialize terminal buffer
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}

    is_initialized = 1;
    return is_initialized;
}

/* Returns whether the module is initialized. */
int terminal_is_initialized(void) {
    return is_initialized;
}


/* Writes a string to the terminal */
void terminal_writestring(const char* data) {
	terminal_write(data, strlen(data));
}

/* Writes 'size' chars of 'data' to the terminal */
void terminal_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}


/* Writes the character to the terminal */
void terminal_putchar(char c) {
    size_t to_tab_multiple;

    // Handle new line character
    if (c == '\n') {
        terminal_column = 0;
        inc_terminal_row();
    }

    // Handle tab character
    else if (c == '\t') {
        to_tab_multiple = to_nearest_multiple(terminal_column, TAB_SPACES);
        while (to_tab_multiple--) {
            terminal_putchar(' ');
            if (terminal_column == 0) {
                break;
            }
        }
    }

    // Handle general case
    else {
	    terminal_putentryat(c, terminal_column, terminal_row, terminal_color);
        if (inc_terminal_column()) {
            inc_terminal_row();
        }
    }
}

/* Writes a character with a specific position and color */
void terminal_putentryat(char c, size_t x, size_t y, uint8_t color) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}


/* Sets the terminal foreground and background */
void terminal_setcolor(vga_color fg, vga_color bg) {
	terminal_color = vga_entry_color(fg, bg);
}


/* Increments terminal_column variable and  *
 * returns whether the variable overflowed. */
static int inc_terminal_column() {
    if (++terminal_column == VGA_WIDTH) {
        terminal_column = 0;
        return TRUE;
    }
    return FALSE;
}

/* Increments terminal_row variable and     *
 * returns whether the variable overflowed. */
static int inc_terminal_row() {
    if (++terminal_row == VGA_HEIGHT) {
        terminal_row = 0;
        return TRUE;
    }
    return FALSE;
}


/* Returns addend required to reach   *
 * next highest multiple of a number. */
static size_t to_nearest_multiple(size_t base, size_t multiple) {
    return multiple - (base % multiple);
}


/* Returns the color code for a terminal *
 * foreground + background color pair.   */
static inline uint8_t vga_entry_color(vga_color fg, vga_color bg) {
	return fg | bg << 4;
}

/* Returns the terminal code for a character and color code */
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
	return (uint16_t) uc | (uint16_t) color << 8;
}


/* TODO: Move this someplace else! */
size_t strlen(const char* str) {
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}
