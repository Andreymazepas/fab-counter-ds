#include <stdio.h>
#include <nds.h>
#include <filesystem.h>
#include <nf_lib.h>

#define MAX_LIFE 99
#define MIN_LIFE 0

int player1Life;
int player2Life;

void updateLife(int player, int change);
void displayLifeTotals();
void handleUserInput();
void initializeGraphics();
void displayBg();
void displayButtons();

int main() {
    consoleDemoInit();
    initializeGraphics();

    // Initialize life totals
    player1Life = 20;
    player2Life = 20;

    while (1) {  // Main game loop
        displayBg();
        displayButtons();
        displayLifeTotals();
        handleUserInput();
        // ...

        // Other game logic

        // Probably reset?
        // ...
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
}

void displayLifeTotals() {
    // Display the life totals for both players on the DS screen
    // Use the DS graphics system to render the life totals using custom font/sprites
    // ...
}

void handleUserInput() {
    // Handle user input for life changes
    // Validate and process the input
    // Update the life totals using the updateLife() function
    // ...
}

void displayBg() {
    // Display the background image on top/bottom screen
    // ...
}

void displayButtons() {
    // Display the buttons on the bottom screen
    // ...
}