
#include "Headers/main.h"
#include "../at91sam3x8.h"
#include "../exceptions.h"
#include "../system_sam3x.h"
#include "Headers/Date.h"
#include "Headers/Keypad.h"
#include "Headers/light.h"
#include "Headers/menu.h"
#include "Headers/temp.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#define dot 0x0E

unsigned int systickCounter;


Pin DisplayGate;
Pin KeypadGate;

//// LIGHTSSSSSSSSSSSSSSSSSSSSSSS

int key_press_delay = 500; // half second
int sun_pos_delay = 0;


void toggle_display_gate (bool toggle) {
    pio_toggle_PIN (DisplayGate, !toggle);
    pio_toggle_PIN (KeypadGate, toggle);
}

void Reset_All () {
    systickCounter = 0;
    fast_ticks_date = 0;
    key_press_delay = 0;
    Reset_light();
    Reset_Temp();
}

void toggle_fast_mode (bool toggle) {
    Toggle_Fast_Mode_Temp (toggle);
    Light_Toggle_Fastmode (toggle);
    Toggle_Fastmode_Date(toggle);
}
int main () { // blinka vid alarm
    {                               // INIT SYSTEM STUFF AND CLOCKS
        *AT91C_PMC_PCER  = 1 << 11; // A
        *AT91C_PMC_PCER  = 1 << 12; // B
        *AT91C_PMC_PCER  = 1 << 13; // C
        *AT91C_PMC_PCER  = 1 << 14; // D
        *AT91C_PMC_PCER1 = 1 << 4;  // PWM
        *AT91C_PMC_PCER  = 1 << 27; // TCO

        SystemInit ();
        SysTick_Config (SystemCoreClock / 1000);
    }

    { // INIT DISPLAY
        DisplayGate = pin_new (PIO_PIOC, 12);
        KeypadGate  = pin_new (PIO_PIOD, 2);

        toggle_display_gate (1);

        Init_Display ();
        Init_Start_Menu_UI ();

        toggle_display_gate (0);
    }


    Keypad_init ();
    Init_Date ();
    temp_ourInit ();
    ourInit();
    Light_Measure();
    
    int index = 0;
    while (1) {
      
        if (Light_Measure_Ready ()) {
            measure_light_delay = 0;
            Light_Measure ();
        }

        if (is_ready_temp ()) {
            temp_flag = 0;
            temp_measure_delay = 0;
            Ready_To_Insert_Temp ();
            Temp_Update ();
            if (Is_Alarm_On ()) { // if recent temp is outside the threshold
                alarm_delay = 60000;
                Init_Alarm_Popup (&index);
            }
        }
       
        if(sun_pos_delay < 0){
          Sun_Degrees();
          sun_pos_delay = 60000;
        }

        int key = -1;
        if (key_press_delay <= 0) {
            key_press_delay = 500;
            toggle_display_gate (0);
            key = press_key ();
        }
//        if (key == -1)
//            continue;
        toggle_display_gate (1);

        switch (get_state ()) {
        case State_StartMenu: Start_Menu_UI (&index, key); break;
        case State_Configuration: Date_Menu_UI (&index, key); break;
        case State_TempMenu: Temp_Menu_UI (&index ,key); break;
        case State_Alarm_Config: Alarm_UI (&index, key); break;
        case State_FastMode: Fast_Mode_UI (key); break;
        case State_Light: Light_UI (key); break;
        case State_Popup: Popup_UI (&index, key); break;
        }
    }
}


void SysTick_Handler () {


    key_press_delay--;
    temp_delay--;
    alarm_delay--; // så inte du får en popup hela tiden

    _delay_ui_rewrite--;
    sun_pos_delay--;
    measure_light_delay++;
    systickCounter++;
    temp_measure_delay++;
    pop_up_timer--;
    if(fast_date_on)
        fast_ticks_date++;
}
