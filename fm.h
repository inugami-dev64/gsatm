#ifndef PARSE_CSV_H 
#define PARSE_CSV_H

#define ATM_EX_RATE_DATA_URL "https://www.eestipank.ee/valuutakursid/export/csv/latest"

/* 
 * Main structure for keeping all information related to one currency 
 * Currency value to Euro is not kept in IEEE 754 float because it is inaccurate
 * Instead decimal integer mantissa and value exponant are kept 
 */


#ifdef __FM_C 
    #include <stdlib.h>
    #include <stdint.h>
    #include <stdio.h>
    #include <string.h>
    #include <curl/curl.h>
    #include "err_def.h"
    #include "hashmap.h"
    
    #define __ATM_DEFAULT_CURRENCY_C 32
    #define __ATM_DEFAULT_NOTE_C 8

    #define bool uint8_t
    #define true 1
    #define false 0
    /* Read file contents to char buffer */
    static char *__readFileToBuffer(char *file_name);
    /* Find total line count from buffer */
    static size_t __lc(char *buf);
    /* Read n bytes from str and push the data to p_out_strs */
    static void __pushSplitStr (
        char *str, 
        size_t n, 
        char ***p_out_strs, 
        size_t *p_out_str_c
    );
    /* 
     * Check if string only contains uppercase or lowercase alphabetical characters 
     * O(n)
     */
    static bool __isAlphabetical (
        char *str, 
        size_t len
    );
    /* Make all characters from string uppercase */
    static void __toUpperCase (
        char *str,
        size_t len
    );
    /* Remove all line comments */
    static void __rmLineComments (
        char cm_sym, 
        char *buf
    );
#endif
/* For information about currency value related to Euros */
typedef struct ExchangeInfo {
    uint32_t mantissa;
    int val_exp;
} ExchangeInfo;

/* For information about each currency's cash reserve in ATM */
typedef struct CashStatus {
    uint32_t banknote_c;
    uint32_t *banknote_vals;
    uint32_t *val_c;
} CashStatus;

/* Contains information about each currency */
typedef struct CurrencyInfo {
    char *code;
    char *name;
    ExchangeInfo ex;
    CashStatus cs;
} CurrencyInfo;


/****************************************/
/****************************************/
/********* CSV PARSING FUNCTIONS ********/
/****************************************/
/****************************************/

/* Get currency exchange rates from Eesti Pank */
void csv_FetchExRates(char *out_file);
/* Parse csv currency information and create CurrencyInfo instance for every currency */
void csv_ParseCurrencyInfo (
    char* csv_file, 
    Hashmap *p_cur_map, 
    char ***p_codes,
    size_t *p_code_c,
    char ***p_meta,
    size_t *p_meta_c
);
/* Extract date information from metadata */
char *csv_MetaExtractDate (
    char **meta, 
    size_t meta_c
);


/**********************************************/
/**********************************************/
/******** CASH DATA PARSING FUNCTIONS *********/
/**********************************************/
/**********************************************/
void cash_ParseData (
    char *cash_file,
    Hashmap *p_cash_map,
    char ***p_codes,
    size_t *p_code_c
);

#endif
