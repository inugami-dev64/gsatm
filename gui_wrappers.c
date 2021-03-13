#include "gui_wrappers.h"


// All csv metadata will be stored here
static char **__meta = NULL;
static size_t __meta_c = 0;

// __rt_codes contains all currency codes that have exchange rate 
// to Euro available
static char **__rt_codes = NULL;
static size_t __rt_code_c = 0;

// __ld_codes contains all currency codes that have cash loaded
static char **__ld_codes = NULL;
static size_t __ld_code_c = 0;

/* Atm instance initialiser */
void atm_Init() {
    initConverter();
    csv_FetchExRates(ATM_EX_RATES_FILE);

    // Parse currency information fetched from Eesti Pank
    csv_ParseCurrencyInfo (
        ATM_EX_RATES_FILE,
        getCurrencyMap(),
        &__rt_codes,
        &__rt_code_c,
        &__meta,
        &__meta_c
    );
    
    // Parse cash information 
    cash_ParseData (
        ATM_CASH_INFO_FILE,
        getCurrencyMap(),
        &__ld_codes,
        &__ld_code_c
    );
}


/* Atm instance cleanup */
void atm_Cleanup() {
    CurrencyInfo *p_ci = NULL;
    for(size_t i = 0; i < __ld_code_c; i++) {
        p_ci = findValue (
            getCurrencyMap(), 
            __ld_codes[i], 
            3
        );

        free(p_ci->cs.banknote_vals);
        free(p_ci->cs.val_c);
        free(__ld_codes[i]);
    }
    
    for(size_t i = 0; i < __rt_code_c; i++)
        free(__rt_codes[i]);
    
    free(__ld_codes);
    free(__rt_codes);
    
    destroyConverter();
}


/* Extract date from metadata */
char *atm_GetExRateDate() {
    char *date = csv_MetaExtractDate(__meta, __meta_c);
    return date;
}


/* Convert currency from src to dst */
WithdrawReport atm_WithdrawCurrency (
    uint64_t amount, 
    char *src, 
    char *dst, 
    char *mode
) {
    // Find the correct WithdrawMode
    WithdrawMode cm = ATM_CONVERSION_MODE_MIN;
    str_ToUpperCase(mode, strlen(mode));
    str_ToUpperCase(src, strlen(src));
    str_ToUpperCase(dst, strlen(dst));

    WithdrawReport wr = {0};
    if(amount > ATM_CASH_HANDLING_LIMIT) {
        wr.error_code = 1;
        return wr;
    }

    if(!strcmp(mode, "MIN"))
        cm = ATM_CONVERSION_MODE_MIN;
    else if(!strcmp(mode, "MAX"))
        cm = ATM_CONVERSION_MODE_MAX;
    else if(!strcmp(mode, "DIF"))
        cm = ATM_CONVERSION_MODE_ALL_BILLS;

    // Currency validity check for safety reasons
    Hashmap *hashm = getCurrencyMap();
    CurrencyInfo *p_sci = NULL;
    CurrencyInfo *p_dci = NULL;
    if(strlen(src) != 3 || !(p_sci = (CurrencyInfo*) findValue(hashm, src, 3))) {
        wr.error_code = WDR_ERR_INVALID_SRC_CURRENCY_CODE;
        return wr;
    }

    else if(strlen(dst) != 3 || !(p_dci = (CurrencyInfo*) findValue(hashm, dst, 3))) {
        wr.error_code = WDR_ERR_INVALID_DST_CURRENCY_CODE;
        return wr;
    }

    wr = convertCurrency (
        amount,
        p_sci,
        p_dci,
        cm
    );

    return wr;
}


/* Get information about certain currency (exchange rate and cash status) */
CurrencyInfo atm_GetCurrencyInfo(char *code) {
    CurrencyInfo *p_ci;
    str_ToUpperCase(code, strlen(code));

    Hashmap *hashm = getCurrencyMap();
    if(strlen(code) != 3 || !(p_ci = (CurrencyInfo*) findValue(hashm, code, 3))) {
        CurrencyInfo ci = {0};
        return ci;
    }

    return *p_ci;
}


/* Convert from SafeFloat to regular IEEE754 float */
float atm_SafeFloatToFloat(SafeFloat sf) {
    char buf[64] = {0};
    sprintSafeFloat (
        NULL, 
        buf,
        sf
    );

    return atof(buf);
}
