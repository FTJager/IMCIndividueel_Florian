#ifndef TREASURE_FINDER_H
#define TREASURE_FINDER_H

/**
 * @brief creates treasures on the field.
 * */
void initTreasure(void);

/**
 * 
 * @brief Used to keep track of the amount of treasure locations that are available.
 * */
typedef struct 
{
    int rows;       //Amount of rows on LCD
    int columns;    //Amount of columns on LCD
}TreasureMap_t;

/**
 * @brief Treasure, with a found toggle and its location.
 * */
typedef struct 
{
    int found;
    int xPosition;
    int yPosition;
}Treasure_t;

/**
 * @brief Main game logic loop.
 * */
void treasureFinderMain(void);

/**
 * @brief Gets called when the rotary encoder is turned.
 * 
 * @param direction Indicates if the rotary moved left or right.
 * */
void rotaryMoved(int direction);

/**
 * @brief Gets called when the rotary encoder's button is pressed.
 * */
void rotaryPressed();

/**
 * @brief Writes the current cursor position and game stats to the LCD.
 * */
void writeCurrentStateToLCD(void);

/**
 * @brief Writes the victory screen to the LCD.
 * */
void writeVictoryToLCD(void);

/**
 * @brief Writes the loss screen to the LCD.
 * */
void writeLossToLCD(void);

/**
 * @brief Writes the initial display of treasure locations to the LCD.
 * */
void writeStartToLCD(void);

#endif