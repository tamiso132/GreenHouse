#pragma once
#include <stdbool.h>
#include <stdint.h>

void temp_ourInit ();
void Temp_Update ();
void Toggle_Fast_Mode_Temp (bool toggle);
bool is_ready_temp ();
void Clear_List ();
void Reset_Temp();

extern int temp_delay;
extern bool temp_flag;
extern int alarm_delay;
extern int temp_measure_delay;

extern unsigned char alarm_max_temp;
extern unsigned char alarm_min_temp;


// Linked List
void Ready_To_Insert_Temp ();
char* get_day (unsigned int index);
bool Is_Alarm_On ();


struct Day {
    char* date_stamp;

    int8_t max_temp;
    char* max_clock_stamp;

    int8_t min_temp;
    char* min_clock_stamp;

    int total;
    unsigned int minute_index;

    struct Day* next;

} typedef Day;