# projet Station Métheo

![Demo](presentation.gif)


## Introduction

Ce projet de la Station Météorologique vise à relever le défi important du changement climatique en utilisant un système embarqué " STM32 ". L'impact de l'activité humaine sur le réchauffement climatique est désormais indéniable, et la nécessité de surveiller et de comprendre les modèles météorologiques n'a jamais été aussi cruciale. Ce projet, inspiré du "**Weather Shield**" de Sparkfun, utilise la carte ***STM32F746G Discovery*** pour créer une station météo complète capable de mesurer divers paramètres météorologiques.

## Objectifs du Projet

L'objectif principal de ce projet est de concevoir et de mettre en œuvre **une station météorologique** en utilisant une carte STM32F746G Discovery. La station devrait être capable de mesurer les paramètres suivants :

- Humidité
- Température
- Pression atmosphérique
- Direction et vitesse du vent
- Précipitations quotidiennes (pluviométrie)

Les données collectées seront affichées et stockées directement sur la station en utilisant l'écran LCD intégré et la **carte µSD** pour stockage des données.

## Composants Matériels

Le matériel se présente sous la forme d'une carte STM32. Les capteurs choisis comprennent :

### 1. Température, Humidité et Pression Atmosphérique

Le bouclier ST X-NUCLEO-IKS01A3 avec les capteurs HTS221 et LPS22HH est utilisé. Vous pouvez consultez la documentation technique du bouclier pour les caractéristiques et les méthodes de récupération des données.

### 2. Capteurs de Vitesse et Direction du Vent

En utilisant les "**Weather Meters**" de Sparkfun, des informations détaillées sur ces capteurs sont disponibles dans la documentation Sparkfun et la documentation Fine Offset fournie. 

### 3. Capteur de Pluie

Le capteur de pluie utilise le principe du "**tipping bucket**".

## Affichage des Données et Interaction

Le projet utilise l'écran LCD de 480x272 pixels de la carte Discovery pour l'affichage des données. L'interaction avec la station comprend des **LED RVB** pour l'indication de l'état et un bouton-poussoir pour l'entrée utilisateur.

## Implémentation Logicielle

Le logiciel STM32 inclure :

- Acquisition des données à partir des capteurs
- Affichage des données sur l'interface graphique
- Stockage des données sur une carte SD en utilisant le middleware FatFS
- Utilisation des mécanismes de gestion de l'énergie du STM32 pour une consommation minimale

## Conclusion

Ce README fournit un aperçu du projet de la Station Météorologique, décrivant ses objectifs, ses composants matériels et son implémentation logicielle. Pour des informations détaillées, veuillez consulter la documentation du projet et le code source.
