#pragma once

typedef enum PIO {
    PIO_PIOA = 0x400E0E00,
    PIO_PIOB = 0x400E1000,
    PIO_PIOC = 0x400E1200,
    PIO_PIOD = 0x400E1400,
} PIO;
struct Pin {
    int bit_number;
    PIO controller_port;
} typedef Pin;

void pio_toggle_PIN (struct Pin pin, int toggle);

int pio_read_pin (Pin pin);

Pin pin_new (PIO type, int bit_number); // creates and inilizeses pin
