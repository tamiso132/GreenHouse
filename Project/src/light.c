#include "Headers/light.h"
#include "../at91sam3x8.h"
#include "../system_sam3x.h"
#include "Headers/Date.h"
#include "Headers/pio.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



// channel values
int channel_one;
int channel_two;
const Pin led_pin = (Pin){ .bit_number = 15, .controller_port = PIO_PIOA };

// Configuration constants
#define PWM_FREQUENCY 50       // Hz
#define PWM_RESOLUTION 8       // bits
#define PWM_DUTY_CYCLE_MIN 0.6 // ms
#define PWM_DUTY_CYCLE_MAX 2.4 // ms


void Init_PWM_Servo (void);
void Servo_Set_Angle (int angle);
void Light_Reset ();

unsigned int minutes_till_nextday;

unsigned int light_today_m = 0;
unsigned int dark_today_m = 0;
bool shader_on          = false;
int measure_light_delay = 60000;

int next_measure = 60000;
bool led_on      = false;
bool angle_init  = false;
int led_degree   = 0;

int Sun_Degrees ();

uint8_t sun_pos = 0;

Date current_date;

void light_update(double lighto);

void Delay(int val){
  for(int i = 0; i < val; i++)
    asm("nop");
}
char* Light_Info () {
    // sun position
    // number of hour in darkness (in hours)
    // number of hour in light (in hours)
    char* buffer = malloc (sizeof (char) * 200);

    int m_dark = dark_today_m % 60;
    int h_dark = (dark_today_m - m_dark) / 60;

    int m_light = light_today_m % 60;
    int h_light = (light_today_m - m_light) / 60;


    sprintf (buffer,"Darkness: %dh and %02dmin \nLightness: %dh and %02dmin\nSun Position: %d\nLights: %s \nShaders: %s",
             h_dark, m_dark, h_light, m_light,  sun_pos, led_on ? "On" : "Off", shader_on ? "On" : "Off");

    return buffer;
}


bool Light_Measure_Ready () {
    return measure_light_delay >= next_measure;
}

void Light_Toggle_Fastmode (bool toggle) {
    if (toggle)
        next_measure = 40;
    else
        next_measure = 60000;
}


void ourInit(void){
  
    *AT91C_PMC_PCER1 = 1<<5;
    *AT91C_ADCC_MR = 1<<10; // ADC Control Register
    current_date = get_date();
   //*AT91C_ADCC_CHER = 1<<0; 
    *AT91C_ADCC_CHER = 1<<2; // channel two 
 // *AT91C_ADCC_MR = 2<<8;
 
  Init_PWM_Servo();
}

void Light_Measure(){

  *AT91C_ADCC_CR = 1<<1;  // begins conversion
 
  while(~(*AT91C_ADCC_SR & (1<<24)) == (1<<24)){  // IT WAITS UNTIL ADC DRDY IS READY 
  }
  double readylight = (*AT91C_ADCC_LCDR);
  light_update(readylight);
  
}

double get_light_measure(){

  *AT91C_ADCC_CR = 1<<1;  // ADC control register
  while(~(*AT91C_ADCC_SR & (1<<24)) == (1<<24)){  // IT WAITS UNTIL ADC DRDY IS READY 
  }
  double readylight = (*AT91C_ADCC_LCDR);
  return readylight;
  
}


void Init_PWM_Servo (void) {

   *AT91C_PMC_PCER = 1<<12; // enable 
  *AT91C_PMC_PCER1 = 1<<4; // enable pwm
  *AT91C_PIOB_ABMR = 1<<17; //multi-driver enable bit, makes the servo faster
  *AT91C_PIOB_PDR = 1<<17; // disable pio control of pin
  *AT91C_PIOB_PPUDR = 1<<17;
  *AT91C_PWMC_ENA = 1<<1; // enable pwm controller
  *AT91C_PWMC_CH1_CMR = 5<<0; // sets channel to ch1
  *AT91C_PWMC_CH1_CPRDR = 52500; // sets channel period
  *AT91C_PWMC_CH1_CDTYR = 1800; // initial duty cycle
}
//-----------------------------------------------------------------------------------------------------
void Servo_Set_Angle (int angle) {

  *AT91C_PWMC_CH1_CDTYR = 1800 + angle*25;
}

void Reset_light(){
  led_on = false;
  shader_on = false;
  light_today_m = 0;
  dark_today_m = 0;
  current_date = get_date();
}

void light_update (double lighto) {
    Date now = get_date();
    int minutes_gone_by = Difference(current_date, now);
   
    
    if(now.day != current_date.day) // reset
    {
      dark_today_m = 0;
      light_today_m = 0;
      shader_on = false;
      minutes_gone_by = now.minute + now.hour * 60;
      pio_toggle_PIN(led_pin, 0);
      led_on = false;
      current_date = get_date();
      minutes_gone_by = now.minute + now.hour * 60;
      return;
    }

    if (shader_on) {
        Servo_Set_Angle(sun_pos);
    }
    if(lighto > 3000 && !led_on || shader_on)
    {
       dark_today_m += minutes_gone_by;
      
    }
    else{
     light_today_m += minutes_gone_by;
    }
    
    if(light_today_m > 16*60){
      shader_on = true;
    }
    
    if(16*60 - light_today_m >= (24-now.hour)*60 + now.minute && lighto > 3000){
      pio_toggle_PIN(led_pin, 1);
      led_on = true;
      shader_on = false;
    }
    current_date = now;
    
}

int Sun_Degrees () {
   int m_pos = 3300;
  int s_pos = 0;
  for(int i = 18; i >=0; i--){
    Delay(3500000);
    Servo_Set_Angle(i * 10);
    int light = get_light_measure();
    if(light < m_pos){
      m_pos = light; // most light position
      s_pos = (i * 10);
    } 
  }
  sun_pos = s_pos;
  return s_pos;

}
