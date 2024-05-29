#pragma once
#include <stdbool.h>

void ourInit ();

bool Light_Measure_Ready ();
void Light_Measure ();
void Light_Toggle_Fastmode (bool toggle);
void Reset_light();

// UI
char* Light_Info ();

extern int measure_light_delay;

int Sun_Degrees ();