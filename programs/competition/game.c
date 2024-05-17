
#include <stdio.h>
#include "raylib.h"

#include "interface.h"
#include "state.h"

State state;
GameState gameState = START_MENU;

void UpdateMenu() {
    if (IsKeyPressed(KEY_ENTER)) {
        gameState = GAMEPLAY;
    }
}

int main() {
    interface_init();
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
				
        } else if (gameState == GAME_OVER) {
			break;
		}
    }

    state_destroy(state); 
    interface_close();
    CloseWindow(); 
    return 0;
}


