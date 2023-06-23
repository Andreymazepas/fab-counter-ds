#include <stdio.h>
#include <nds.h>
#include <filesystem.h>
#include <nf_lib.h>

#define MAX_LIFE 99
#define MIN_LIFE 0

int player1Life;
int player2Life;
int isTouch = 0;

void updateLife(int player, int change);
void displayLifeTotals();
void handleUserInput();
void initializeGraphics();
void displayBg();
void displayButtons();

int main(int argc, char **argv) {
    nitroFSInit(NULL);
    initializeGraphics();
    displayBg();


    // Initialize life totals
    player1Life = 20;
    player2Life = 20;
    


    while (1) {  // Main game loop
        displayButtons();
        displayLifeTotals();
        handleUserInput();
        // ...

        // Other game logic

        // Probably reset?
        // ...
        swiWaitForVBlank();
        oamUpdate(&oamMain);
		oamUpdate(&oamSub);
    }
    return 0;
}

void updateLife(int player, int change) {
    // Update the life total for the specified player
    if (player == 1) {
        player1Life += change;
        if (player1Life < MIN_LIFE)
            player1Life = MIN_LIFE;
        else if (player1Life > MAX_LIFE)
            player1Life = MAX_LIFE;
    } else if (player == 2) {
        player2Life += change;
        if (player2Life < MIN_LIFE)
            player2Life = MIN_LIFE;
        else if (player2Life > MAX_LIFE)
            player2Life = MAX_LIFE;
    }
}

void initializeGraphics() {
    // Initialize the DS graphics system
    // ...
    
    NF_Set2D(0, 0);
    NF_Set2D(1, 0);
    NF_SetRootFolder("NITROFS");
    

    NF_InitSpriteBuffers();		
	NF_InitSpriteSys(0);		
	NF_InitSpriteSys(1);

    NF_LoadSpriteGfx("sprite/numbers", 0, 64, 64);
    NF_LoadSpritePal("sprite/numbers", 0);

    NF_LoadSpriteGfx("sprite/buttons", 1, 64, 64);
    NF_LoadSpritePal("sprite/buttons", 1);

    NF_VramSpriteGfx(0, 0, 0, false);
    NF_VramSpritePal(0, 0, 0);

    NF_VramSpriteGfx(1, 1, 1, false);
    NF_VramSpritePal(1, 1, 1);

    NF_CreateSprite(0, 0, 0, 0, 0, 128);   
    NF_CreateSprite(0, 1, 0, 0, 64, 128);

    NF_CreateSprite(0, 2, 0, 0, 128, 128);
    NF_CreateSprite(0, 3, 0, 0, 192, 128);




    NF_CreateSprite(1, 4, 1, 1, 32, 32);
    NF_CreateSprite(1, 5, 1, 1, 32, 96);

    NF_CreateSprite(1, 6, 1, 1, 160, 32);
    NF_CreateSprite(1, 7, 1, 1, 160, 96);


    NF_SpriteFrame(1, 5, 1);
    NF_SpriteFrame(1, 7, 1);

    /*
        0 - first digit, first player
        1 - second digit, first player
        2 - first digit, second player
        3 - second digit, second player
        4 - plus button, first player
        5 - minus button, first player
        6 - plus button, second player
        7 - minus button, second player
    
    */
}

void displayLifeTotals() {
    // Display the life totals for both players on the DS screen
    // Use the DS graphics system to render the life totals using custom font/sprites
    // ...
    NF_SpriteFrame(0, 0, player1Life / 10);
    NF_SpriteFrame(0, 1, player1Life % 10);
    NF_SpriteFrame(0, 2, player2Life / 10);
    NF_SpriteFrame(0, 3, player2Life % 10);

    NF_SpriteOamSet(0);
	
}

void handleUserInput() {
    // Handle user input for life changes
    // Validate and process the input
    // Update the life totals using the updateLife() function
    // ...
    scanKeys();
    // if touch between 32 and 96 on x axis, and between 32 and 64 on y axis, then player 1 +1
    // if touch between 32 and 96 on x axis, and between 96 and 128 on y axis, then player 1 -1
    // if touch between 160 and 224 on x axis, and between 32 and 64 on y axis, then player 2 +1
    // if touch between 160 and 224 on x axis, and between 96 and 128 on y axis, then player 2 -1
    touchPosition touch;

    touchRead(&touch);
    if(keysDown() & KEY_TOUCH && isTouch == 0) {
        isTouch = 1;
    if (touch.px > 32 && touch.px < 96 && touch.py > 32 && touch.py < 64) {
        updateLife(1, 1);
    } else if (touch.px > 32 && touch.px < 96 && touch.py > 96 && touch.py < 128) {
        updateLife(1, -1);
    } else if (touch.px > 160 && touch.px < 224 && touch.py > 32 && touch.py < 64) {
        updateLife(2, 1);
    } else if (touch.px > 160 && touch.px < 224 && touch.py > 96 && touch.py < 128) {
        updateLife(2, -1);
    }}

    if(keysUp() & KEY_TOUCH || !(keysHeld() & KEY_TOUCH) || !(keysDown() & KEY_TOUCH)) {
        isTouch = 0;
    }

    // if press start, reset to 20
    if (keysDown() & KEY_START) {
        player1Life = 20;
        player2Life = 20;
    }

    if (keysDown() & KEY_UP) {
        updateLife(1, 1);
    }
    if (keysDown() & KEY_DOWN) {
        updateLife(1, -1);
    }
    if (keysDown() & KEY_X) {
        updateLife(2, 1);
    }
    if (keysDown() & KEY_B) {
        updateLife(2, -1);
    }


}

void displayBg() {
    // Display the background image on top/bottom screen
    // ...
    NF_InitTiledBgBuffers();
    NF_InitTiledBgSys(0);
    NF_InitTiledBgSys(1);
    NF_LoadTiledBg("bg/fai", "fai", 256, 256);
    NF_CreateTiledBg(0, 3, "fai");
}

void displayButtons() {
    // Display the buttons on the bottom screen
    // ...
    NF_SpriteOamSet(1);

}