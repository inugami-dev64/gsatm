#ifndef __GUI_WRAPPERS_H
#define __GUI_WRAPPERS_H
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef uint8_t bool;
#define true 1
#define false 0

#include "lib_export.h"
#include "strparse.h"
#include "hashmap.h"
#include "fm.h"
#include "algorithm.h"
#include "converter.h"

#define ATM_EX_RATES_FILE           "ex_rates.csv"
#define ATM_CASH_INFO_FILE          "cash.info"


/*
 * Initialise new atm instance 
 * This function fetches currency data from Eesti Pank and reads cash information
 * from file cash.info
 */
void atm_Init();


/*
 * Perform final cleanup for the atm program
 */
void atm_Cleanup();


/*
 * Get information about the date exchange info is from
 */
char *atm_GetExRateDate();


/*
 * Convert amount of src currency to dst currency and 
 * get withdraw report for it. Mode can be the following:
 * "max", "min", "dif" (case insensitive) to withdraw maximum amount, 
 * minumum amount and the most amount of different banknotes respectively
 */
WithdrawReport atm_WithdrawCurrency (
    uint64_t amount, 
    char *src, 
    char *dst,
    char *mode
);


/*
 * Get information about certain currency
 * Currency code is case insensitive
 * If error occurs currencyinfo parameters value will be null;
 */
CurrencyInfo atm_GetCurrencyInfo(char *code);


/*
 * Convert from SafeFloat to regular IEEE754 float
 */
float atm_SafeFloatToFloat(SafeFloat sf);



#endif
