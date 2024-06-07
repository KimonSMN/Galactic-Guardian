#include "raylib.h"
#include <math.h>

#include "state.h"
#include "interface.h"
#include <string.h>
#include <stdio.h>

// Assets
Texture2D spaceship_img;
Texture2D asteroid_img;
Texture2D bullet_img;
Texture2D heart;
Texture2D explosion;
Texture2D enemy_scout;
Texture2D game_name; 
Texture2D start_button;
Texture2D info_button;
Texture2D exit_button;
Texture2D astronaut;
Texture2D skip_text_button;
Texture2D space_background;
Texture2D wave;

Texture2D shop_frame;
Texture2D shop_text;
Texture2D shop_item_frame;
Texture2D item_icon_frame;
Texture2D shop_ending_text;

Texture2D tiny_twin_desc;
Texture2D tiny_twin_image;
Texture2D tiny_twin_title;
Texture2D cost_2000;

Texture2D pew_pew_plus_desc;
Texture2D pew_pew_plus_image; // not imported
Texture2D pew_pew_plus_title;
Texture2D cost_1000;

Texture2D healaholic_desc;
Texture2D healaholic_image; // not imported
Texture2D healaholic_title;
Texture2D cost_250;

Texture2D boss;
Texture2D final_boss;
Texture2D boss_healthbar;

Texture2D coin;
Texture2D purchase_complete;
Texture2D not_enough_coins;

Texture2D info_menu;

Texture2D game_over;
Texture2D restart;
Texture2D back;

Texture2D back_won;
Texture2D congratulations;

Sound bought_item; 
Sound cant_buy; 

int purchase_complete_sound_timer = 0;
int not_enough_coins_sound_timer = 0;

Sound test;
Sound confirm;
Sound player_damaged;
Sound laser;
Sound hit;
Sound boss_damaged;
Sound boss_died;
Sound boss_roar;

Music background_music;
Music intro_music;

int coinIndex = 0;
float coinTimer = 4; // number of sprites

int heartIndex = 0;

int waveIndex = 0; 

int skipTextIndex = 0;
float skipTextTimer = 8;

int bossHealthIndex = 0;
int finalBossHealthIndex = 0;

int buttonCounter = 1;
float buttonTimer = 30;

int startButtonIndex = 0;
int infoButtonIndex = 0;
int exitButtonIndex = 0;

int purchase_popup_timer = 100;
// Start menu background color
Color menu_color = {1,0,20,0};

// Αρχικοποιεί το interface του παιχνιδιού

void interface_init(){
    // Initialize the window
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "asteroids");
    InitAudioDevice();

	SetTargetFPS(60);
    
    // Load sounds
    bought_item = LoadSound("assets/sounds/can_buy_2.wav"); 
    cant_buy = LoadSound("assets/sounds/cant_buy.wav"); 
    test = LoadSound("assets/sounds/test.wav"); 
    confirm = LoadSound("assets/sounds/confirm.wav"); 
    player_damaged= LoadSound("assets/sounds/player_damaged.wav");
    laser = LoadSound("assets/sounds/laser.mp3");
    hit = LoadSound("assets/sounds/hit.wav");
    intro_music = LoadMusicStream("assets/sounds/intro_music.ogg");
    background_music = LoadMusicStream("assets/sounds/background_music.ogg");
    boss_damaged = LoadSound("assets/sounds/boss_damaged.wav");
    boss_died = LoadSound("assets/sounds/boss_died.wav");
    boss_roar = LoadSound("assets/sounds/boss_roar.wav");

    SetSoundVolume(player_damaged,0.7);
    SetMusicVolume(background_music,0.5);
    SetMusicVolume(intro_music,0.5);

	// Load images
    game_name = LoadTextureFromImage(LoadImage("assets/images/game_name_v2.png"));

    shop_frame = LoadTextureFromImage(LoadImage("assets/images/shop_frame.png"));
    shop_text = LoadTextureFromImage(LoadImage("assets/images/shop_text.png"));
    shop_item_frame = LoadTextureFromImage(LoadImage("assets/images/shop_item_frame.png"));
    item_icon_frame = LoadTextureFromImage(LoadImage("assets/images/item_icon_frame.png"));
    shop_ending_text = LoadTextureFromImage(LoadImage("assets/images/shop_ending_text.png"));

    info_menu = LoadTextureFromImage(LoadImage("assets/images/info_menu.png"));

    tiny_twin_desc = LoadTextureFromImage(LoadImage("assets/images/tiny_twin_desc.png"));
    tiny_twin_image = LoadTextureFromImage(LoadImage("assets/images/tiny_twin_image.png"));
    tiny_twin_title = LoadTextureFromImage(LoadImage("assets/images/tiny_twin_title.png"));
    cost_2000 = LoadTextureFromImage(LoadImage("assets/images/cost_2000.png"));

    pew_pew_plus_desc = LoadTextureFromImage(LoadImage("assets/images/pew_pew_plus_desc.png"));
    pew_pew_plus_image = LoadTextureFromImage(LoadImage("assets/images/pew_pew_plus_image.png"));
    pew_pew_plus_title = LoadTextureFromImage(LoadImage("assets/images/pew_pew_plus_title.png"));
    cost_1000 = LoadTextureFromImage(LoadImage("assets/images/cost_1000.png"));

    healaholic_desc = LoadTextureFromImage(LoadImage("assets/images/healaholic_desc.png"));
    healaholic_image = LoadTextureFromImage(LoadImage("assets/images/healaholic_image.png"));
    healaholic_title = LoadTextureFromImage(LoadImage("assets/images/healaholic_title.png"));
    cost_250 = LoadTextureFromImage(LoadImage("assets/images/cost_250.png"));

    boss_healthbar = LoadTextureFromImage(LoadImage("assets/images/boss_healthbar.png"));
    boss = LoadTextureFromImage(LoadImage("assets/images/boss.png"));
    final_boss = LoadTextureFromImage(LoadImage("assets/images/final_boss.png"));

    coin = LoadTextureFromImage(LoadImage("assets/images/coin.png"));
    purchase_complete = LoadTextureFromImage(LoadImage("assets/images/purchase_complete.png"));
    not_enough_coins = LoadTextureFromImage(LoadImage("assets/images/not_enough_coins.png"));

    start_button = LoadTextureFromImage(LoadImage("assets/images/start_button.png"));
    info_button = LoadTextureFromImage(LoadImage("assets/images/info_button.png"));
    exit_button = LoadTextureFromImage(LoadImage("assets/images/exit_button.png"));
    skip_text_button = LoadTextureFromImage(LoadImage("assets/images/skip_text_button.png"));
    space_background = LoadTextureFromImage(LoadImage("assets/images/space.png"));
    wave = LoadTextureFromImage(LoadImage("assets/images/wave.png"));
    wave.height = wave.height * 0.5;
    wave.width = wave.width * 0.5;
    astronaut = LoadTextureFromImage(LoadImage("assets/images/astronaut.png"));
    astronaut.height = astronaut.height * 10;
    astronaut.width = astronaut.width * 10;
    enemy_scout = LoadTextureFromImage(LoadImage("assets/images/enemy_scout.png"));

    game_over = LoadTextureFromImage(LoadImage("assets/images/game_over.png"));
    restart = LoadTextureFromImage(LoadImage("assets/images/restart.png"));
    back = LoadTextureFromImage(LoadImage("assets/images/back_to_menu.png"));

    congratulations = LoadTextureFromImage(LoadImage("assets/images/congratulations.png"));
    back_won = LoadTextureFromImage(LoadImage("assets/images/back_won.png"));
 

    heart = LoadTextureFromImage(LoadImage("assets/images/hearts.png"));
	spaceship_img = LoadTextureFromImage(LoadImage("assets/images/spaceship.png"));
    asteroid_img = LoadTextureFromImage(LoadImage("assets/images/asteroid.png"));
    bullet_img = LoadTextureFromImage(LoadImage("assets/images/bullet.png"));
    explosion = LoadTextureFromImage(LoadImage("assets/images/explosion.png"));
    spaceship_img.height = spaceship_img.height * 3;
    spaceship_img.width = spaceship_img.width * 3;
	start_button.height = start_button.height *1.2;
    start_button.width = start_button.width *1.2;
	info_button.height = info_button.height *1.2;
    info_button.width = info_button.width *1.2;
    exit_button.height = exit_button.height *1.2;
    exit_button.width = exit_button.width *1.2;
}

void play_sound(int num) {
    switch(num) {
        case 1:
            PlaySound(laser);
            break;
        case 2:
            PlaySound(player_damaged);
            break;
        case 3:
            PlaySound(hit);
            break;
        case 4:
            PlaySound(boss_damaged);
            break;
        case 5:
            PlaySound(boss_died);
            break;
        case 6:
            PlaySound(boss_roar);
            break;
        default:
            break;
    }
}

// Κλείνει το interface του παιχνιδιού
void interface_close(){
    UnloadSound(bought_item);
    UnloadSound(cant_buy);
    UnloadSound(test);
    UnloadSound(confirm);
    UnloadSound(player_damaged);
    UnloadSound(laser);
    UnloadSound(hit);

    UnloadMusicStream(intro_music);
    UnloadMusicStream(background_music);

    CloseAudioDevice();
    CloseWindow();
}

float fadeInOpacity = 1;
bool fadingIn = true;

void interface_fade_in() {
    if (fadingIn) {
        fadeInOpacity -= 0.015;
        if (fadeInOpacity <= 0) {
            fadeInOpacity = 0;
            fadingIn = false;
        }
    }
}

// Αρχικοποιεί το start menu του παιχνιδιού
void interface_draw_menu() {

    buttonTimer --;
    if (buttonTimer < 0 && buttonCounter == 1){
        buttonTimer = 30;
        startButtonIndex++;
        if(startButtonIndex > 1){
            startButtonIndex = 0;
        }
    } else if (buttonTimer < 0 && buttonCounter == 2){
        buttonTimer = 30;
        infoButtonIndex++;
        if(infoButtonIndex > 1){
            infoButtonIndex = 0;
        }
    } else if (buttonTimer < 0 && buttonCounter == 3){
        buttonTimer = 30;
        exitButtonIndex++;
        if(exitButtonIndex > 1){
            exitButtonIndex = 0;
        }
    }

    if(IsKeyPressed(KEY_UP)){
        startButtonIndex = 0;
        infoButtonIndex = 0;
        exitButtonIndex = 0;
        buttonCounter--;
        if(buttonCounter < 1)
            buttonCounter = 3;
        else if(buttonCounter > 3)
            buttonCounter = 1;
    } else if (IsKeyPressed(KEY_DOWN)){
       
        startButtonIndex = 0;
        infoButtonIndex = 0;
        exitButtonIndex = 0;
        buttonCounter++;
        if(buttonCounter < 1)
            buttonCounter = 3;
        else if(buttonCounter > 3)
            buttonCounter = 1;
    } 
    if (IsKeyPressed(KEY_ENTER)){
        PlaySound(confirm);

    } // might not work?
    else if(IsKeyPressed(KEY_UP)){
        PlaySound(test);
    }
    else if(IsKeyPressed(KEY_DOWN)){
        PlaySound(test);
    }
    

    Vector2 gameNamePos = { SCREEN_WIDTH / 2 - game_name.width / 2, 60 };
    Vector2 startButtonPos = { SCREEN_WIDTH / 2 - start_button.width / 2, 350 };
    Vector2 infoButtonPos = { SCREEN_WIDTH / 2 - info_button.width / 2, 450 };
    Vector2 exitButtonPos = { SCREEN_WIDTH / 2 - exit_button.width / 2, 550 };

    BeginDrawing();
    ClearBackground(menu_color);
    DrawTexture(space_background, 0, 0, WHITE);

    DrawTexture(game_name, gameNamePos.x, gameNamePos.y, WHITE);

    Rectangle start_source = (Rectangle){192 * startButtonIndex ,0,192,48};
    Rectangle start_dest = (Rectangle){startButtonPos.x, startButtonPos.y, start_source.width,start_source.height};
    DrawTexturePro(start_button, start_source, start_dest, (Vector2){-95,0}, 0, WHITE);

    Rectangle info_source = (Rectangle){192 * infoButtonIndex ,0,192,48};
    Rectangle info_dest = (Rectangle){infoButtonPos.x, infoButtonPos.y, info_source.width,info_source.height};
    DrawTexturePro(info_button, info_source, info_dest, (Vector2){-95,0}, 0, WHITE);

    Rectangle exit_source = (Rectangle){192 * exitButtonIndex ,0,192,48};
    Rectangle exit_dest = (Rectangle){exitButtonPos.x, exitButtonPos.y, exit_source.width,exit_source.height};
    DrawTexturePro(exit_button, exit_source, exit_dest, (Vector2){-95,0}, 0, WHITE);

    if (fadingIn) {
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, fadeInOpacity));
    }

    EndDrawing();
}


void interface_draw_info(State state){

    BeginDrawing();
    DrawTexture(space_background, 0, 0, WHITE);

    DrawTexture(info_menu,0,0,WHITE);    

    EndDrawing();

};

void interface_draw_lost(State state) {
    BeginDrawing();
    ClearBackground(BLACK);

    int game_over_x = (SCREEN_WIDTH / 2) - (game_over.width / 2);
    int game_over_y = (SCREEN_HEIGHT / 2) - (game_over.height / 2) - restart.height - 35; 
    int restart_x = (SCREEN_WIDTH / 2) - (restart.width / 2);
    int restart_y = (SCREEN_HEIGHT / 2) - (restart.height / 2);
    int back_x = (SCREEN_WIDTH / 2) - (back.width / 2);
    int back_y = (SCREEN_HEIGHT / 2) - (back.height / 2) + restart.height + 25;


    DrawTexture(game_over, game_over_x, game_over_y, WHITE);
    DrawTexture(restart, restart_x, restart_y, WHITE);
    DrawTexture(back, back_x, back_y, WHITE);

    EndDrawing();
}

void interface_draw_win(State state) {
    BeginDrawing();
    ClearBackground(BLACK);

    int congratulations_x = (SCREEN_WIDTH / 2) - (congratulations.width / 2);
    int congratulations_y = (SCREEN_HEIGHT / 2) - (congratulations.height / 2) ; 
    int back_x = (SCREEN_WIDTH / 2) - (back_won.width / 2);
    int back_y = (SCREEN_HEIGHT / 2) - (back_won.height / 2) + back_won.height + 150;



    DrawTexture(congratulations, congratulations_x, congratulations_y, WHITE);
    DrawTexture(back_won, back_x, back_y, WHITE);

    EndDrawing();
}

static void interface_draw_shop(State state){
    BeginDrawing();

    int shop_frame_x = (SCREEN_WIDTH - shop_frame.width) / 2;
    int shop_frame_y = (SCREEN_HEIGHT - shop_frame.height) / 2 + 20;

    // Shop frame
    DrawTexture(shop_frame, shop_frame_x, shop_frame_y, WHITE);

    // Shop text "SHOP"
    int shop_text_x = shop_frame_x + (shop_frame.width - shop_text.width) / 2; // Center of Shop frame
    int shop_text_y = shop_frame_y + 25;

    DrawTexture(shop_text, shop_text_x, shop_text_y, WHITE);

    // Item frames in the shop frame
    int item_frame_x = shop_frame_x + (shop_frame.width - shop_item_frame.width) / 2;

    int item_frame_y1 = shop_text_y + shop_text.height + 25;
    int item_frame_y2 = item_frame_y1 + shop_item_frame.height + 35;
    int item_frame_y3 = item_frame_y2 + shop_item_frame.height + 35;

    DrawTexture(shop_item_frame, item_frame_x, item_frame_y1, WHITE);
    DrawTexture(shop_item_frame, item_frame_x, item_frame_y2, WHITE);
    DrawTexture(shop_item_frame, item_frame_x, item_frame_y3, WHITE);

    // complaint text 
    int shop_ending_text_x = shop_frame_x + (shop_frame.width - shop_ending_text.width) / 2;
    int shop_ending_text_y = shop_frame_y + shop_frame.height - shop_ending_text.height - 20               ; 



    DrawTexture(shop_ending_text, shop_ending_text_x, shop_ending_text_y, WHITE);
    // HEAL-A-HOLIC
    int healaholic_title_x = item_frame_x + (shop_item_frame.width - healaholic_title.width) / 2;
    int healaholic_title_y = item_frame_y1 + 15;

    DrawTexture(healaholic_title, healaholic_title_x, healaholic_title_y, WHITE);

    int healaholic_icon_frame_x = item_frame_x + 10;
    int healaholic_icon_frame_y = healaholic_title_y + healaholic_title.height - 25;

    DrawTexture(item_icon_frame, healaholic_icon_frame_x, healaholic_icon_frame_y, WHITE);

    int healaholic_image_x = healaholic_icon_frame_x + (item_icon_frame.width - healaholic_image.width) / 2;
    int healaholic_image_y = healaholic_icon_frame_y + (item_icon_frame.height - healaholic_image.height) / 2;

    DrawTexture(healaholic_image, healaholic_image_x, healaholic_image_y, WHITE);

    int healaholic_desc_x = healaholic_icon_frame_x;
    int healaholic_desc_y = healaholic_icon_frame_y + item_icon_frame.height + 10;

    DrawTexture(healaholic_desc, healaholic_desc_x, healaholic_desc_y, WHITE);

    int cost_250_x = item_frame_x + shop_item_frame.width - cost_250.width - 10;
    int cost_250_y = item_frame_y1 + shop_item_frame.height - cost_250.height - 10;

    DrawTexture(cost_250, cost_250_x, cost_250_y, WHITE);

    // PEW PEW PLUS
    int pew_pew_plus_title_x = item_frame_x + (shop_item_frame.width - pew_pew_plus_title.width) / 2;
    int pew_pew_plus_title_y = item_frame_y2 + 15;

    DrawTexture(pew_pew_plus_title, pew_pew_plus_title_x, pew_pew_plus_title_y, WHITE);

    int pew_pew_plus_icon_frame_x = item_frame_x + 10;
    int pew_pew_plus_icon_frame_y = pew_pew_plus_title_y + pew_pew_plus_title.height - 25;

    DrawTexture(item_icon_frame, pew_pew_plus_icon_frame_x, pew_pew_plus_icon_frame_y, WHITE);

    int pew_pew_plus_image_x = pew_pew_plus_icon_frame_x + (item_icon_frame.width - pew_pew_plus_image.width) / 2;
    int pew_pew_plus_image_y = pew_pew_plus_icon_frame_y + (item_icon_frame.height - pew_pew_plus_image.height) / 2;

    DrawTexture(pew_pew_plus_image, pew_pew_plus_image_x, pew_pew_plus_image_y, WHITE);

    int pew_pew_plus_desc_x = pew_pew_plus_icon_frame_x;
    int pew_pew_plus_desc_y = pew_pew_plus_icon_frame_y + item_icon_frame.height + 10;

    DrawTexture(pew_pew_plus_desc, pew_pew_plus_desc_x, pew_pew_plus_desc_y, WHITE);

    int cost_1000_x = item_frame_x + shop_item_frame.width - cost_1000.width - 10;
    int cost_1000_y = item_frame_y2 + shop_item_frame.height - cost_1000.height - 10;

    DrawTexture(cost_1000, cost_1000_x, cost_1000_y, WHITE);

    // TINY TWIN
    int tiny_twin_title_x = item_frame_x + (shop_item_frame.width - tiny_twin_title.width) / 2;
    int tiny_twin_title_y = item_frame_y3 + 15; // Adjust as needed for vertical centering

    DrawTexture(tiny_twin_title, tiny_twin_title_x, tiny_twin_title_y, WHITE);

    int tiny_twin_icon_frame_x = item_frame_x + 10; 
    int tiny_twin_icon_frame_y = tiny_twin_title_y + tiny_twin_title.height - 25;
    DrawTexture(item_icon_frame, tiny_twin_icon_frame_x, tiny_twin_icon_frame_y, WHITE);

    int tiny_twin_image_x = tiny_twin_icon_frame_x + (item_icon_frame.width - tiny_twin_image.width) / 2;
    int tiny_twin_image_y = tiny_twin_icon_frame_y + (item_icon_frame.height - tiny_twin_image.height) / 2;

    DrawTexture(tiny_twin_image, tiny_twin_image_x, tiny_twin_image_y, WHITE);

    // Position the description below the icon frame
    int tiny_twin_desc_x = tiny_twin_icon_frame_x;
    int tiny_twin_desc_y = tiny_twin_icon_frame_y + item_icon_frame.height + 10;

    DrawTexture(tiny_twin_desc, tiny_twin_desc_x, tiny_twin_desc_y, WHITE);

    // Position the cost at the bottom right of the item frame
    int shop_cost_x = item_frame_x + shop_item_frame.width - cost_2000.width - 10;
    int shop_cost_y = item_frame_y3 + shop_item_frame.height - cost_2000.height - 10;

    DrawTexture(cost_2000, shop_cost_x, shop_cost_y, WHITE);

    if (state_info(state)->purchase_complete) {
        DrawTexture(purchase_complete, shop_ending_text_x - 10, shop_ending_text_y - 70, WHITE);
        if (purchase_complete_sound_timer == 0) {
            PlaySound(bought_item);
            purchase_complete_sound_timer = 125; 
        }
    }

    if (state_info(state)->not_enough_coins) {
        DrawTexture(not_enough_coins, shop_ending_text_x + 55, shop_ending_text_y - 70, WHITE);
        if (not_enough_coins_sound_timer == 0) {
            PlaySound(cant_buy);
            not_enough_coins_sound_timer = 65;
        }
    } 

    if (purchase_complete_sound_timer > 0) {
        purchase_complete_sound_timer--;
    }
    if (not_enough_coins_sound_timer > 0) {
        not_enough_coins_sound_timer--;
    }

}

const char *introTexts[] = {
    "Hello traveler!\nI am BRUNO.",
    "You have to help me.\nMy galaxy is under attack!",
    "You must defend it from\n10 grueling waves of enemies.",
    "But be careful, daunting bosses\nawait you at waves 5 and 10!"
};
const int numTexts = sizeof(introTexts) / sizeof(introTexts[0]);

void interface_draw_intro(State state, GameState *gameState) {
    
    BeginDrawing();
    DrawTexture(space_background, 0, 0, WHITE);

    // Draw astronaut
    DrawTexture(astronaut,SCREEN_WIDTH / 2 - astronaut.width /2 ,150,WHITE);

    // Draw text box
    DrawRectangle(0, 480, SCREEN_WIDTH, 150, LIGHTGRAY);

    if (state_text(state)->textIndex >= 0 && state_text(state)->textIndex < numTexts) {
        const char *currentText = introTexts[state_text(state)->textIndex];
        int length = strlen(currentText);

        // Draw text letter by letter
        for (int i = 0; i < state_text(state)->index; i++) {
            DrawText(TextSubtext(currentText, 0, i + 1), 20, 500, 40, BLACK);
        }

        state_text(state)->timer += 1;
        if (state_text(state)->timer >= state_text(state)->delay && state_text(state)->index < length) {
            state_text(state)->index++;
            state_text(state)->timer = 0;
        }
        
        if (state_text(state)->index >= length) {
            skipTextTimer++;
            if (skipTextTimer >= 7) { 
                skipTextIndex++;
                if (skipTextIndex >= 8) 
                    skipTextIndex = 0;
                skipTextTimer = 0;
            }

            Rectangle skipTextSource = { 45 * skipTextIndex, 0, 45, 38 };
            DrawTextureRec(skip_text_button, skipTextSource, (Vector2){SCREEN_WIDTH - 70, 560}, WHITE);
            if (IsKeyPressed(KEY_ENTER)) {
                PlaySound(confirm);
                state_text(state)->textIndex++;
                if (state_text(state)->textIndex < numTexts) {
                    state_text(state)->index = 0; 
                } else {
                    gameState->introduction = false;
                    gameState->gameplay = true;
                }
            }
        }
    }

    EndDrawing();
}


// Σχεδιάζει ένα frame με την τωρινή κατάσταση του παιχνδιού
void interface_draw_frame(State state) {

    int scale_factor = 5;



    coinTimer--;
    if (coinTimer < 0) {
        coinTimer = 5;
        coinIndex++;
        if (coinIndex >= 9) {
            coinIndex = 0;
        }
    }

    Camera2D camera; // Αρχικοποιηση camera

    camera.offset = (Vector2){SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2}; // window origin
    camera.target = (Vector2){0, 0}; 
    camera.rotation = 0;
    camera.zoom = 1;
    
    // Καμερα ακολουθει διαστημοπλοιο 
    camera.target.x = state_info(state)->spaceship->position.x;
    camera.target.y = state_info(state)->spaceship->position.y;

    BeginDrawing();
    BeginMode2D(camera); 
    ClearBackground(menu_color);

    float radians = atan2(
                        state_info(state)->spaceship->orientation.x,
                        state_info(state)->spaceship->orientation.y
                        );

    float rotation = radians * (-180 / PI);
    
    rotation += 180; // add 180 for spaceship to look at correct angle
    
    Rectangle spaceship_source = {-heartIndex * 90, 0, 90, 78};
    Rectangle spaceship_dest = {
                    state_info(state)->spaceship->position.x,
                    state_info(state)->spaceship->position.y, 
                    spaceship_source.width, 
                    spaceship_source.height
                    };
                    
    DrawTexturePro(spaceship_img, spaceship_source, spaceship_dest, (Vector2){spaceship_dest.width / 2, spaceship_dest.height / 2}, rotation, WHITE);


    if (state_info(state)->buddy) {
        Rectangle buddy_source = {0, 0, tiny_twin_image.width, tiny_twin_image.height};
        Rectangle buddy_dest = {
            state_info(state)->buddy->position.x,
            state_info(state)->buddy->position.y,
            tiny_twin_image.width,
            tiny_twin_image.height
        };
        Vector2 buddy_origin = {buddy_dest.width / 2, buddy_dest.height / 2};
        DrawTexturePro(tiny_twin_image, buddy_source, buddy_dest, buddy_origin, rotation, WHITE);
    }


    Vector2 top_left = {
        state_info(state)->spaceship->position.x - ASTEROID_MAX_DIST, 
        state_info(state)->spaceship->position.y + ASTEROID_MAX_DIST
    }; // Set top_left
    Vector2 bottom_right = {
        state_info(state)->spaceship->position.x + ASTEROID_MAX_DIST, 
        state_info(state)->spaceship->position.y - ASTEROID_MAX_DIST
    };  // Set bottom_right

    List objects_in_range = state_objects(state, top_left, bottom_right);
    
    for (ListNode node = list_first(objects_in_range); 
        node != LIST_EOF;
        node = list_next(objects_in_range, node)) {

        Object object = list_node_value(objects_in_range, node);
        if (object->type == ASTEROID) {
            Rectangle source = {0, 0, asteroid_img.width, asteroid_img.height};
            Rectangle dest = {object->position.x, object->position.y, object->size * scale_factor, object->size * scale_factor};
            Vector2 origin = {object->size * scale_factor / 2, object->size * scale_factor / 2};
            DrawTexturePro(asteroid_img, source, dest, origin, 0, WHITE);
        } else if (object->type == BULLET) {
            float radians = atan2(object->orientation.y, object->orientation.x);
            float rotation = radians * (180 / PI);
            Rectangle source = {0, 0, bullet_img.width, bullet_img.height};
            Rectangle dest = {object->position.x, object->position.y, object->size * 30, object->size * 30};
            Vector2 origin = {object->size * 30 / 2, object->size * 30 / 2};
            DrawTexturePro(bullet_img, source, dest, origin, rotation, WHITE);

        } else if (object->type == ENEMY) {
            Vector2 directionToSpaceship = {
                state_info(state)->spaceship->position.x - object->position.x,
                state_info(state)->spaceship->position.y - object->position.y
            };
            float enemy_radians = atan2(directionToSpaceship.y, directionToSpaceship.x);
            float enemy_rotation = enemy_radians * (180 / PI) + 90;

            int enemy_health = object_health(object);

            int enemyHealthIndex = 3 - enemy_health;
            Rectangle source = (Rectangle){enemyHealthIndex * ENEMY_SIZE, 0, ENEMY_SIZE, ENEMY_SIZE};
            Rectangle dest = {object->position.x, object->position.y, object->size * 3, object->size * 3};
            Vector2 origin = {dest.width / 2, dest.height / 2};

            DrawTexturePro(enemy_scout, source, dest, origin, enemy_rotation, WHITE);
        } else if (object->type == BOSS) {
            Vector2 directionToSpaceship = {
                state_info(state)->spaceship->position.x - object->position.x,
                state_info(state)->spaceship->position.y - object->position.y
            };
            float boss_radians = atan2(directionToSpaceship.y, directionToSpaceship.x);
            float boss_rotation = boss_radians * (180 / PI) + 90;

            Rectangle source = (Rectangle){0, 0, BOSS_SIZE, BOSS_SIZE};
            Rectangle dest = {object->position.x, object->position.y, object->size * 3, object->size * 3 };
            Vector2 origin = {dest.width / 2, dest.height / 2};

            DrawTexturePro(boss, source, dest, origin, boss_rotation, WHITE);

        } else if (object->type == FINAL_BOSS) {
            Vector2 directionToSpaceship = {
                state_info(state)->spaceship->position.x - object->position.x,
                state_info(state)->spaceship->position.y - object->position.y
            };
            float final_boss_radians = atan2(directionToSpaceship.y, directionToSpaceship.x);
            float final_boss_rotation = final_boss_radians * (180 / PI) + 90;

            Rectangle source = (Rectangle){0, 0, FINAL_BOSS_SIZE, FINAL_BOSS_SIZE};
            Rectangle dest = {object->position.x, object->position.y, object->size * 3, object->size * 3 };
            Vector2 origin = {dest.width / 2, dest.height / 2};

            DrawTexturePro(final_boss, source, dest, origin, final_boss_rotation, WHITE);
            // draw boss shockwave
            if (state_info(state)->final_boss_attacked) {
                Color shockwave_color = Fade(GREEN, 0.5);

                // Draw the shockwave circle around the boss
                DrawCircleV(object->position, 300, shockwave_color);
            }
        }
    }
    
    EndMode2D();

    if(!state_info(state)->boss_died){
        bossHealthIndex = (44 - state_info(state)->boss_health) /(44/11)  ;

        Rectangle health_source = {bossHealthIndex * 500, 0, 500, boss_healthbar.height};
        Rectangle health_dest = {SCREEN_WIDTH / 2, SCREEN_HEIGHT - 70, 500, boss_healthbar.height};
        DrawTexturePro(boss_healthbar, health_source, health_dest, (Vector2){health_dest.width/2,health_dest.height/2}, 0, WHITE);
    }

    if(!state_info(state)->final_boss_died){
        bossHealthIndex = (132 - state_info(state)->boss_health) /(132/11)  ;

        Rectangle health_source = {bossHealthIndex * 500, 0, 500, boss_healthbar.height};
        Rectangle health_dest = {SCREEN_WIDTH / 2, SCREEN_HEIGHT - 70, 500, boss_healthbar.height};
        DrawTexturePro(boss_healthbar, health_source, health_dest, (Vector2){health_dest.width/2,health_dest.height/2}, 0, WHITE);
    }


    heartIndex = state_info(state)->spaceship->health; 

    Rectangle heart_source = (Rectangle){0, 0, HEART_SIZE * heartIndex, HEART_SIZE};
    Rectangle heart_dest = (Rectangle){0, 10, heart_source.width, heart_source.height};
    DrawTexturePro(heart, heart_source, heart_dest, (Vector2){0, 0}, 0, WHITE);

    waveIndex = state_wave(state)->current_wave - 1;

    Rectangle wave_source = (Rectangle){waveIndex * 500/2, 0, 500/2, 80/2};
    Rectangle wave_dest = (Rectangle){0, 0, wave_source.width, wave_source.height};
    DrawTexturePro(wave, wave_source, wave_dest, (Vector2){ - SCREEN_WIDTH /2 + 250/2, -20}, 0, WHITE);

    // Draw the coins and the FPS counter
    DrawText(TextFormat("%04d", state_info(state)->coins), 780, 20, 40, WHITE);
    Rectangle source = (Rectangle){20 * coinIndex, 0, 20, 20};
    Rectangle dest = (Rectangle){740, 21, source.width * 1.7, source.height * 1.7};
    DrawTexturePro(coin, source, dest, (Vector2){0, 0}, 0, WHITE);

    DrawFPS(0, 0);

    if(state_info(state)->shop_open){ // make it open only when a wave has passed
        interface_draw_shop(state);
    }
   
    EndDrawing();
}
