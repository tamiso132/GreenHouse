#include "Headers/Keypad.h"
#include <stdio.h>
//( // define numbers in hex

volatile Keypad pad;

int press_key () // return hexDecimal. if it returns -1, it did not work
{
    for (int column_i = 0; column_i < sizeof (pad.column) / sizeof (Pin); column_i++) {
        // set one column and the rest clear
        pio_toggle_PIN (pad.column[0], 1);
        pio_toggle_PIN (pad.column[1], 1);
        pio_toggle_PIN (pad.column[2], 1);

        pio_toggle_PIN (pad.column[column_i], 0);

        for (int row_i = 0; row_i < sizeof (pad.rows) / sizeof (Pin); row_i++) {
            int val = pio_read_pin (pad.rows[row_i]);
            if (val == 0) {

                int number = (row_i * 3 + column_i + 1);
                return number;
            }
        }
    }
    return -1;
}
void Keypad_init () {
    pad.rows[0] = pin_new (PIO_PIOC, 2); // 34
    pad.rows[1] = pin_new (PIO_PIOC, 3); // 35
    pad.rows[2] = pin_new (PIO_PIOC, 4); // 36
    pad.rows[3] = pin_new (PIO_PIOC, 5); // 37

    pad.column[0] = pin_new (PIO_PIOC, 7); // 39
    pad.column[1] = pin_new (PIO_PIOC, 8); // 40
    pad.column[2] = pin_new (PIO_PIOC, 9); // 41
}
