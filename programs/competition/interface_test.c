#include "raylib.h"
#include <math.h>

#include "state.h"
#include "interface.h"

#define PICKUP_SIZE 32
#define PICKUP_COUNT 15

#define EXPLOSION_SIZE 96
#define EXPLOSION_COUNT 4
#define EXPLOSION_TIME 4.0

// Assets
Texture spaceship_img;
Texture asteroid_img;
Texture bullet_img;
Texture2D atlas;
//Texture background_img;

int explosionIndex = 0;
float explosionTimer = PICKUP_TIME;
// Αρχικοποιεί το interface του παιχνιδιού

void interface_draw_menu() {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("Press ENTER to Start", SCREEN_WIDTH / 2 - MeasureText("Press ENTER to Start", 20) / 2, SCREEN_HEIGHT / 2 - 10, 20, DARKGRAY);
    EndDrawing();
}

void interface_init(){
    // Initialize the window
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "asteroids");
	SetTargetFPS(60);
    InitAudioDevice();
	
	// Load images

    atlas = LoadTextureFromImage(LoadImage("assets/explosion.png"));

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


void draw_explosion(State state){
    
    explosionTimer --;
    if (explosionTimer < 0){
        explosionTimer = EXPLOSION_TIME;
        explosionIndex++;
        if(explosionIndex >= EXPLOSION_COUNT){
            explosionIndex = 0;
        }
    }

    BeginDrawing();
    ClearBackground(BLACK);

    Rectangle source = (Rectangle){EXPLOSION_SIZE * explosionIndex,0,EXPLOSION_SIZE,EXPLOSION_SIZE};
    Rectangle dest = (Rectangle){SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, source.width,source.height};
    DrawTexturePro(atlas, source, dest, (Vector2){0, 0}, 0, WHITE);

    
    EndMode2D();

    // Draw the score and the FPS counter
    DrawText(TextFormat("%04i", state_info(state)->score), 780, 20, 40, WHITE);
    DrawFPS(0, 0);

    EndDrawing();
}
int counter = 4;
// Σχεδιάζει ένα frame με την τωρινή κατάσταση του παιχνδιού
void interface_draw_frame(State state) {

    if(IsKeyPressed(KEY_A))
        counter--;

    if(counter!= 0){
        draw_explosion(state);

    }
}
