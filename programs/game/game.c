
#include <stdio.h>
#include "raylib.h"

#include "../include/interface.h"
#include "../include/state.h"

State state;

int main() {

    interface_init();
    state = state_create(); 

	struct key_state keys = { false, false, false, false, false, false, false };
	
	state_info(state)->spaceship->position.x = (SCREEN_WIDTH - 120) / 2;
	state_info(state)->spaceship->position.y = (SCREEN_HEIGHT - 120) / 2;

	while (!WindowShouldClose()) {
			// Update key states
			keys.up = IsKeyDown(KEY_UP);
			keys.left = IsKeyDown(KEY_LEFT);
			keys.right = IsKeyDown(KEY_RIGHT);
			keys.space = IsKeyDown(KEY_SPACE);
			keys.p = IsKeyDown(KEY_P);
			keys.n = IsKeyDown(KEY_N);

			state_update(state, &keys);
			interface_draw_frame(state);

			// Debug output for key states
			printf("Keys pressed: Up(%d) Left(%d) Right(%d) Space(%d) P(%d) N(%d)\n",
				keys.up, keys.left, keys.right, keys.space, keys.p, keys.n);

			printf("Location of spaceship: %f,%f\n"
				, state_info(state)->spaceship->position.x
				, state_info(state)->spaceship->position.y);

    }

    state_destroy(state); 
    interface_close();
    return 0;
}

