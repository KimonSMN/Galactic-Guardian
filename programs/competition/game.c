
#include <stdio.h>
#include "raylib.h"

#include "interface.h"
#include "state.h"

State state;
GameState gameState = START_MENU;
MenuButton button = {true, false, false, 1};


void UpdateMenu() {
    if (IsKeyPressed(KEY_DOWN)) {
        button.counter++;
        if (button.counter > 3) button.counter = 1;
    } else if (IsKeyPressed(KEY_UP)) {
        button.counter--;
        if (button.counter < 1) button.counter = 3;
    }

    button.start = (button.counter == 1); // assigns either true or false
    button.info = (button.counter == 2);
    button.exit = (button.counter == 3);
    
    if (IsKeyPressed(KEY_ENTER)) {
        if (button.start) {
            gameState = GAMEPLAY;
        } else if (button.info) {
            gameState = INFO_MENU;
        } else if (button.exit) {
            gameState = GAME_OVER; 
        }
    }
}

int main() {
    interface_init();
    InitAudioDevice(); 
    state = state_create(); 

    struct key_state keys = { false, false, false, false, false, false, false };
    
    while (!WindowShouldClose()) {    
        if (gameState == START_MENU) {
            UpdateMenu();
			interface_fade_in();
            interface_draw_menu();
        } else if (gameState == GAMEPLAY) {

            keys.up = IsKeyDown(KEY_UP);
            keys.left = IsKeyDown(KEY_LEFT);
            keys.right = IsKeyDown(KEY_RIGHT);
            keys.space = IsKeyDown(KEY_SPACE);
            keys.p = IsKeyDown(KEY_P);
            keys.n = IsKeyDown(KEY_N);

            state_update(state, &keys);
            interface_draw_frame(state);

            // if hearts <= 0
            if (state_info(state)->lost) 
                gameState = GAME_OVER;
				
        } else if (gameState == INFO_MENU){
            interface_draw_info(state);
            
        } else if (gameState == GAME_OVER) {
			break;
		}
    }

    state_destroy(state); 
    interface_close();
    return 0;
}


