#ifndef PARSE_CSV_H 
#define PARSE_CSV_H

#define ATM_EX_RATE_DATA_URL "https://www.eestipank.ee/valuutakursid/export/csv/latest"

/* 
 * Main structure for keeping all information related to one currency 
 * Currency value to Euro is not kept in IEEE 754 float because it is inaccurate
 * Instead decimal integer mantissa and value exponant are kept 
 */
typedef struct CurrencyInfo {
    char *full_name;
    char code[4];
    long mantissa;
    int val_exp;
} CurrencyInfo;


#ifdef __PARSE_CSV_C
    #include <stdlib.h>
    #include <stdint.h>
    #include <stdio.h>
    #include <string.h>
    #include <curl/curl.h>
    
    #define __ATM_DEFAULT_CURRENCY_C 32
    /* Read file contents to char buffer */
    char *__readFileToBuffer(char *file_name);
    /* Find total line count from buffer */
    size_t __lc(char *buf);
    /* Read n bytes from str and push the data to p_out_strs */
    void __pushSplitStr (
        char *str, 
        size_t n, 
        char ***p_out_strs, 
        size_t *p_out_str_c
    );
#endif


/* Get currency exchange rates from Eesti Pank */
void fetchExRates(char *out_file);

/* Parse csv currency information and create CurrencyInfo instance for every currency */
void parseCurrencyInfo (
    char* csv_file, 
    CurrencyInfo **p_cur_infos, 
    size_t *p_cur_info_c,
    char ***p_meta,
    size_t *p_meta_c
);

/* Extract date information from metadata */
char *metaExtractDate (
    char **meta, 
    size_t meta_c
);

#endif
