
#include "Headers/pio.h" // we are dividing by 4. because how int* adding works. it adds like if it were byte and not bits.
#define PIO_IER (0x0040 / 4)
#define PIO_REHLSR (0x00D4)
#define PIO_SODR (0x0030 / 4)
#define PIO_CODR (0x0034 / 4)
#define PIO_PDSR (0x003C / 4)
#define PIO_OER (0x0010 / 4)
#define PIO_PUDR (0x0060 / 4)
#define PIO_PDR (0x004 / 4)
#define PIO_FELLSR (0x00D0 / 4)
#define PIO_ESR (0x00C0 / 4)
#define PIO_ODR (0x0014 / 4)
#define PIO_IFER (0x0020 / 4)
#define PIO_AIMER (0x00B0 / 4)
#define PIO_SCDR (0x008C / 4)
#define PIO_ISR (0x004C / 4)
#define PIO_DIFSR (0x0084 / 4)

// enable pio controller

void pio_initilize_pin (Pin pin) {

    int mask           = 1 << pin.bit_number;
    volatile int* base = (void*)pin.controller_port;

    *(base)            = mask; // enable pio controller
    *(base + PIO_PUDR) = mask; // disable pullup
}

void pio_toggle_PIN (struct Pin pin, int toggle) {
    volatile int* base = (void*)pin.controller_port;
    int mask           = 1 << pin.bit_number;

    *(base + PIO_OER) = mask; // enable output
    if (toggle == 1) {
        *(base + PIO_SODR) = mask; // set pin
    } else if (toggle == 0) {
        *(base + PIO_CODR) = mask; // clear pin
    }
}

int pio_read_pin (Pin pin) {
    volatile int* base = (void*)pin.controller_port;
    int mask           = 1 << pin.bit_number;

    *(base + PIO_ODR) = mask;                          // disable output
    return (*(base + PIO_PDSR) >> pin.bit_number) & 1; // pin data status register
}

Pin pin_new (PIO type, int bit_number) // creates and inilizeses pin
{
    Pin pin;
    pin.bit_number      = bit_number;
    pin.controller_port = type;

    { // initlize
        int mask           = 1 << pin.bit_number;
        volatile int* base = (void*)pin.controller_port;

        *(base)            = mask; // enable pio controller
        *(base + PIO_PUDR) = mask; // disable pullup
    }
    return pin;
};
