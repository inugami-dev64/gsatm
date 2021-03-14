#ifndef PARSE_CSV_H 
#define PARSE_CSV_H

#define ATM_EX_RATE_DATA_URL "https://www.eestipank.ee/valuutakursid/export/csv/latest"

#ifdef __FM_C 
    #include <stdlib.h>
    #include <stdint.h>
    #include <stdio.h>
    #include <string.h>
	#include <errno.h>
	#ifdef _WIN32
		#define CURL_STATICLIB
	#endif
    #include <curl/curl.h>

    typedef uint8_t bool;
    #define true 1
    #define false 0

	#include "lib_export.h"
    #include "err_def.h"
    #include "hashmap.h"
    #include "strparse.h"
    
    #define __ATM_DEFAULT_CURRENCY_C 32
    #define __ATM_DEFAULT_NOTE_C 8

    /* Read file contents to char buffer */
    static char *__readFileToBuffer(char *file_name);
#endif
/* For information about currency value related to Euros */
typedef struct SafeFloat {
    uint64_t mantissa;
    int val_exp;
} SafeFloat;

/* For information about each currency's cash reserve in ATM */
typedef struct CashStatus {
    uint64_t banknote_c;
    uint64_t *banknote_vals;
    int64_t *val_c;
} CashStatus;


/* 
 * Main structure for keeping all information related to one currency 
 * Currency value to Euro is not kept in IEEE 754 float because it is inaccurate
 * Instead decimal integer mantissa and value exponant are kept 
 */
typedef struct CurrencyInfo {
    char *code;
    char *name;
    SafeFloat ex;
    CashStatus cs;
} CurrencyInfo;

#ifdef __FM_C
    #include "algorithm.h"
#endif

/****************************************/
/****************************************/
/********* CSV PARSING FUNCTIONS ********/
/****************************************/
/****************************************/

#if !defined(__USER_C) || !defined(_WIN32)
	/* Get currency exchange rates from Eesti Pank */
	LIB_EXPORT void CALL_CON csv_FetchExRates(char *out_file, bool is_min);

	/* Parse csv currency information and create CurrencyInfo instance for every currency */
	LIB_EXPORT void CALL_CON csv_ParseCurrencyInfo (
		char* csv_file, 
		Hashmap *p_cur_map, 
		char ***p_codes,
		size_t *p_code_c,
		char ***p_meta,
		size_t *p_meta_c
	);

	/* Extract date information from metadata */
	LIB_EXPORT char* CALL_CON csv_MetaExtractDate (
		char **meta, 
		size_t meta_c
	);


	/**********************************************/
	/**********************************************/
	/******** CASH DATA PARSING FUNCTIONS *********/
	/**********************************************/
	/**********************************************/
	LIB_EXPORT void CALL_CON cash_ParseData (
		char *cash_file,
		Hashmap *p_cash_map,
		char ***p_codes,
		size_t *p_code_c
	);
#endif

#endif
