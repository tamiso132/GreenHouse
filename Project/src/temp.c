#include "Headers/temp.h"
#include "../at91sam3x8.h"
#include "../system_sam3x.h"
#include "Headers/Date.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void remove_first ();
void Toggle_Fast_Mode_Temp (bool toggle);

// externs
int temp_delay         = 0; // used as a delay in reset
bool temp_flag         = 0; // ready to read
int temp_measure_delay = 60000; //  timer
int next_measure_time  = 60000; // time for next measure
// Alarm
unsigned char alarm_max_temp = 30; // temp max for alarm
unsigned char alarm_min_temp = 15; // temp min for alarm

int alarm_delay = 0; // ui popui delay, make it so it dosent spam 
// non_externs
Day** start; // start day
Day* current; // current day
Date current_date_yop; // date for recent measure. used for knowing if a new day has gone by
uint8_t day_counter = 0; // current index



unsigned short int amount_of_remove;
unsigned short int Amount_Of_Remove () {
    return amount_of_remove;
}


// For Temp Calculating
void measure (void);
bool is_ready_temp ();
void Temp_Update ();

// For LinkedList
void insert_temp_last ();
void Init_List ();
// for calculating temp methods

int recent_temp = 0;

bool Is_Alarm_On () {
    if (alarm_delay > 0)
        return false;

    return recent_temp > alarm_max_temp || recent_temp < alarm_min_temp;
}


void reset () { // reset temp to get it accurate
    *AT91C_PIOB_OER = (1 << 25); // Enable output

    *AT91C_PIOB_CODR = (1 << 25); // Set pin to low

    temp_delay = 1000;
    while (temp_delay >= 0) {
    }

    *AT91C_PIOB_SODR = (1 << 25); // Set pin to high
}

void Temp_Update () {
    reset ();
    measure ();
}

void Clear_List () {
    while (*start != NULL) {
        remove_first ();
    }

    Init_List ();
}

void Reset_Temp(){
  Clear_List();
  temp_measure_delay = 60000;
  temp_flag = 1;
}

void Init_List () {


    current                  = malloc (sizeof (Day));
    current->date_stamp      = Date_Stamp ();
    current->max_clock_stamp = Clock_Stamp ();
    current->min_clock_stamp = Clock_Stamp ();
    current->max_temp        = -50;
    current->min_temp        = 50;
    current->next            = 0;
    current->minute_index    = 0;
    current->total           = 0;
    *start                   = current;

   
    current_date_yop = get_date();
}
void temp_ourInit () {

    start = malloc (sizeof (Day));
    Init_List ();
    *AT91C_PMC_PCER = PMC_PCER0_PID12; // Enable clock PIO B
    *AT91C_PMC_PCER = PMC_PCER0_PID27; // Enable clock TC0
    *AT91C_TC0_CMR  = (*AT91C_TC0_CMR & 0xfff8) | (1 << 18) |
    (1 << 17); // Set TC_CMR0 TCCLKS to 0, Timer_Clock1

    *AT91C_PIOB_PER   = (1 << 25); // Enable pin 2, temperature
    *AT91C_PIOB_OER   = (1 << 25); // Enable output
    *AT91C_PIOB_PPUDR = (1 << 25); // Disable pull-up

    *AT91C_TC0_CCR |= 1;        // Enable clock
    *AT91C_TC0_CCR |= (1 << 2); // Clock software reset

    reset ();

    NVIC_ClearPendingIRQ (TC0_IRQn);
    NVIC_SetPriority (TC0_IRQn, 0);
    NVIC_EnableIRQ (TC0_IRQn);
    
    current_date_yop = get_date();
    
    reset ();
    measure ();
}

void Toggle_Fast_Mode_Temp (bool toggle) {
    if (toggle) {
        next_measure_time = 40;
    } else {
        next_measure_time = 60000;
    }
}


void measure (void) {
    *AT91C_PIOB_OER  = (1 << 25); // Enable output
    *AT91C_PIOB_SODR = (1 << 25); // Set pin to high

    *AT91C_PIOB_CODR = (1 << 25); // Set pin to low
    *AT91C_PIOB_SODR = (1 << 25); // Set pin to high

    *AT91C_PIOB_ODR = (1 << 25); // Enable input

    *AT91C_TC0_CCR = (1 << 2); // Clock software reset
    *AT91C_TC0_SR;             // Read value to clear old events
    *AT91C_TC0_IER = (1 << 6); // Enable RB load interrupt
}


bool is_ready_temp () {
    return (temp_measure_delay >= next_measure_time && temp_flag == 1);
}

void TC0_Handler () {              // ready to read temperature
    *AT91C_TC0_IDR     = (1 << 6); // Disable RB load interrupt
    temp_flag          = 1;
}
// for displaying ui methods


// LinkedList methods
void Ready_To_Insert_Temp () {
    uint32_t a = (*AT91C_TC0_RA) / 42;
    uint32_t b = (*AT91C_TC0_RB) / 42;

    int temp    = (int)round (((b - a) / 5) - 273.15); // temperature
    recent_temp = temp;

 
    if (current->max_temp < temp) { // if new temperature is bigger then the current biggest
        current->max_clock_stamp = Clock_Stamp ();
        current->max_temp        = (int)temp;
    }
    if (current->min_temp > temp) {
        current->min_clock_stamp = Clock_Stamp ();
        current->min_temp        = (int)temp;
    }
    current->total += temp;
    current->minute_index++;
    if (current_date_yop.day != get_date().day) {
        current->next = malloc (sizeof (char) * 1024); // 1 kb

        while (current->next == NULL) {  // checks so there is at least 1kb memory
            remove_first (); 
            current->next = malloc(sizeof (char) * 1024);
        }
        
        while(day_counter > 7){ // no more then 7 days
          remove_first();
          day_counter--;
        }
        free(current->next);

        current->next = malloc(sizeof(Day));
        current                  = current->next;
        current->date_stamp      = Date_Stamp ();
        current->max_clock_stamp = Clock_Stamp ();
        current->min_clock_stamp = Clock_Stamp ();

        current->max_temp     = -50;
        current->min_temp     = 50;
        current->next         = 0;
        current->minute_index = 0;
        current->total = 0;
        current_date_yop = get_date(); 
    }
     
}

void remove_first () {

    Day* temp = *start;
    if (temp == NULL)
        return;
    if (temp->date_stamp != NULL) {
        free (temp->date_stamp);
    }
    if (temp->max_clock_stamp != NULL) {
        free (temp->max_clock_stamp);
    }
    if (temp->min_clock_stamp != NULL) {
        free (temp->min_clock_stamp);
    }

    *start = (*start)->next;
    free (temp);
}

char* get_day (unsigned int index) {
    Day* c = *start;
    while (index > 0) {
        c = c->next;
        index--;
        if (c == NULL)
            return NULL;
    }
    char* buffer = malloc (sizeof (unsigned char) * 500);
    int avg      = c->total / c->minute_index;


    sprintf (buffer, "Date: %s \nMaxTemp: %d,clock: %s\nMinTemp: %d,clock: %s\nAvgTemp: %d",
    c->date_stamp, c->max_temp, c->max_clock_stamp,
    c->min_temp, c->min_clock_stamp, avg);
    return buffer; // remember to free this
}
