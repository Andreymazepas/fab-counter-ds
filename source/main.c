#include <stdio.h>
#include <nds.h>
#include <filesystem.h>
#include <nf_lib.h>

#define MAX_LIFE 99
#define MIN_LIFE 0

int player1Life;
int player2Life;
int player1Temp;
int player2Temp;
int isTouch = 0;
int timer = 0;
int touchTimer = 0;

int bg1 = 0;
int bg2 = 0;
// map id to bg
// 0 - fai
// 1 - oldhin
// 2 - rhinar
char *bgMap[3] = {"fai", "oldhin", "rhinar"};
int maxHeroesId = 2;
int bgShakeTimer = 0;
int isShakeBg = 0;


void updateLife(int player, int change);
void displayLifeTotals();
void handleUserInput();
void initializeGraphics();
void displayBg();
void displayButtons();
void shakeBg(int layer);



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
    
    // if(timer > 0 && timer < 60 * 3) {
    //     timer = 0;
    //     return;
    // }
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
        NF_EnableSpriteRotScale(1, 4, 0, 0);

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
    // holding button for 2 seconds updateLife by 5
    if (keysHeld() & KEY_UP) {
        // flash buttons sprite 0
        u8 r, g, b;
	    s8 red, green, blue;
        NF_SpriteRotScale(1, 0, 0, 290, 290);
        // for (u16 n = 1; n < 256; n ++) {
		// 		// Obten el valor actual del color en la paleta
		// 		NF_SpriteGetPalColor(1, 1, n, &r, &g, &b);
		// 		red = r;
		// 		green = g;
		// 		blue = b;
		// 		// Modifica los valores
		// 		red --;
		// 		if (red < 0) red = 31;
		// 		//green ++;
		// 		//if (green > 31) green = 0;
		// 		blue ++;
		// 		if (blue > 31) blue = 0;
		// 		// Actualiza el color en la paleta en RAM
		// 		NF_SpriteSetPalColor(1, 1, n, red, green, blue);
		// 	}

        timer++;
        if(timer == 60 * 1) {
            updateLife(1, 5);
            isShakeBg = 3;
        }
    }
    if (keysUp() & KEY_UP) {
        // unflash buttons sprite 0
        NF_SpriteRotScale(1, 0, 0, 256,256);
        NF_SpriteUpdatePalette(1, 1);
        if(timer < 60 * 1) {
            updateLife(1, 1);
        }
        
        timer = 0;
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

    if(keysDown() & KEY_L) {
        if(bg1 == maxHeroesId) {
            bg1 = 0;
        } else {
            bg1++;
        }

        NF_CreateTiledBg(0, 3, bgMap[bg1]);
        NF_ScrollBg(0, 3, 128, 0);
    }

    if(keysDown() & KEY_R) {
        if(bg2 == maxHeroesId) {
            bg2 = 0;
        } else {
            bg2++;
        }

        NF_CreateTiledBg(0, 2, bgMap[bg2]);
    }

    if(isShakeBg != 0) {
        shakeBg(isShakeBg);
        
    }


}

void displayBg() {
    // Display the background image on top/bottom screen
    // ...
    NF_InitTiledBgBuffers();
    NF_InitTiledBgSys(0);
    NF_InitTiledBgSys(1);
    NF_LoadTiledBg("bg/fai", "fai", 256, 256);
    NF_LoadTiledBg("bg/oldhim", "oldhin", 256, 256);
    NF_LoadTiledBg("bg/rhinar", "rhinar", 256, 256);
    NF_CreateTiledBg(0, 3, "fai");
    NF_CreateTiledBg(0, 2, "oldhin");
    NF_ScrollBg(0, 3, 128, 0);
}

void displayButtons() {
    // Display the buttons on the bottom screen
    // ...
    NF_SpriteOamSet(1);

}

// shake bg for 1 second, random amounts
void shakeBg(int layer) {
    int x = rand() % 10;
    int y = rand() % 10;
    if(rand() % 2 == 0) {
        x = -x;
    }
    if(rand() % 2 == 0) {
        y = -y;
    }
    if(layer == 3) {
        x = x + 128;
    }
    NF_ScrollBg(0, layer, x, y);

    bgShakeTimer++;
    if(bgShakeTimer == 60 * 1) {
        if (layer == 3) {
            NF_ScrollBg(0, layer, 128, 0);
        } else {
            NF_ScrollBg(0, layer, 0, 0);
        }

        bgShakeTimer = 0;
        isShakeBg = 0;
    }
}