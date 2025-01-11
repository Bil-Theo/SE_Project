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
#include "fond1.h"
#include "pluviometre.h"
#include "meteo_reg.h"

#define BLOCK_WIDTH 180
#define BLOCK_HEIGHT 80
#define BLOCK_PADDING 10

#define BUTTON_WIDTH_fl 100
#define BUTTON_HEIGHT_fl 50

static uint8_t tx_buffer[1000];
extern volatile float pressure_hPa;
extern volatile float qte_pluie, speed_kmh;
extern float tick_count;
extern volatile hum_temp_t grandeur;
extern volatile uint16_t screen_pile;
static int cmpt = 0;
extern volatile uint8_t page;





void show_sensors(){

	  sensors_screen();
	  get_values_pressure_sensor_lps22hh();
	  get_grandeur_values_sensor_hts221();

	   snprintf((char *)tx_buffer, sizeof(tx_buffer), "%6.2f[degC]",
			   grandeur.temp);
	   setDrawText(65, 45, (char *)tx_buffer);

	   snprintf((char *)tx_buffer, sizeof(tx_buffer), "%3.2f[%c]", grandeur.hum, 37);
	   setDrawText(110 + BLOCK_WIDTH + BLOCK_PADDING, 45, (char *)tx_buffer);

	   snprintf((char *)tx_buffer, sizeof(tx_buffer),"%6.2f[hPa]", pressure_hPa);
	   setDrawText(160, 65 + BLOCK_HEIGHT + BLOCK_PADDING, (char *)tx_buffer);

//	   snprintf((char *)tx_buffer, sizeof(tx_buffer), "%6.2f[mm]",rainfall);
//	   setDrawText(60, 55, (char *)tx_buffer);
}

void show_rain(){

	  raie_screen();
	  Get_Wind_Speed();
	  detect_pluie();


	   snprintf((char *)tx_buffer, sizeof(tx_buffer), "%6.2f", qte_pluie);
	   setDrawText(65, 45, (char *)tx_buffer);

	   snprintf((char *)tx_buffer, sizeof(tx_buffer), "%3.2f[km/h]", speed_kmh);
	   setDrawText(110 + BLOCK_WIDTH + BLOCK_PADDING, 45, (char *)tx_buffer);

	   snprintf((char *)tx_buffer, sizeof(tx_buffer),"East");
	   setDrawText(200, 65 + BLOCK_HEIGHT + BLOCK_PADDING, (char *)tx_buffer);
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
	DrawBlock(70, 40, title1);
	DrawBlock(100 + BLOCK_WIDTH + BLOCK_PADDING, 40, title2);
	DrawBlock(170, 60 + BLOCK_HEIGHT + BLOCK_PADDING, title3);
}


void ephemere_screen(){
	 BSP_LCD_Init();
	BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER, SDRAM_DEVICE_ADDR);
	BSP_LCD_SetLayerVisible(LTDC_ACTIVE_LAYER, ENABLE);
	BSP_LCD_SetFont(&LCD_DEFAULT_FONT);
	BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER);
	BSP_LCD_Clear(LCD_COLOR_WHITE);
	BSP_LCD_DrawBitmap(130, 60,(uint8_t *)meteo_bmp);
}

void sensors_screen(){
	base_screen("Temp", "Humidite", "Pression");
	BSP_LCD_DrawBitmap(380, 145, (uint8_t*)btn_next_bmp);

}

void raie_screen(){
	base_screen("Pluie", "Vent", "Transfert");
    BSP_LCD_DrawBitmap(380, 145, (uint8_t*)btn_next_bmp);

}
void DrawBlock(uint16_t x, uint16_t y, const char *title) {


    // Remplir le fond (optionnel, ici blanc)
    BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
    BSP_LCD_FillRect(x - 20, y + 1, BLOCK_WIDTH - 2, BLOCK_HEIGHT - 2);

    // Afficher le titre en gras
    //BSP_LCD_DrawBitmap(0, 0, (uint8_t*)fond_bmp);
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_SetBackColor(LCD_COLOR_CYAN);
    BSP_LCD_SetFont(&Font20);
    BSP_LCD_DisplayStringAt(x + 10, y + 15, (uint8_t *)title, LEFT_MODE);
}
void Home(void){

    // Définir le texte principal
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
    BSP_LCD_SetFont(&Font24); // Police large pour un effet gras
    BSP_LCD_DisplayStringAt(0, 50, (uint8_t *)"Temps d'acquisition", CENTER_MODE);

    // Paramètres des blocs
    uint16_t blockWidth = 100;   // Largeur d'un bloc
    uint16_t blockHeight = 50;  // Hauteur d'un bloc
    uint16_t blockY = 150;      // Position Y des blocs
    uint16_t spacing = 20;      // Espace entre les blocs
    uint16_t startX = (BSP_LCD_GetXSize() - (3 * blockWidth + 2 * spacing)) / 2; // Calculer la position de départ X

    // Dessiner le premier bloc (5 sec)
    BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
    BSP_LCD_FillRect(startX, blockY, blockWidth, blockHeight);
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_SetBackColor(LCD_COLOR_CYAN);
    BSP_LCD_DisplayStringAt(100, 165, (uint8_t *)"5s", LEFT_MODE);

    // Dessiner le deuxième bloc (10 min)
    BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
    BSP_LCD_FillRect(startX + blockWidth + spacing, blockY, blockWidth, blockHeight);
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_SetBackColor(LCD_COLOR_CYAN);
    BSP_LCD_DisplayStringAt(210, 165, (uint8_t *)"10mn",LEFT_MODE);

    // Dessiner le troisième bloc (1 h)
    BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
    BSP_LCD_FillRect(startX + 2 * (blockWidth + spacing), blockY, blockWidth, blockHeight);
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_SetBackColor(LCD_COLOR_CYAN);
    BSP_LCD_DisplayStringAt(343, 165, (uint8_t *)"1h", LEFT_MODE);
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
	    int staticArray[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

	    BSP_TS_GetState(&ts);


	    if (ts.touchDetected) {
	        // If a touch is detected and no touch was previously active
	    	start_again_timer(htim2);
	        if (!is_touching) {
	            is_touching = 1;  // Mark as touching
	            if(page == 0){
	            	if ((ts.touchX[0] >= 30 && ts.touchX[0] <= 166) &&
	            	    (ts.touchY[0] >= 160 && ts.touchY[0] <= 200))
	            	{
	            	    printf("You are pressng 5s\r\n");
	            	    page = 1;
	            	}
	            	else if ((ts.touchX[0] >= 186 && ts.touchX[0] <= 300) &&
	            		    (ts.touchY[0] >= 160 && ts.touchY[0] <= 200))
	            		{
	            		printf("You are pressng 10min\r\n");
	            		page = 1;
	            		}
	            	else if ((ts.touchX[0] >= 320 && ts.touchX[0] <= 390) &&
	            		    (ts.touchY[0] >= 160 && ts.touchY[0] <= 200))
	            		{
	            		printf("You are pressng 1h\r\n");
	            		page = 1;
	            		}

	            }
	            else{

		            /*clic button stocker*/
		            if ((ts.touchX[0] >= 45 && ts.touchX[0] <= 80 + 30)
		            	            		&& (ts.touchY[0] >= 190 && ts.touchY[0]<= 260)){
		            	cmpt++;
		            	printf("Touche ecran ici %d \r\n", cmpt);
		            	//register_SD_CARD(staticArray, 10);

		            }
		            if ((ts.touchX[0] >= 380 && ts.touchX[0] <= 440)
		            		&& (ts.touchY[0] >= 145 && ts.touchY[0]<= 250)) {
		                //cmpt++;
		                //printf("Touche ecran ici %d \r\n", cmpt);
		                //show_rain();
		            	if(screen_pile == 0){
		            		show_rain();
		            	    screen_pile = 1;
		            	}
		            	else{
		            		show_sensors();
		            	    screen_pile = 0;
		            	}

		            }
		        }}
	        // Update previous state
	        prev_state.touchX[0] = ts.touchX[0];
	        prev_state.touchY[0] = ts.touchY[0];
	    } else {
	        // If no touch is detected, reset the touch state
	        is_touching = 0;
	    }
}


