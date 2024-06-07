#include <stdio.h>
#include "raylib.h"
#include "interface.h"
#include "state.h"

State state;
GameState gameState = {true, false, false, false, false, false, false};
MenuButton button = {true, false, false, 1};


void ResetGame() {
    state_destroy(state);     // Destroy the current state
    state = state_create();   // Create a new state

    // Reset button states
    button.start = true;
    button.info = false;
    button.exit = false;
    button.counter = 1;

    // Reset game state variables
    gameState.start_menu = false;
    gameState.gameplay = true;
    gameState.info_menu = false;
    gameState.game_over = false;
    gameState.introduction = false;
    gameState.quit = false;

    StopMusicStream(intro_music); // Restart intro music
    PlayMusicStream(background_music); // Pause background music
}

void BackToMenu() {
    state_destroy(state);     // Destroy the current state
    state = state_create();   // Create a new state

    // Reset button states
    button.start = false;
    button.info = false;
    button.exit = false;
    button.counter = 1;

    // Reset game state variables
    gameState.start_menu = true;
    gameState.gameplay = false;
    gameState.info_menu = false;
    gameState.game_over = false;
    gameState.introduction = false;

    StopMusicStream(intro_music); // Restart intro music
    StopMusicStream(background_music); // Pause background music
}

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
            StopMusicStream(intro_music);
            PlayMusicStream(background_music);
        } else if (button.info) {
            gameState.start_menu = false;
            gameState.info_menu = true;
        } else if (button.exit) {
            gameState.start_menu = false;
            gameState.quit = true;
        }
    }
}

int main() {
    interface_init();
    state = state_create(); 

    struct key_state keys = { false, false, false, false, false, false, false };

    PlayMusicStream(intro_music); // Start playing intro music

    while (!WindowShouldClose()) {   
        UpdateMusicStream(intro_music);
        UpdateMusicStream(background_music);

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

            if (state_info(state)->lost) {
                gameState.gameplay = false;
                gameState.game_over = true;
            }
                
            if(state_info(state)->game_won){
                gameState.gameplay = false;
                gameState.game_won = true;
            }
        
        } else if (gameState.info_menu){
            interface_draw_info(state);
            if (IsKeyPressed(KEY_B)) {
                gameState.info_menu = false;
                gameState.start_menu = true;
            }
        } else if (gameState.game_over) {
            interface_draw_lost(state);
            StopMusicStream(background_music);
            StopMusicStream(intro_music);
            if (IsKeyPressed(KEY_ENTER)){
                ResetGame(); 
            }
            else if(IsKeyPressed(KEY_B)){
                BackToMenu();
            }
        } else if (gameState.introduction){
            interface_draw_intro(state, &gameState);
        } else if (gameState.game_won) {
            interface_draw_win(state);

            if(IsKeyPressed(KEY_B))
                BackToMenu();
            
        } else if(gameState.quit){
            break;
        }
    }

    state_destroy(state); 
    interface_close();
    return 0;
}
