
#include "Headers/Date.h"
#include "Headers/main.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>


Date date;
#define ASCII_CONVERTER 48

unsigned char month (unsigned char month);


bool fast_date_on = 0;

unsigned int fast_ticks_date = 0;


void Toggle_Fastmode_Date (bool toggle) {
   fast_date_on = toggle;
}

int Time_Into_Next_Day () {
    char* clock = Clock_Stamp ();
    int hour    = atoi (&clock[0]);
    int minute  = atoi (&clock[3]);
    int minutes = minute * (hour * 60);
    return (1440 - minutes);
};


Date Set_Date_Bounds (Date* date) {
    (*date).month  = ((*date).month + 13) % 13;
    int days       = month ((*date).month);
    (*date).day    = (days + 1 + (*date).day) % (days + 1);
    (*date).year   = ((*date).year + 10000) % 10000;
    (*date).second = ((*date).second + 60) % 60;
    (*date).minute = ((*date).minute + 60) % 60;
    (*date).hour   = ((*date).hour + 24) % 24;

    return *date;
}

void Init_Date () {
    time_t t;
    time (&t);

    struct tm* info;

    info = localtime (&t);

    date.day   = (*info).tm_mday;
    date.month = (*info).tm_mon;
    date.year  = ((*info).tm_year + 1900);


    date.second = (*info).tm_sec;
    date.minute = (*info).tm_min;
    date.hour   = (*info).tm_hour;
}


void set_time (struct Date new_date) {
    date.day   = new_date.day;
    date.month = new_date.month;
    date.year  = new_date.year;

    date.second = new_date.second;
    date.minute = new_date.minute;

    date.hour = new_date.hour;
}
Date get_date () {
    char* d_char = Date_Stamp ();
    //char *d_char = "01/04/2023";
    char* c_char = Clock_Stamp ();
    Date _date;
    char result[2];
    
   
    
    _date.year = atoi(&d_char[6]);
    _date.month = atoi(strncpy(result, &d_char[3], 2));
    _date.day   = atoi(strncpy(result, d_char, 2));

    _date.hour   = atoi(strncpy(result, c_char, 2));
    _date.minute = atoi(strncpy(result, &c_char[3], 2));
    _date.second = atoi(strncpy(result, &c_char[6], 2));

    free (d_char);
    free (c_char);
    return _date;
}
unsigned char month (unsigned char month) {
    unsigned char d = 0;
    switch (month) {

    case 1: d = 31; break;
    case 2: d = 28; break;
    case 3: d = 31; break;
    case 4: d = 30; break;
    case 5: d = 31; break;
    case 6: d = 30; break;
    case 7: d = 31; break;
    case 8: d = 31; break;
    case 9: d = 30; break;
    case 10: d = 31; break;
    case 11: d = 30; break;
    case 12: d = 31; break;

    default: break;
    }
    return d;
}
int64_t calculate_total_time(Date d){

 int64_t one_amount = d.year * 365 * 24 * 60 * 60 + 
      d.day * 24 * 60 * 60 
        + d.hour * 60 * 60 + 
          d.minute * 60 + d.second;
    
    int m = d.month -1;
    while (m > 0){
      int day = month(m);
      one_amount += day*24*60*60; 
      m--;
    }
    return one_amount/60;
}

/// return the current time stamp
/// first 10 are the date
/// last 10 are for the clock
int Difference(Date one, Date two) {
  
    int t = calculate_total_time(one) - calculate_total_time(two);
    if(t < 0)
        t *=-1;
    return t;
    
}
char* Clock_Stamp () {

    

    int seconds    = date.second + ((systickCounter-fast_ticks_date) / 1000) + (fast_ticks_date * 1.8);
    uint8_t second = seconds % 60;

    int minutes    = date.minute + ((seconds - second) / 60);
    uint8_t minute = minutes % 60;

    int hours    = date.hour + ((minutes - minute) / 60);
    uint8_t hour = hours % 24;


    char* c = malloc (sizeof (char) * 30);


    sprintf (c, "%02d:%02d:%02d", hour, minute, second);
    c[11] = '\0';
    return c; // don't forget to free
}
char* Date_Stamp () {

  
    int seconds    = date.second + ((systickCounter-fast_ticks_date) / 1000) + (fast_ticks_date * 1.8);
    uint8_t second = seconds % 60;

    int minutes          = date.minute + ((seconds - second) / 60);
    uint8_t minute = minutes % 60;

    int hours          = date.hour + ((minutes - minute) / 60);
    uint8_t hour = hours % 24;

    int days                    = date.day + ((hours - hour) / 24);
    uint8_t days_of_month = month (date.month);
    
    
    int d = date.day;
    int m = date.month;
    int y = date.year;
    
    int temp_day = date.day;
    
    
    
    while(days > 0){
      int dtemp = days;
      days = days - month(m);
      if(days > 0){
      
        m +=1;
        if(m > 12){
          y++;
          m %= 13;
          m+=1;
        }
        
      }
      else{
        d = dtemp;
      }
    
    }


    char* c = malloc (sizeof (char) * 20);

    sprintf (c, "%02d/%02d/%d", d, m, y);
    return c; // remember to free memory
}


