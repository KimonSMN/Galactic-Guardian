#pragma once

#include "state.h"

// Αρχικοποιεί το interface του παιχνιδιού
void interface_init();

// Αρχικοποιεί το start menu του παιχνιδιού
void interface_draw_menu();

// Αρχικοποιεί το info menu του παιχνιδιού
void interface_draw_info();

void interface_draw_intro();


void interface_fade_in();

// Κλείνει το interface του παιχνιδιού
void interface_close();

// Σχεδιάζει ένα frame με την τωρινή κατάσταση του παιχνδιού
void interface_draw_frame(State state);