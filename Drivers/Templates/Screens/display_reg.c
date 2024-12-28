/*
 * display_reg.c
 *
 *  Created on: Dec 28, 2024
 *      Author: Dell
 */

#include "display_reg.h"
#include "pression.h"
#include <stdio.h>
#include <string.h>


#include "hts221_reg.h"
#include "humidity.h"
#include "next.h"
#include "meteo_reg.h"
#include "fond.h"


#define BLOCK_WIDTH 200
#define BLOCK_HEIGHT 100
#define BLOCK_PADDING 20

#define BUTTON_WIDTH_fl 100
#define BUTTON_HEIGHT_fl 50

static uint8_t tx_buffer[1000];
extern volatile float pressure_hPa;
extern volatile hum_temp_t grandeur;
extern volatile uint16_t cmpt, screen_pile;

void show_sensors(){
	  get_values_pressure_sensor_lps22hh();
	  get_grandeur_values_sensor_hts221();

	   snprintf((char *)tx_buffer, sizeof(tx_buffer), "%6.2f[degC]",
			   grandeur.temp, '\xB0');
	   setDrawText(20, 20, (char *)tx_buffer);

	   snprintf((char *)tx_buffer, sizeof(tx_buffer), "%3.2f[%c]", grandeur.hum, 37);
	   setDrawText(20 + BLOCK_WIDTH + BLOCK_PADDING, 20, (char *)tx_buffer);

	   snprintf((char *)tx_buffer, sizeof(tx_buffer),"%6.2f[hPa]", pressure_hPa);
	   setDrawText(20, 20 + BLOCK_HEIGHT + BLOCK_PADDING, (char *)tx_buffer);
}


void setDrawText(uint16_t x, uint16_t y, const char *value){
	  // Afficher la valeur
	    BSP_LCD_SetFont(&Font16);
	    BSP_LCD_DisplayStringAt(x + 10, y + 40, (uint8_t *)value, LEFT_MODE);
}

void base_screen(char *title1, char *title2, char *title3){
  BSP_LCD_Clear(LCD_COLOR_WHITE);
  // Dessiner les trois blocs
  //BSP_LCD_DrawBitmap(0, 0, (uint8_t*)fond_bmp);
	DrawBlock(20, 20, title1);
	DrawBlock(20 + BLOCK_WIDTH + BLOCK_PADDING, 20, title2);
	DrawBlock(20, 20 + BLOCK_HEIGHT + BLOCK_PADDING, title3);
}


void ephemere_screen(){
	 BSP_LCD_Init();
	BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER, SDRAM_DEVICE_ADDR);
	BSP_LCD_SetLayerVisible(LTDC_ACTIVE_LAYER, ENABLE);
	BSP_LCD_SetFont(&LCD_DEFAULT_FONT);
	BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER);
	BSP_LCD_Clear(LCD_COLOR_WHITE);
	BSP_LCD_DrawBitmap(130, 60, (uint8_t*)meteo_bmp);
}

void sensors_screen(){
	base_screen("Temperature", "Humidite", "Pression");
	BSP_LCD_DrawBitmap(380, 180, (uint8_t*)btn_next_bmp);
	show_sensors();
}

void raie_screen(){
	base_screen("Pluie", "Vent", "Transfert");
    BSP_LCD_DrawBitmap(380, 180, (uint8_t*)btn_next_bmp);
}
void DrawBlock(uint16_t x, uint16_t y, const char *title) {
    // Dessiner le rectangle
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_DrawRect(x, y, BLOCK_WIDTH, BLOCK_HEIGHT);

    // Remplir le fond (optionnel, ici blanc)
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_FillRect(x + 1, y + 1, BLOCK_WIDTH - 2, BLOCK_HEIGHT - 2);

    // Afficher le titre en gras
    //BSP_LCD_DrawBitmap(0, 0, (uint8_t*)fond_bmp);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
    BSP_LCD_SetFont(&Font20);
    BSP_LCD_DisplayStringAt(x + 10, y + 10, (uint8_t *)title, LEFT_MODE);
}


void error(uint8_t * message) {
    // Initialisation du LCD
    BSP_LCD_Init();

    // Initialisation des couches LCD
    BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER, SDRAM_DEVICE_ADDR);
    BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER);

    // Configuration de la police, des couleurs et du fond
    BSP_LCD_SetFont(&Font16); // Choisissez une police lisible
    BSP_LCD_SetBackColor(LCD_COLOR_RED); // Fond rouge
    BSP_LCD_Clear(LCD_COLOR_RED); // Efface l'écran avec le fond rouge
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE); // Texte blanc

    // Affichage du message
    BSP_LCD_DisplayStringAt(10, 50, message, LEFT_MODE);
}

void TouchScreen(){
	TS_StateTypeDef ts = {0};                  // Zero-initialize the current state to ensure no uninitialized data
	    static TS_StateTypeDef prev_state = {0};
	    static uint8_t is_touching = 0;           // Track if a touch is currently active

	    BSP_TS_GetState(&ts);

	    if (ts.touchDetected) {
	        // If a touch is detected and no touch was previously active
	    	start_again_timer(htim2);
	        if (!is_touching) {
	            is_touching = 1;  // Mark as touching
	            if ((ts.touchX[0] >= 380 && ts.touchX[0] <= 380 + 60)
	            		&& (ts.touchY[0] >= 180 && ts.touchY[0]<= 180 + 50)) {
	                //cmpt++;
	                //printf("Touche ecran ici %d \r\n", cmpt);
	                raie_screen();
	            	if(screen_pile == 0){
	            		raie_screen();

	            	    screen_pile = 1;
	            	}
	            	else{
	            		sensors_screen();
	            	    screen_pile = 0;
	            	}

	            }
	        }
	        // Update previous state
	        prev_state.touchX[0] = ts.touchX[0];
	        prev_state.touchY[0] = ts.touchY[0];
	    } else {
	        // If no touch is detected, reset the touch state
	        is_touching = 0;
	    }
}
