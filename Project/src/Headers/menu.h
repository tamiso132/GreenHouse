#pragma once
enum StateMachine_UI {
    State_StartMenu,
    State_Configuration,
    State_TempMenu,
    State_Alarm_Config,
    State_FastMode,
    State_Light,
    State_Popup,
} typedef StateMachine_UI;

extern StateMachine_UI State_Machine;


void Init_Start_Menu_UI ();
void Init_Alarm_Popup (int* index);


void Date_Menu_UI (int* index, int key_pressed);
void Start_Menu_UI (int* index, int key_pressed);
void Temp_Menu_UI (int *index, int key_pressed);
void Fast_Mode_UI (int key_pressed);
void Alarm_UI (int* index, int key_pressed);
void Light_UI (int key_press);
void Popup_UI (int* index, int key_press);

StateMachine_UI get_state ();

extern int _delay_ui_rewrite;
extern float pop_up_timer;