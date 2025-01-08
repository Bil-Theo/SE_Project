/*
 * pluviometre.c
 *
 *  Created on: Jan 8, 2025
 *      Author: Dell
 *      but: Implimente des fonctions d'acquistion du pluviomettre
 */
#include "pluviometre.h"
#include <stdio.h>
#include <string.h>

volatile float qte_pluie = 0.0, speed_kmh = 0.0;
volatile uint32_t interrupt_count = 0;
volatile float tick_count;

void detect_pluie()
    {
        //Exemple d'utilisation du compteur d'interruptions
         qte_pluie = interrupt_count * 0.2794; // Calcul de la pluie en mm
    }
//////////////////////////////////////////////////////////////////////
void Get_Wind_Speed() {
   // Récupération des ticks capturés
     float ticks_per_second = tick_count;

   // Réinitialisation du compteur pour la prochaine mesure
   tick_count = 0;

   // Conversion des ticks/s en vitesse (km/h)
   speed_kmh = (ticks_per_second/5) * 2.4;

   }
