#include <stdio.h>
#include <nds.h>
#include <filesystem.h>
#include <nf_lib.h>

#define MAX_LIFE 99
#define INITIAL_LIFE 20
#define MIN_LIFE 0
#define HALF_SCREEN_WIDTH 128
#define HALF_SCREEN_HEIGHT 96
#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 192
#define SHAKE_TIME_MS 500
#define SHAKE_AMOUNT 5
#define LONG_PRESS_TIME_MS 800
#define TEMP_TIME_MS 1500

// Global variables
s8 player1Life = INITIAL_LIFE;
s8 player2Life = INITIAL_LIFE;
s8 player1Temp = 0;
s8 player2Temp = 0;
u8 isTouch = 0;
u16 timer = 0;
u16 touchTimer = 0;
u8 bg1 = 0;
u8 bg2 = 0;
u8 bg1Temp = 0;
u8 bg2Temp = 0;
u8 maxHeroesId = 35;
u8 maxHeroesIdCC = 21;
u16 bgShakeTimer = 0;
u8 isShakeBg = 0;
u8 hasTemp = 0;
u16 tempTimer = 0;
u8 touchedButton = 0;
u8 heldKeySuccess = 0;
u8 lastPlayer = 0;
u8 editMode = 0;
u8 ccMode = 0;
u8 initialCCMode = 0;

char bgName[32];

char *bgMapCC[22] = {
    "default",
    "arakni",
    "azalea",
    "boltyn",
    "bravo",
    "bravoelem",
    "briar",
    "chane",
    "dash",
    "dorinthea",
    "dromai",
    "fai",
    "iyslander",
    "kano",
    "katsu",
    "levia",
    "lexi",
    "oldhim",
    "prism",
    "rhinar",
    "uzuri",
    "viserai",
};

char *bgMap[36] = {
    "default",
    "arakni",
    "arakniConfin",
    "azalea",
    "boltyn",
    "bravo",
    "briar",
    "benji",
    "chane",
    "dash",
    "datadoll",
    "dorinthea",
    "dorintheaProd",
    "dromai",
    "emperor",
    "fai",
    "grenis",
    "ira",
    "iyslander",
    "kano",
    "kassai",
    "katsu",
    "kavdaen",
    "kano",
    "levia",
    "lexi",
    "oldhim",
    "prism",
    "prismAdvent",
    "rhinar",
    "shiyana",
    "taylor",
    "uzuri",
    "valda",
    "viserai",
    "yoji",
};

void updateLife(u8 player, s8 change);
void displayLifeTotals();
void handleUserInput();
void initializeGraphics();
void displayBg();
void displayButtons();
void shakeBg(u8 layer);
void handleKeyHeld(u8 rotId, u8 player, s8 change);
void handleKeyUp(u8 rotId, u8 player, s8 change);
void tempLifeLogic();

int main(int argc, char **argv)
{
    nitroFSInit(NULL);
    initializeGraphics();
    displayBg();

    while (1)
    { // Main game loop
        if (editMode)
        {
            editModeText();
            handleUserInputEditMode();
        }
        else
        {

            displayButtons();
            displayLifeTotals();
            handleUserInput();
            if (hasTemp == 1)
            {
                tempLifeLogic();
            }
        }

        // ...

        // Other game logic

        swiWaitForVBlank();
        oamUpdate(&oamMain);
        oamUpdate(&oamSub);
    }
    return 0;
}

void updateGameMode(u8 newMode)
{
    ccMode = newMode;
    player1Life = INITIAL_LIFE * (ccMode ? 2 : 1);
    player2Life = INITIAL_LIFE * (ccMode ? 2 : 1);
    int tempArray[2] = {bg1Temp, bg2Temp};

    for (int k = 0; k < 2; k++)
    {
        if (tempArray[k] > 0)
        {
            int found = 0;
            for (int i = 1; i <= (ccMode ? maxHeroesIdCC : maxHeroesId); i++)
            {
                if (ccMode)
                {
                    if (strcmp(bgMap[tempArray[k]], bgMapCC[i]) == 0)
                    {
                        tempArray[k] = i;
                        found = 1;
                        break;
                    }
                }
                else
                {
                    if (strcmp(bgMapCC[tempArray[k]], bgMap[i]) == 0)
                    {
                        tempArray[k] = i;
                        found = 1;
                        break;
                    }
                }
            }
            if (!found)
            {
                tempArray[k] = 0;
            }
        }
    }

    bg1Temp = tempArray[0];
    bg2Temp = tempArray[1];
}

void enterEditMode()
{
    editMode = 1;
    initialCCMode = ccMode;
    // update backgrounds
    bg1Temp = bg1;
    bg2Temp = bg2;
    NF_LoadTextFont16("fnt/font16", "edit", 256, 256, 0);
    NF_CreateTextLayer16(1, 0, 0, "edit");
    NF_ClearTextLayer16(1, 0);
    // hide buttons
    NF_ShowSprite(1, 4, false);
    NF_ShowSprite(1, 5, false);
    NF_ShowSprite(1, 6, false);
    NF_ShowSprite(1, 7, false);
    NF_SpriteOamSet(1);
}

void exitEditMode()
{
    editMode = 0;

    if (bg1 == bg2)
    {
        if (ccMode != initialCCMode)
        {
            NF_UnloadTiledBg(!ccMode ? bgMapCC[bg1] : bgMap[bg1]);
        }
        else
        {
            NF_UnloadTiledBg(ccMode ? bgMapCC[bg1] : bgMap[bg1]);
        }
    }
    else
    {
        if (ccMode != initialCCMode)
        {
            NF_UnloadTiledBg(!ccMode ? bgMapCC[bg1] : bgMap[bg1]);
            NF_UnloadTiledBg(!ccMode ? bgMapCC[bg2] : bgMap[bg2]);
        }
        else
        {
            NF_UnloadTiledBg(ccMode ? bgMapCC[bg1] : bgMap[bg1]);
            NF_UnloadTiledBg(ccMode ? bgMapCC[bg2] : bgMap[bg2]);
        }
    }
    bg1 = bg1Temp;
    snprintf(
        bgName,
        sizeof(bgName),
        "bg/%s%s",
        ccMode ? bgMapCC[bg1] : bgMap[bg1],
        ccMode || !bg1 ? "" : "_young");
    NF_LoadTiledBg(bgName,
                   ccMode ? bgMapCC[bg1] : bgMap[bg1], 256, 256);
    NF_CreateTiledBg(0, 3, ccMode ? bgMapCC[bg1] : bgMap[bg1]);
    NF_ScrollBg(0, 3, HALF_SCREEN_WIDTH, 0);

    bg2 = bg2Temp;
    snprintf(
        bgName,
        sizeof(bgName),
        "bg/%s%s",
        ccMode ? bgMapCC[bg2] : bgMap[bg2],
        ccMode || !bg2 ? "" : "_young");
    NF_LoadTiledBg(bgName,
                   ccMode ? bgMapCC[bg2] : bgMap[bg2], 256, 256);
    NF_CreateTiledBg(0, 2, ccMode ? bgMapCC[bg2] : bgMap[bg2]);

    NF_ShowSprite(1, 4, true);
    NF_ShowSprite(1, 5, true);
    NF_ShowSprite(1, 6, true);
    NF_ShowSprite(1, 7, true);
    NF_ClearTextLayer16(1, 0);
    NF_UpdateTextLayers();
    updateGameMode(ccMode);
}

void editModeText()
{
    NF_ClearTextLayer16(1, 0);
    NF_WriteText16(1, 0,
                   0,
                   0, "<");
    NF_WriteText16(1, 0,
                   14,
                   0, ">");
    NF_WriteText16(1, 0,
                   16,
                   0, "<");
    NF_WriteText16(1, 0,
                   31,
                   0, ">");

    snprintf(
        bgName,
        sizeof(bgName),
        "%c%s",
        ccMode ? *bgMapCC[bg1Temp] - 32 : *bgMap[bg1Temp] - 32,
        ccMode ? bgMapCC[bg1Temp] + 1 : bgMap[bg1Temp] + 1);

    NF_WriteText16(1, 0,
                   2,
                   0, bgName);
    snprintf(
        bgName,
        sizeof(bgName),
        "%c%s",
        ccMode ? *bgMapCC[bg2Temp] - 32 : *bgMap[bg2Temp] - 32,
        ccMode ? bgMapCC[bg2Temp] + 1 : bgMap[bg2Temp] + 1);
    NF_WriteText16(1, 0,
                   18,
                   0, bgName);

    NF_WriteText16(1, 0,
                   0,
                   2, "CC_MODE=");
    NF_WriteText16(1, 0,
                   8,
                   2, ccMode ? "ON" : "OFF");
    NF_UpdateTextLayers();
}

void handleUserInputEditMode()
{
    scanKeys();
    u8 maxId = ccMode ? maxHeroesIdCC : maxHeroesId;

    if (keysDown() & KEY_LEFT)
    {
        if (bg1Temp == 0)
        {
            bg1Temp = maxId;
        }
        else
        {
            bg1Temp--;
        }
    }

    if (keysDown() & KEY_RIGHT)
    {
        if (bg1Temp == maxId)
        {
            bg1Temp = 0;
        }
        else
        {
            bg1Temp++;
        }
    }

    if (keysDown() & KEY_Y)
    {
        if (bg2Temp == 0)
        {
            bg2Temp = maxId;
        }
        else
        {
            bg2Temp--;
        }
    }

    if (keysDown() & KEY_A)
    {
        if (bg2Temp == maxId)
        {
            bg2Temp = 0;
        }
        else
        {
            bg2Temp++;
        }
    }

    if (keysDown() & KEY_SELECT)
    {
        exitEditMode();
    }

    if (keysDown() & KEY_START)
    {
        updateGameMode(!ccMode);
    }
}

void updateLife(u8 player, s8 change)
{
    if (lastPlayer != player)
    {
        hasTemp = 0;
        player1Temp = 0;
        player2Temp = 0;
    }
    lastPlayer = player;

    if (player == 1)
    {
        player1Temp += change;
        player1Life += change;
        if (player1Life < MIN_LIFE)
            player1Life = MIN_LIFE;
        else if (player1Life > MAX_LIFE)
            player1Life = MAX_LIFE;
    }
    else if (player == 2)
    {
        player2Temp += change;
        player2Life += change;
        if (player2Life < MIN_LIFE)
            player2Life = MIN_LIFE;
        else if (player2Life > MAX_LIFE)
            player2Life = MAX_LIFE;
    }
    s8 playerTemp = player == 1 ? player1Temp : player2Temp;
    // convert player temp to string
    char playerTempStr[32];
    //  how the fuck do you print negative s8

    snprintf(
        playerTempStr,
        sizeof(playerTempStr),
        "%c%i%c",
        playerTemp < 0 ? '-' : '+',
        abs(playerTemp),
        playerTemp >= 5 || playerTemp <= -5 ? '!' : ' ');

    NF_ClearTextLayer(0, 0);
    NF_WriteText16(0, 0,
                   7 + (player == 1 ? 0 : 16),
                   7, playerTempStr);

    tempTimer = 0;
    hasTemp = 1;
}

void tempLifeLogic()
{
    tempTimer += 16;

    if (tempTimer > TEMP_TIME_MS)
    {
        tempTimer = 0;
        hasTemp = 0;
        player1Temp = 0;
        player2Temp = 0;
        NF_ClearTextLayer(0, 0);
    }
    NF_UpdateTextLayers();
}

void initializeGraphics()
{
    // Initialize the DS graphics system
    // ...

    NF_Set2D(0, 0);
    NF_Set2D(1, 0);
    NF_SetRootFolder("NITROFS");
    consoleDemoInit();
    NF_InitSpriteBuffers();
    NF_InitSpriteSys(0);
    NF_InitSpriteSys(1);
    NF_InitTextSys(0);
    NF_InitTextSys(1);

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
    NF_EnableSpriteRotScale(1, 5, 1, 0);

    NF_CreateSprite(1, 6, 1, 1, 160, 32);
    NF_EnableSpriteRotScale(1, 6, 2, 0);

    NF_CreateSprite(1, 7, 1, 1, 160, 96);
    NF_EnableSpriteRotScale(1, 7, 3, 0);

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

void displayLifeTotals()
{
    // Display the life totals for both players on the DS screen
    // Use the DS graphics system to render the life totals using custom font/sprites
    // ...
    NF_SpriteFrame(0, 0, player1Life / 10);
    NF_SpriteFrame(0, 1, player1Life % 10);
    NF_SpriteFrame(0, 2, player2Life / 10);
    NF_SpriteFrame(0, 3, player2Life % 10);

    NF_SpriteOamSet(0);
}

void handleUserInput()
{
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
    if (keysHeld() & KEY_TOUCH)
    {
        if (touch.px > 32 && touch.px < 96 && touch.py > 32 && touch.py < 88)
        {
            handleKeyHeld(0, 1, 1);
            touchedButton = 1;
        }
        else if (touch.px > 32 && touch.px < 96 && touch.py > 96 && touch.py < 148)
        {
            handleKeyHeld(1, 1, -1);
            touchedButton = 2;
        }
        else if (touch.px > 160 && touch.px < 224 && touch.py > 32 && touch.py < 88)
        {
            handleKeyHeld(2, 2, 1);
            touchedButton = 3;
        }
        else if (touch.px > 160 && touch.px < 224 && touch.py > 96 && touch.py < 148)
        {
            handleKeyHeld(3, 2, -1);
            touchedButton = 4;
        }
        else
        {
            touchedButton = 0;
        }
    }

    if (keysUp() & KEY_TOUCH)
    {
        if (touchedButton == 1)
        {
            handleKeyUp(0, 1, 1);
        }
        else if (touchedButton == 2)
        {
            handleKeyUp(1, 1, -1);
        }
        else if (touchedButton == 3)
        {
            handleKeyUp(2, 2, 1);
        }
        else if (touchedButton == 4)
        {
            handleKeyUp(3, 2, -1);
        }
    }

    // if press start, reset to initial
    if (keysDown() & KEY_START)
    {
        player1Life = INITIAL_LIFE * (ccMode ? 2 : 1);
        player2Life = INITIAL_LIFE * (ccMode ? 2 : 1);
    }

    if (keysHeld() & KEY_UP)
    {
        handleKeyHeld(0, 1, 1);
    }

    if (keysUp() & KEY_UP)
    {
        handleKeyUp(0, 1, 1);
    }

    if (keysHeld() & KEY_DOWN)
    {
        handleKeyHeld(1, 1, -1);
    }

    if (keysUp() & KEY_DOWN)
    {
        handleKeyUp(1, 1, -1);
    }

    if (keysHeld() & KEY_X)
    {
        handleKeyHeld(2, 2, 1);
    }
    if (keysUp() & KEY_X)
    {
        handleKeyUp(2, 2, 1);
    }

    if (keysHeld() & KEY_B)
    {
        handleKeyHeld(3, 2, -1);
    }

    if (keysUp() & KEY_B)
    {
        handleKeyUp(3, 2, -1);
    }

    if (keysDown() & KEY_SELECT)
    {
        enterEditMode();
    }

    if (isShakeBg != 0)
    {
        shakeBg(isShakeBg);
    }
}

void handleKeyHeld(u8 rotId, u8 player, s8 change)
{
    NF_SpriteRotScale(1, rotId, 0, 290, 290);
    timer += 16;
    if (timer >= LONG_PRESS_TIME_MS)
    {
        updateLife(player, change * 5);
        if (change == -1)
        {
            if (player == 1)
            {
                isShakeBg = 3;
            }
            else
            {
                isShakeBg = 2;
            }
        }
        timer = 0;
        heldKeySuccess = 1;
    }
}

void handleKeyUp(u8 rotId, u8 player, s8 change)
{
    NF_SpriteRotScale(1, rotId, 0, 256, 256);
    if (!heldKeySuccess)
    {
        updateLife(player, change);
    }

    timer = 0;
    heldKeySuccess = 0;
}

void displayBg()
{
    // Display the background image on top/bottom screen
    // ...
    NF_InitTiledBgBuffers();
    NF_InitTiledBgSys(0);
    NF_InitTiledBgSys(1);
    NF_LoadTextFont16("fnt/font16", "down", 256, 256, 0);
    NF_CreateTextLayer16(0, 0, 0, "down");
    NF_LoadTiledBg("bg/default", "default", 256, 256);
    NF_LoadTiledBg("bg/background", "background", 256, 256);
    NF_ClearTextLayer16(0, 0);
    NF_UpdateTextLayers();
    NF_CreateTiledBg(0, 3, "default");
    NF_CreateTiledBg(0, 2, "default");
    NF_CreateTiledBg(1, 3, "background");

    NF_ScrollBg(0, 3, HALF_SCREEN_WIDTH, 0);
}

void displayButtons()
{
    // Display the buttons on the bottom screen
    // ...
    NF_SpriteOamSet(1);
}

// shake bg for 1 second, random amounts
void shakeBg(u8 layer)
{
    int x = rand() % SHAKE_AMOUNT;
    int y = rand() % SHAKE_AMOUNT;

    if (rand() % 2 == 0)
    {
        y = -y;
    }
    if (layer == 3)
    {
        x = HALF_SCREEN_WIDTH - x;
    }
    NF_ScrollBg(0, layer, x, y);

    bgShakeTimer += 16;
    if (bgShakeTimer >= SHAKE_TIME_MS)
    {
        if (layer == 3)
        {
            NF_ScrollBg(0, layer, 128, 0);
        }
        else
        {
            NF_ScrollBg(0, layer, 0, 0);
        }

        bgShakeTimer = 0;
        isShakeBg = 0;
    }
}