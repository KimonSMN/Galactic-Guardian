#include "raylib.h"
#include <math.h>

#include "state.h"
#include "interface.h"

#define PICKUP_SIZE 32
#define PICKUP_COUNT 15
#define PICKUP_TIME 4.0

// Assets
Texture spaceship_img;
Texture asteroid_img;
Texture bullet_img;
Texture2D atlas;
//Texture background_img;

int pickupIndex = 0;
float pickupTimer = PICKUP_TIME;
// Αρχικοποιεί το interface του παιχνιδιού

void interface_init(){
    // Initialize the window
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "asteroids");
	SetTargetFPS(60);
    InitAudioDevice();
	
	// Load images

    atlas = LoadTextureFromImage(LoadImage("assets/rocket_pickup.png"));

	spaceship_img = LoadTextureFromImage(LoadImage("assets/spaceship.png"));
    asteroid_img = LoadTextureFromImage(LoadImage("assets/asteroid.png"));
    bullet_img = LoadTextureFromImage(LoadImage("assets/bullet.png"));
    spaceship_img.height = spaceship_img.height * 3;
    spaceship_img.width = spaceship_img.width * 3;

}

// Κλείνει το interface του παιχνιδιού
void interface_close(){
	CloseAudioDevice();
	CloseWindow();
}

// Σχεδιάζει ένα frame με την τωρινή κατάσταση του παιχνδιού
void interface_draw_frame(State state) {


    pickupTimer --;
    if (pickupTimer < 0){
        pickupTimer = PICKUP_TIME;
        pickupIndex++;
        if(pickupIndex >= PICKUP_COUNT){
            pickupIndex = 0;
        }
    }

    BeginDrawing();
    ClearBackground(BLACK);

    Rectangle source = (Rectangle){PICKUP_SIZE * pickupIndex,0,PICKUP_SIZE,PICKUP_SIZE};
    Rectangle dest = (Rectangle){SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, source.width,source.height};
    DrawTexturePro(atlas, source, dest, (Vector2){0, 0}, 0, WHITE);

    
    EndMode2D();

    // Draw the score and the FPS counter
    DrawText(TextFormat("%04i", state_info(state)->score), 780, 20, 40, WHITE);
    DrawFPS(0, 0);

    EndDrawing();
}
