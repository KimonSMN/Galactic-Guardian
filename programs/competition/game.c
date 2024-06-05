
#include <stdio.h>
#include "raylib.h"
#include "interface.h"
#include "state.h"

State state;
GameState gameState = {true, false, false, false, false};
MenuButton button = {true, false, false, 1};

void UpdateMenu() {
    if (IsKeyPressed(KEY_DOWN)) {
        button.counter++;
        if (button.counter > 3) 
            button.counter = 1;
    } else if (IsKeyPressed(KEY_UP)) {
        button.counter--;
        if (button.counter < 1) 
            button.counter = 3;
    }

    button.start = (button.counter == 1); // assigns either true or false
    button.info = (button.counter == 2);
    button.exit = (button.counter == 3);
    
    if (IsKeyPressed(KEY_ENTER)) {
        if (button.start) {
            gameState.start_menu = false;
            gameState.introduction = true;
        } else if (button.info) {
            gameState.start_menu = false;
            gameState.info_menu = true;
        } else if (button.exit) {
            gameState.start_menu = false;
            gameState.game_over = true;
        }
    }
}

 int main() {
    interface_init();
    InitAudioDevice(); 
    state = state_create(); 

    struct key_state keys = { false, false, false, false, false, false, false };
    
    while (!WindowShouldClose()) {    
        if (gameState.start_menu) {
            UpdateMenu();
			interface_fade_in();
            interface_draw_menu();
        } else if (gameState.gameplay) {

            keys.up = IsKeyDown(KEY_UP);
            keys.left = IsKeyDown(KEY_LEFT);
            keys.right = IsKeyDown(KEY_RIGHT);
            keys.space = IsKeyDown(KEY_SPACE);
            keys.p = IsKeyDown(KEY_P);
            keys.n = IsKeyDown(KEY_N);
            keys.s = IsKeyDown(KEY_S);
            keys.q = IsKeyDown(KEY_Q);
            keys.w = IsKeyDown(KEY_W);
            keys.e = IsKeyDown(KEY_E);

            state_update(state, &keys);
            interface_draw_frame(state);
            // if hearts <= 0
            if (state_info(state)->lost) 
                break;
				
        } else if (gameState.info_menu){
            interface_draw_info(state);
            if (IsKeyPressed(KEY_B)) {
                gameState.info_menu = false;
                gameState.start_menu = true;
             }
   
            
        } else if (gameState.game_over) {
			break;

		} else if (gameState.introduction){
            interface_draw_intro(state, &gameState);
        }
    }

    state_destroy(state); 
    interface_close();
    return 0;
}


