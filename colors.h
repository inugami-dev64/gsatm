#ifndef __COLORS_H
#define __COLORS_H

/**************************************************
 * This file is for setting and configuring all 
 * the color schemes used in atm cli program
 *************************************************/

/*
 * Preprocessor definition for enabling and disabling colorisation
 */
#define COLORISE
#define WELCOME_COLOR           FCOLOR_YELLOW
#define INPUT_POINT_COLOR       FCOLOR_GREEN
#define CURRENCY_COLOR          BCOLOR_BLUE
#define WITHDRAW_COLOR          FCOLOR_YELLOW
#define ERR_COLOR               FCOLOR_RED


/* 
 * Windows specific color specifications for SetConsoleTextAttribute()
 */
#ifdef _WIN32
    #define FCOLOR_RED          FOREGROUND_RED
    #define FCOLOR_GREEN        FOREGROUND_GREEN
    #define FCOLOR_BLUE         FOREGROUND_BLUE  
    #define FCOLOR_YELLOW       FOREGROUND_RED | FOREGROUND_GREEN
    #define FCOLOR_GRAY         FOREGROUND_INTENSITY

    #define BCOLOR_RED          BACKGROUND_RED
    #define BCOLOR_GREEN        BACKGROUND_GREEN
    #define BCOLOR_BLUE         BACKGROUND_BLUE
    #define BCOLOR_GRAY         BACKGROUND_GRAY
    #define COLOR_CLEAR         FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
#endif

/* 
 * Bash color codes
 */
#ifdef __linux__
    #define FCOLOR_RED          "\e[31m"
    #define FCOLOR_GREEN        "\e[32m"
    #define FCOLOR_BLUE         "\e[34m"
    #define FCOLOR_YELLOW       "\e[1;33m"
    #define FCOLOR_GRAY         "\e[37m"
    
    #define BCOLOR_RED          "\e[41m"
    #define BCOLOR_GREEN        "\e[42m"
    #define BCOLOR_BLUE         "\e[44m"
    #define BCOLOR_GRAY         "\e[47m"

    #define COLOR_CLEAR         "\e[0m"
#endif

#endif
