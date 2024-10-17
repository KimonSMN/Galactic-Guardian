#pragma once

#include "state.h"

extern Music background_music;
extern Music intro_music;


// Αρχικοποιεί το interface του παιχνιδιού
void interface_init();

// Αρχικοποιεί το start menu του παιχνιδιού
void interface_draw_menu();

// Αρχικοποιεί το info menu του παιχνιδιού
void interface_draw_info();

void play_sound();

void interface_draw_intro();

void interface_fade_in();

void interface_draw_lost();

void interface_draw_win();

// Κλείνει το interface του παιχνιδιού
void interface_close();

// Σχεδιάζει ένα frame με την τωρινή κατάσταση του παιχνδιού
void interface_draw_frame(State state);