
#include "Headers/menu.h"
#include "Headers/Date.h"
#include "Headers/DisplayBoard.h"
#include "Headers/light.h"
#include "Headers/main.h"
#include "Headers/temp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void Init_Temp ();
void Init_Temp_UI ();
void Init_Start_Menu_UI ();
void Init_Date_UI ();
void Init_Fast_Mode ();
void Init_Change_Date ();
void Init_Alarm_Config ();
void Init_Light_UI (int* index);


StateMachine_UI State_Machine = State_StartMenu;

StateMachine_UI after_popup_state = State_StartMenu;

StateMachine_UI get_state () {
    return State_Machine;
}

// Menu Variables
int start_max_option = 5;
int date_max_option  = 2;
int temp_options     = 0;

bool fastMode         = 0;
bool change_date_mode = false;
Date date_UI;
bool check_day_temp = false;

int _delay_ui_rewrite = 0;

float pop_up_timer = 0;
float pop_up_time = 200;
bool written_popUI = false;
void Start_Menu_UI (int* index, int key_pressed) {
    if (key_pressed == 8) {

        write_text ("  ", *index, 0);
        *index = ((*index - 1) + start_max_option) % start_max_option;
        write_text ("->", *index, 0);
    }
    if (key_pressed == 2) {
        write_text ("  ", *index, 0);
        *index = (*index + 1) % start_max_option;
        write_text ("->", *index, 0);
    }
    if (key_pressed == 5) {
        switch (*index) {
        case 0:
            Init_Date_UI (index);
            *index = 1;
            break;
        case 1:
            Init_Temp_UI ();
            *index = 0;
            break;
        case 2:
            *index = 8;
            Init_Alarm_Config ();
            break;
        case 3:
            *index = 0;
            Init_Fast_Mode ();
            break;

        case 4: Init_Light_UI (index); break;
        }
    }
}

void Date_Menu_UI (int* index, int key_pressed) {
    if (change_date_mode == false) {
        if (_delay_ui_rewrite < 0 && !fastMode) {
            _delay_ui_rewrite = 1000;
            clear_specific_row_screen (0);


            char buffer[100];
            char* d = Date_Stamp ();
            char* c = Clock_Stamp ();
            sprintf (buffer, "%s %s", d, c);
            write_text (buffer, 0, 0);
            write_text ("Change Date", 1, 2);
            write_text ("Back", 2, 2);

            free (d);
            free (c);
        } else if (fastMode && _delay_ui_rewrite < 0) {
            _delay_ui_rewrite = 500;
            clear_specific_row_screen (0);


            char buffer[100];
            char* d = Date_Stamp ();
            char* c = Clock_Stamp ();
            sprintf (buffer, "%s %s", d, c);
            write_text (buffer, 0, 0);
            write_text ("Change Date", 1, 2);
            write_text ("Back", 2, 2);

            free (d);
            free (c);
        }
        switch (key_pressed) {
        case 2:
            write_text ("  ", *index, 0);
            *index = ((*index) % 2) + 1;
            write_text ("->", *index, 0);
            break;
        case 8:
            write_text ("  ", *index, 0);
            *index = ((*index) % 2) + 1;
            write_text ("->", *index, 0);
            break;
        case 5:
            write_text ("  ", *index, 0);
            if (*index == 2) {
                *index = 0;
                Init_Start_Menu_UI ();
                return;
            } else if (*index == 1) {
                *index = 0;
                Init_Change_Date ();
            }
        }
    } else {
        switch (key_pressed) {
        case 4:

            write_text (" ", 1, *index);


            *index += 1;
            if (*index == 2 || *index == 5 || *index == 10 || *index == 13 || *index == 16) {

                *index += 1;

            } else if (*index >= 19) {
                *index = 0;
            }

            write_text ("^", 1, *index);

            break;

        case 6:
            write_text (" ", 1, *index);


            *index -= 1;
            if (*index == 2 || *index == 5 || *index == 10 || *index == 13 || *index == 16) {
                *index -= 1;

            } else if (*index < 0) {
                *index = 18;
            }
            write_text ("^", 1, *index);
            break;
        case 8:
            switch (*index) {
            case 0: date_UI.day += 10; break;
            case 1: date_UI.day += 1; break;

            case 3: date_UI.month += 10; break;
            case 4: date_UI.month += 1; break;

            case 6: date_UI.year += 1000; break;
            case 7: date_UI.year += 100; break;
            case 8: date_UI.year += 10; break;
            case 9: date_UI.year += 1; break;

            case 11: date_UI.hour += 10; break;
            case 12: date_UI.hour += 1; break;
            case 14: date_UI.minute += 10; break;

            case 15: date_UI.minute += 1; break;

            case 17: date_UI.second += 10; break;
            case 18: date_UI.second += 1; break;
            }

            Set_Date_Bounds (&date_UI);
            clear_specific_row_screen (0);

            char buffer[200];


            sprintf (buffer, "%02d/%02d/%d %02d:%02d:%02d", date_UI.day, date_UI.month,
            date_UI.year, date_UI.hour, date_UI.minute, date_UI.second);
            write_text (buffer, 0, 0);
            break;

        case 2:
            switch (*index) {
            case 0: date_UI.day -= 10; break;
            case 1: date_UI.day -= 1; break;

            case 3: date_UI.month -= 10; break;
            case 4: date_UI.month -= 1; break;

            case 6: date_UI.year -= 1000; break;
            case 7: date_UI.year -= 100; break;
            case 8: date_UI.year -= 10; break;
            case 9: date_UI.year -= 1; break;

            case 11: date_UI.hour -= 10; break;
            case 12: date_UI.hour -= 1; break;
            case 14: date_UI.minute -= 10; break;

            case 15: date_UI.minute -= 1; break;

            case 17: date_UI.second -= 10; break;
            case 18: date_UI.second -= 1; break;
            }

            Set_Date_Bounds (&date_UI);
            clear_specific_row_screen (0);

            char buf[200];


            sprintf (buf, "%02d/%02d/%d %02d:%02d:%02d", date_UI.day, date_UI.month,
            date_UI.year, date_UI.hour, date_UI.minute, date_UI.second);
            write_text (buf, 0, 0);
            break;

        case 12:
            set_time (date_UI); //
            *index           = 1;
            change_date_mode = false;
            Reset_All ();
            Init_Date_UI (index);
            DelayYY (1000);
            break;
        }
    }
}


void Temp_Menu_UI (int* index, int key_pressed) {
    if (check_day_temp) {
        if (key_pressed == 12) {
            clear_screen ();
            Init_Temp_UI ();
            check_day_temp = false;
            *index         = 0;
        }
    } else {
        if (key_pressed == 12) {
            Init_Start_Menu_UI ();
        }

        if (key_pressed == 8) {

            write_text ("  ", *index, 0);
            *index = ((*index - 1) + temp_options) % temp_options;
            write_text ("->", *index, 0);
        }
        if (key_pressed == 2) {
            write_text ("  ", *index, 0);
            *index = (*index + 1) % temp_options;
            write_text ("->", *index, 0);
        }


        else if (key_pressed == 5) {
            clear_screen ();
            char* d = get_day (*index);
            write_text (d, 0, 0);
            check_day_temp = true;
        }
    }
}

void Alarm_UI (int* index, int key_pressed) {
    // 8,9
    // 19 20
    char buffer[50] = { 0 };
    switch (key_pressed) {
    case 4:
        write_text (" ", 1, *index);
        *index += 1;
        if (*index > 20)
            *index = 8;
        else if (*index == 10)
            *index += 9;

        write_text ("^", 1, *index);
        break;
    case 6:
        write_text (" ", 1, *index);
        *index -= 1;
        if (*index < 8)
            *index = 20;
        else if (*index == 18)
            *index -= 9;

        write_text ("^", 1, *index);
        break;


    case 8:
        switch (*index) {
        case 8: alarm_max_temp += 10; break;
        case 9: alarm_max_temp += 1; break;
        case 19: alarm_min_temp += 10; break;
        case 20: alarm_min_temp += 1; break;
        }
        alarm_max_temp = alarm_max_temp % 100;
        alarm_min_temp = alarm_min_temp % 100;
        sprintf (buffer, "MaxTemp:%02d,MinTemp:%02d,", alarm_max_temp, alarm_min_temp);
        write_text (buffer, 0, 0);
        break;

    case 2:
        switch (*index) {
        case 8: alarm_max_temp -= 10; break;
        case 9: alarm_max_temp -= 1; break;
        case 19: alarm_min_temp -= 10; break;
        case 20: alarm_min_temp -= 1; break;
        }
        alarm_max_temp = alarm_max_temp % 100;
        alarm_min_temp = alarm_min_temp % 100;
        sprintf (buffer, "MaxTemp:%02d,MinTemp:%02d,", alarm_max_temp, alarm_min_temp);
        write_text (buffer, 0, 0);
        break;


    case 12:
        *index = 0;
        Init_Start_Menu_UI ();
        break;
    }
}

void Popup_UI (int* index, int key_press) {
    
  if(pop_up_timer < 0){
    if(written_popUI){
      clear_screen();
      written_popUI = !written_popUI;
    }
    else{
      write_text ("ALARM HAS GONE OFF. CHECK THE TEMPERATURE. PRESS # To Go Back", 0, 0);
      written_popUI = !written_popUI;
    }
    pop_up_timer = pop_up_time;
  }
  
    if (key_press == 12) {
        switch (after_popup_state) {
        case State_Alarm_Config: Init_Alarm_Config (); break;
        case State_Light: Init_Light_UI (index); break;
        case State_TempMenu: Init_Temp_UI (); break;
        case State_Configuration:
            *index = 1;
            Init_Date_UI (*index);
            break;

        default:
            *index = 0;
            Init_Start_Menu_UI ();
            break;
        }
    }
}

void Fast_Mode_UI (int key_pressed) {
    if (key_pressed == 12) {
        Init_Start_Menu_UI ();
    }
}

void Light_UI (int key_press) {
    if (_delay_ui_rewrite < 0) {
        _delay_ui_rewrite = 3000;
        clear_screen ();
        Init_Light_UI (0);
    }
    if (key_press == 12) {
        Init_Start_Menu_UI ();
    }
}

void Init_Start_Menu_UI () {
    State_Machine = State_StartMenu;
    clear_screen ();

    write_text ("->Date Config", 0, 0);
    write_text ("Temp", 1, 2);
    write_text ("Alarm Config", 2, 2);
    write_text ("FastMode", 3, 2);
    write_text ("Light", 4, 2);
}

void Init_Temp_UI () {
    State_Machine = State_TempMenu; 
    clear_screen ();


    char* day;
    unsigned int i = 0;
    int row        = 0;
    day            = get_day (i);
    char* c        = malloc (sizeof (20));
    while (day != NULL) {

        write_text ("Day", row, 2);
        sprintf (c, " %d", i);
        write_text (c, row, 5);
        row += 1;
        i++;
        free (day);
        day = get_day (i);
    }
    temp_options = i;
    write_text ("->", 0, 0);
}

void Init_Change_Date () {
    clear_screen ();
    change_date_mode = true;
    date_UI          = get_date ();
    char* d          = Date_Stamp ();
    char* c          = Clock_Stamp ();
    write_text ("^", 1, 0);


    char buffer[100];
    sprintf (buffer, "%s %s", d, c);
    write_text (buffer, 0, 0);

    free (d);
    free (c);
    d = 0;
    c = 0;
}

void Init_Date_UI (int* index) {
    *index        = 1;
    State_Machine = State_Configuration;
    clear_screen ();
    write_text ("->", 1, 0);
    char buffer[100];
    char* d = Date_Stamp ();
    char* c = Clock_Stamp ();
    sprintf (buffer, "%s %s", d, c);
    write_text (buffer, 0, 0);
    write_text ("Change Date", 1, 2);
    write_text ("Back", 2, 2);

    free (d);
    free (c);

    d = 0;
    c = 0;
}

void Init_Alarm_Config () {
    State_Machine = State_Alarm_Config;
    clear_screen ();

    char buffer[50] = { 0 };
    sprintf (buffer, "MaxTemp:%02d,MinTemp:%02d,", alarm_max_temp, alarm_min_temp);
    write_text (buffer, 0, 0);
    write_text ("^", 1, 8);
}

void Init_Fast_Mode () {

    clear_screen ();
    State_Machine = State_FastMode;
    fastMode      = !fastMode;

    if (fastMode) {
        write_text ("Fast Mode is enabled", 0, 0);
    } else {
        write_text ("Fast Mode is disabled", 0, 0);
    }

    toggle_fast_mode (fastMode);
    write_text ("Press # to go back to start menu", 1, 0);
}

void Init_Light_UI (int* index) {
    clear_screen ();
    (*index)      = 0;
    State_Machine = State_Light;
    char* buffer  = Light_Info ();


    write_text (buffer, 0, 0);

    free (buffer);
}
void Init_Alarm_Popup (int* index) {
    *index            = 0;
    after_popup_state = State_Machine;
    State_Machine = State_Popup;
    clear_screen ();
    write_text ("ALARM HAS GONE OFF. CHECK THE TEMPERATURE. PRESS # To Go Back", 0, 0);
}