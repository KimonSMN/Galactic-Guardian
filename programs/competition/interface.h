#pragma once

#include "state.h"

// Αρχικοποιεί το interface του παιχνιδιού
void interface_init();

// Αρχικοποιεί το start menu του παιχνιδιού
void interface_draw_menu();

// Κλείνει το interface του παιχνιδιού
void interface_close();

// Σχεδιάζει ένα frame με την τωρινή κατάσταση του παιχνδιού
void interface_draw_frame(State state);