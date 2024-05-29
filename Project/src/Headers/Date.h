#pragma once
#include <stdbool.h>
#include <stdint.h>

struct Date {
    uint8_t day, month;
    uint16_t year;

    uint8_t second, minute, hour;

} typedef Date;


extern bool fast_date_on;
extern unsigned int fast_ticks_date;

void Toggle_Fastmode_Date(bool toggle);
void Init_Date ();
int Time_Into_Next_Day ();
struct Date get_date ();
void set_time (struct Date new_date);
bool Date_Ready ();
char* Date_Stamp ();
char* Clock_Stamp ();
int Difference(Date one, Date two);
Date Set_Date_Bounds (Date* date);
