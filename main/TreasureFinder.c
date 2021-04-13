#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>	
#include <pthread.h> 
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_wifi.h"

#include "nvs_flash.h"
#include "nvs.h"
#include "esp_timer.h"

#include "sdkconfig.h"

#include "esp_peripherals.h"
#include "periph_wifi.h"
#include "board.h"

#include "audio_mem.h"
#include "periph_touch.h"
#include "periph_adc_button.h"
#include "periph_button.h"

#include "sntp_sync.h"
#include "Rotary.h"

//#include "sdcard_list.h"
//#include "sdcard_scan.h"

#include "i2c-lcd1602.h"
#include "smbus.h"
#include <time.h>
#include "TreasureFinder.h"

#define MODE_WAIT_FOR_START 0
#define MODE_RUNNING 1

static TreasureMap_t treasureMapStruct;
int treasureAmount = 0;
int attemptsLeft;
int treasuresLeft;
Treasure_t treasures[72];
int currentXPosition;
int currentYPosition;

int state = MODE_WAIT_FOR_START;

//LCD stuff
smbus_info_t *smbus_info = NULL;
i2c_lcd1602_info_t *lcd_info = NULL;

initTreasure(){
    treasureMap.rows = 4;
    treasureMap.columns = 18;       //The width of the LCD -2, to leave room for the displaying of the attempts and treasure left. 

    srand(time(NULL));  //Create a randomizer seed using current time.
    RAND_MAX = 20;      //Random value is 20.

    for (size_t y = 0; y < treasureMap.rows; y++)
    {
        for (size_t x = 0; x < treasureMap.columns; x)
        {
            int r = rand();     //Grab a random value and see if it's 1.
            if (r == 1)         //This should make for a 1/20, or 5%, chance of any position having a treasure. 
            {
                treasureAmount++;
                treasures[treasureAmount] = new Treasure_t treasure{0, x, y}
            }
        }
    }

    treasuresLeft = treasureAmount;
    attemptsLeft = treasureAmount + 2;

    state = MODE_RUNNING;
    
    writeStartToLCD();
    treasureFinderMain();
}

treasureFinderMain(){
    //BASICALLY: The rotary encoder is supposed to be used to loop through the possible positions on the array. Clicking it 'digs' in a location. 
    //At the start of the game,  the treasures get made and flash on the screen for a moment. The user then has a certain number of tries, treasureAmount+2.
    //User wins when all treasures are found, user loses when running out of attempts.

    while (MODE_RUNNING)
    {
        writeCurrentStateToLCD();

        vTaskDelay(10);
    }
}

rotaryMoved(int direction){

    if (direction == 1)     //Rotary moved right
    {
        if (currentXPosition < treasureMapStruct.columns - 1)   //Check if X position is not at the end of the line.
        {
            currentXPosition++;
        }else if (currentYPosition < treasureMapStruct.rows - 1)    //Check if the Y position is not at the bottom of the LCD.
        {
            currentXPosition = 0;       //Move to the start of the line below
            currentYPosition++;
        }else{
            currentXPosition = 0;       //Move to the start of the line on the first row
            currentYPosition = 0;
        }
    }else if (direction == -1)      //Rotary moved left
    {
        if (currentXPosition > 0)   //Check if X position is not at the beginning of the line
        {
            currentXPosition--; 
        }else if (currentYPosition > 0)
        {
            currentXPosition = treasureMapStruct.columns - 1;   //Move to the end of a line on the previous row
            currentYPosition--;
        }else{
            currentXPosition = treasureMapStruct.columns - 1;   //Move to the end of a line on the last row
            currentYPosition = treasureMapStruct.rows - 1;
        }
    } 
}

rotaryPressed(){
    if (state = MODE_RUNNING)
    {
        attemptsLeft--;
        for (size_t i = 0; i < treasureAmount; i++) 
        {
            if (treasures[i].found == 0 && treasures[i].xPosition == currentXPosition && treasures[i] == currentYPosition)  //check if it's not been found before and if position matches.
            {
                treasures[i].found = 1;
                treasuresLeft --;

                if (treasuresLeft == 0)
                {
                    state = MODE_WAIT_FOR_START;        //Player has won
                    writeVictoryToLCD();
                }
            }
        }
        if (attemptsLeft == 0 && treasuresLeft != 0)
        {
            state = MODE_WAIT_FOR_START;        //Player has lost
            writeLossToLCD();
        }
        

    }else if (state = MODE_WAIT_FOR_START)
    {
        initTreasure();
    }
    
}

writeCurrentStateToLCD(){
    i2c_lcd1602_clear(lcd_info);

	i2c_lcd1602_move_cursor(lcd_info, treasureMapStruct.columns - 1, 0);    //-1 because the variable counts from 1, while LCD counts from 0
	i2c_lcd1602_write_string(lcd_info, attemptsLeft);

	i2c_lcd1602_move_cursor(lcd_info, treasureMapStruct.columns - 1, 1);
	i2c_lcd1602_write_string(lcd_info, treasuresLeft);

    i2c_lcd1602_move_cursor(lcd_info, currentXPosition, currentYPosition);
    i2c_lcd1602_set_cursor(lcd_info, true);
	i2c_lcd1602_set_blink(lcd_info, true);
	vTaskDelay(10);
}

writeVictoryToLCD(){
    i2c_lcd1602_clear(lcd_info);

    i2c_lcd1602_move_cursor(lcd_info, 10, 1);
    i2c_lcd1602_write_string(lcd_info, "Victory!");

    i2c_lcd1602_move_cursor(lcd_info, 0, 2);
    i2c_lcd1602_write_string(lcd_info, "Press to play again!");
}

writeLossToLCD(){
    i2c_lcd1602_clear(lcd_info);

    i2c_lcd1602_move_cursor(lcd_info, 9, 1);
    i2c_lcd1602_write_string(lcd_info, "You lose!");

    i2c_lcd1602_move_cursor(lcd_info, 0, 2);
    i2c_lcd1602_write_string(lcd_info, "Press to play again!");
}

writeStartToLCD(){
    i2c_lcd1602_clear(lcd_info);

    for (size_t i = 0; i < treasureAmount; i++){
        i2c_lcd1602_move_cursor(lcd_info, treasures[i].xPosition, treasures[i].yPosition);
	    i2c_lcd1602_write_string(lcd_info, "    ");
    }

    vTaskDelay(500);    //Pause for half a second to let the user see all the positions.
    i2c_lcd1602_clear(lcd_info);
}