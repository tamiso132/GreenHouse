#include "Headers/DisplayBoard.h"
#include "Headers/Date.h"
#include "Headers/pio.h"
#include <string.h>

struct Display_Board {
    Pin WR;    // read:1, write:0
    Pin RD;    // ACTIVE LOW: READ
    Pin CE;    // ACTIVE LOW: Chip Select
    Pin C_D;   // REGISTER SELECT SIGNAL, 1: Command , 0: Data
    Pin Reset; // active,low reset signal
    Pin DB[8];
    // data
    Pin FS; // FONT SELECT SIGNAL H:6x8, L:8x8
    Pin RV; // DISPLAY MODE SIGNAL, 1: REVERSE DISPLAY, 0: NORMAL DISPLAY
} typedef Display_Board;

volatile Pin DIR;

void Write_Command_2_Display (unsigned char Command);
void Write_Data_2_Display (unsigned char data);

volatile Display_Board board;
void DelayYY (int value) {
    for (int i = 0; i < value; i++)
        asm ("NOP");
}

void clear_screen () {
    Write_Data_2_Display ((0x00));
    Write_Data_2_Display (0x00);
    Write_Command_2_Display (0x24);

    for (int i = 0; i < 500; i++) {
        Write_Data_2_Display (0x00);
        Write_Command_2_Display (0xC0);
    }

    Write_Data_2_Display ((0x00));
    Write_Data_2_Display (0x00);
    Write_Command_2_Display (0x24);
}

void clear_specific_row_screen (int row) {
    unsigned short over = (0x1E) * row; // 8
    unsigned char lo    = over;
    unsigned char hi    = over >> 8;

    Write_Data_2_Display (lo);

    Write_Data_2_Display (hi);

    Write_Command_2_Display (0x24);
    for (int i = 0; i < 0x1E; i++) {
        Write_Data_2_Display (0x00);
        Write_Command_2_Display (0xC0);
    }
}

void Init_Display () {

    { // #region initilize db

        board.DB[0] = pin_new (PIO_PIOC, 2); // pin 34

        board.DB[1] = pin_new (PIO_PIOC, 3); // pin 35

        board.DB[2] = pin_new (PIO_PIOC, 4); // pin 36

        board.DB[3] = pin_new (PIO_PIOC, 5); // pin 37

        board.DB[4] = pin_new (PIO_PIOC, 6); // pin 38

        board.DB[5] = pin_new (PIO_PIOC, 7); // pin 39

        board.DB[6] = pin_new (PIO_PIOC, 8); // pin 39

        board.DB[7] = pin_new (PIO_PIOC, 9); // pin 39
    }

    { // #region initlize the rest of board
        board.FS = pin_new (PIO_PIOC, 18);

        board.WR = pin_new (PIO_PIOC, 17);

        board.RD = pin_new (PIO_PIOC, 16);

        board.CE = pin_new (PIO_PIOC, 15);

        board.C_D = pin_new (PIO_PIOC, 14);

        board.Reset = pin_new (PIO_PIOD, 0);

        DIR = pin_new (PIO_PIOC, 13);
    }
    pio_toggle_PIN (board.CE, 1);
    pio_toggle_PIN (board.FS, 0);

    pio_toggle_PIN (board.Reset, 0);
    DelayYY (100);
    pio_toggle_PIN (board.Reset, 1);

    { // #region startup commands
        Write_Data_2_Display (0x00);
        Write_Data_2_Display (0x00);

        Write_Command_2_Display (0x40); // set text home adress

        Write_Data_2_Display (0x00);
        Write_Data_2_Display (0x40);

        Write_Command_2_Display (0x42); // set graphic home adress

        Write_Data_2_Display (0x1e);
        Write_Data_2_Display (0x00);

        Write_Command_2_Display (0x41); // Set Text Area

        Write_Data_2_Display (0x1e);
        Write_Data_2_Display (0x00);

        Write_Command_2_Display (0x43); // Set graphic area
        Write_Command_2_Display (0x80); // text mode // 1000 0000
        Write_Command_2_Display (0x94); // Text on graphic off

        Write_Data_2_Display (0x00);
        Write_Data_2_Display (0x00);
        Write_Command_2_Display (0x24);
    }

    { // CLEAR SCREEN
        for (int i = 0; i < 500; i++) {
            Write_Data_2_Display (0x00);
            Write_Command_2_Display (0xC0);
        }

        Write_Data_2_Display (0x00);
        Write_Data_2_Display (0x00);
        Write_Command_2_Display (0x24); // set cursor to start
    }
}

unsigned char Read_Status_Display () {
    unsigned char temp;
    { // before communication
        for (int i = 0; i < 8; i++) {
            pio_read_pin (board.DB[i]); // reading a pin, disable output
        }

        pio_toggle_PIN (DIR, 1);
        pio_toggle_PIN (board.C_D, 1);

        pio_toggle_PIN (board.RD, 0);
        pio_toggle_PIN (board.WR, 1);
    }
    pio_toggle_PIN (board.CE, 0);

    for (int i = 0; i < 8; i++) {
        pio_read_pin (board.DB[i]); // reading a pi
    }
    pio_toggle_PIN (board.CE, 1);

    int STA1 = pio_read_pin (board.DB[1]);
    int STA0 = pio_read_pin (board.DB[0]);

    temp = STA0 & STA1; // read first and second bit

    pio_toggle_PIN (board.CE, 1); // disable communication
    pio_toggle_PIN (DIR, 0);
    return temp;
}

void Write_Command_2_Display (unsigned char Command) {
    while (Read_Status_Display () == 0) {
    }
    { // before communication
        for (int i = 0; i < 8; i++) {
            int val = (Command >> i) & 1;
            pio_toggle_PIN (board.DB[i], val); // sets the databus as output
        }

        pio_toggle_PIN (DIR, 0); // set dir to output

        pio_toggle_PIN (board.C_D, 1); // set command
        pio_toggle_PIN (board.WR, 0);  // clear write
        pio_toggle_PIN (board.RD, 1);  // set read
    }

    pio_toggle_PIN (board.CE, 0);

    DelayYY (40);
    pio_toggle_PIN (board.CE, 1);
    pio_toggle_PIN (DIR, 1);
}

void Write_Data_2_Display (unsigned char data) {
    while (Read_Status_Display () == 0) {
    }
    { // before communication
        for (int i = 0; i < 8; i++) {
            int val = (data >> i) & 1;
            pio_toggle_PIN (board.DB[i], val); // sets the databus as output
        }

        pio_toggle_PIN (DIR, 0); // set dir to output

        pio_toggle_PIN (board.C_D, 0); // set data
        pio_toggle_PIN (board.WR, 0);  // clear write
        pio_toggle_PIN (board.RD, 1);  // set read
    }

    pio_toggle_PIN (board.CE, 0);

    DelayYY (50);
    pio_toggle_PIN (board.CE, 1);
    pio_toggle_PIN (DIR, 1);
}

void write_text (char* a, int row, int spaces) {

    unsigned short over = (0x1E) * row + spaces; // 8
    unsigned char lo    = over;
    unsigned char hi    = over >> 8;

    Write_Data_2_Display (lo);

    Write_Data_2_Display (hi);

    Write_Command_2_Display (0x24);

    unsigned short cursor_position = hi | lo;
    int index = 0;
    while (a[index] != NULL) {
        if (a[index] == '\n') {
            unsigned short diff_to_next_row = cursor_position % 0x1E;
            cursor_position += 0x1E - diff_to_next_row;
            
            unsigned char lo    = cursor_position;
            unsigned char hi    = cursor_position >> 8;

            Write_Data_2_Display (lo);

            Write_Data_2_Display (hi);

            Write_Command_2_Display (0x24);
            index++;
            continue;
        }
        unsigned char temp = a[index] - 32;

        Write_Data_2_Display (temp);
        Write_Command_2_Display (0xC0);
        index++;
    }
}
