#ifndef __COLORS_H
#define __COLORS_H

/**************************************************
 * This file is for setting and configuring all 
 * the color schemes used in atm cli program
 *************************************************/

/*
 * Preprocessor definition for enabling and disabling colorisation
 */
#define COLORISE 1
#define WELCOME_COLOR           FCOLOR_BLUE
#define WITHDRAW_DATA_COLOR     FCOLOR_BOLD
#define ERROR_COLOR             FCOLOR_RED


#ifdef _WIN32
    #define FCOLOR_RED          FOREGROUND_RED
    #define FCOLOR_GREEN        FOREGROUND_GREEN
    #define FCOLOR_BLUE         FOREGROUND_BLUE  
    #define FCOLOR_BOLD         FOREGROUND_INTENSITY
#endif

#endif
