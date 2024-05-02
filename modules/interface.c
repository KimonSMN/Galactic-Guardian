#include "raylib.h"

#include "../include/state.h"
#include "../include/interface.h"


// Assets
Texture spaceship_img;


// Αρχικοποιεί το interface του παιχνιδιού
void interface_init(){
    // Αρχικοποίηση του παραθύρου
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "asteroids");
	SetTargetFPS(60);
    InitAudioDevice();

	// Φόρτωση εικόνων
	spaceship_img = LoadTextureFromImage(LoadImage("assets/spaceship.png"));
}

// Κλείνει το interface του παιχνιδιού
void interface_close(){
	CloseAudioDevice();
	CloseWindow();
}

// Σχεδιάζει ένα frame με την τωρινή κατάσταση του παιχνδιού
void interface_draw_frame(State state) {
	BeginDrawing();
	ClearBackground(BLACK);
	



    DrawTexture(spaceship_img, state_info(state)->spaceship->position.x, 
				state_info(state)->spaceship->position.y, WHITE);

	// Σχεδιάζουμε το σκορ και το FPS counter
	DrawText(TextFormat("%04i", state_info(state)->score), 780, 20, 40, WHITE);
	DrawFPS(0, 0);

	EndDrawing();

}