#define __USER_C
#include "atm.h"

#define bool uint8_t
#define true 1
#define false 0

static char **rt_codes = NULL;
static size_t rt_code_c = 0;

static char **ld_codes = NULL;
static size_t ld_code_c = 0;

void __initAtm() {
    initConverter();
    printf("Welcome to Goldstein Bank ATM!\n");
    csv_FetchExRates(ATM_EX_RATES_FILE);

    char **meta = NULL;
    size_t meta_c = 0;
    csv_ParseCurrencyInfo (
        ATM_EX_RATES_FILE,
        getCurrencyMap(), 
        &rt_codes,
        &rt_code_c,
        &meta,
        &meta_c
    );
    cash_ParseData (
        ATM_CASH_INFO_FILE, 
        getCurrencyMap(),
        &ld_codes,
        &ld_code_c
    );

    char *date = csv_MetaExtractDate(meta, meta_c);
    printf("Recieved exchange rates from: %s\n", date);

    // TODO: Free memory for meta data 
    for(size_t i = 0; i < meta_c; i++) {
        if(meta[i])
            free(meta[i]);
    }

    /*if(meta) */
        /*free(meta);*/
}


InputAction __inputParse(char *in, char ***p_in_params) {
    // TODO: Extract user parameters
    char *p_ins = in, *n_ins = in;
    char *words[512] = {0};
    size_t len[512] = {0};
    if(!in)
        return ATM_INPUT_ACTION_UNKNOWN;

    in[strlen(in) - 1] = 0x00;
    
    size_t i = 0;
    char *tmp = NULL;
    while((tmp = strchr(n_ins, 0x20)) || (tmp = strchr(n_ins, 0x00))) {
        n_ins = tmp;
        while(p_ins < n_ins && *p_ins == 0x20)
            p_ins++;

        n_ins++;
        if(p_ins >= n_ins)
            continue;

        words[i] = p_ins;
        len[i] = n_ins - p_ins - 1;
        p_ins = n_ins;
        i++;

        if(!(*n_ins) || !(*p_ins))
            break;
    }

    (*p_in_params) = (char**) calloc (
        i + 1,
        sizeof(char*)
    );

    memcpy (
        (*p_in_params),
        words,
        i + 1
    );
    
    // TODO: Check input actions
    if(i >= 1 && !strncmp(words[0], "exit", len[0]))
        return ATM_INPUT_ACTION_EXIT;
    if(i >= 1 && !strncmp(words[0], "help", len[0]))
        return ATM_INPUT_ACTION_HELP;
    if(i >= 1 && !strncmp(words[0], "list", len[0])) {
        if(i >= 2 && !strncmp(words[1], "rates", len[1])) {
            return ATM_INPUT_ACTION_LIST_RATES;
        }
        else if(i >= 2 && !strncmp(words[1], "status", len[1]))
            return ATM_INPUT_ACTION_LIST_STATUS;
        else 
            return ATM_INPUT_ACTION_UNKNOWN;
    }
    if(i >= 4 && !strncmp(words[1], "convert", len[1]) && atof(words[1]))
        return ATM_INPUT_ACTION_CONVERT;

    return ATM_INPUT_ACTION_UNKNOWN;
}


/* Get exchange rate in printable format */
char *__getPrintableExRate(long mantissa, int exp) {
    static char rate[32];
    memset(rate, 0, 32);
    sprintf (
        rate, 
        "%ld", 
        mantissa
    );
    
    char *tmp = rate + strlen(rate) - abs(exp);
    size_t dif;
    if(tmp > rate) {
        if(strlen(rate) + 1 < 32) {
            memmove(tmp + 1, tmp, strlen(tmp));
            *tmp = '.';
        }
        else MEM_CORRUPTION("Failed to print exchange rates");
    }

    else {
        // dif becomes negative exponant
        dif = rate - tmp + 1;
        if(strlen(rate) + dif + 1 < 32) {
            memmove(tmp + dif + 1, tmp, strlen(tmp));
            *(tmp) = '0';
            *(tmp + 1) = '.';
            for(size_t i = 0; i < dif - 1; i++)
                *(tmp + i + 1) = '0';
        }
        else MEM_CORRUPTION("Failed to print exchange rates");
    }

    return rate;
}


size_t __getReqSpaceIndent (
    Hashmap *p_hm,
    size_t tab_sp,
    size_t tab_from_max
) {
    size_t pos = 0;
    CurrencyInfo *p_ci = NULL;
    for(size_t i = 0; i < rt_code_c; i++) {
        p_ci = findValue(p_hm, rt_codes[i], 3);
        if(!p_ci) CUR_CODE_ERR(rt_codes[i]);

        if(strlen(p_ci->name) > pos)
            pos = strlen(p_ci->name);
    }

    pos += tab_sp - pos % tab_sp + tab_from_max * tab_sp;
    return pos;
}


char *__getTextPadding(char *text, size_t req_sp) {
    // TODO: Find correct padding for string from indetation tab count
    static char pad[128];
    memset(pad, 0, 128);

    size_t sp_c = req_sp - strlen(text);
    for(size_t i = 0; i < sp_c; i++)
        pad[i] = 0x20;

    return pad;
}


/**************************************/
/**************************************/
/******* User action functions ********/
/**************************************/
/**************************************/

void __listRates() {
    // TODO: Find correct tab count to display assuming that tab spacing is 4 spaces
    printf("Currency name | ISO4217 code | Exchange rate to Euros\n");
    size_t pos = __getReqSpaceIndent (
        getCurrencyMap(), 
        __ATM_INDENT_SPACE_C, 
        __ATM_TAB_FROM_MAX_NAME_LEN
    );
    char *pad;

    CurrencyInfo *p_ci = NULL;
    // TODO: print information about currency and its exchange rates
    for(size_t i = 0; i < rt_code_c; i++) {
        p_ci = findValue(getCurrencyMap(), rt_codes[i], 3);
        if(!p_ci) CUR_CODE_ERR(rt_codes[i]);

        char *rate = __getPrintableExRate(p_ci->ex.mantissa, p_ci->ex.val_exp);
        char *name = p_ci->name;
        pad = __getTextPadding (
            p_ci->name, 
            pos 
        );

        printf (
            "%s%s%s\t%s\n",
            name,
            pad,
            p_ci->code,
            rate
        );
    }
}


void __listStatus() {
    // TODO: List the cash availability in ATM
    size_t tab_c = __getReqSpaceIndent (
        getCurrencyMap(), 
        __ATM_INDENT_SPACE_C, 
        __ATM_TAB_FROM_MAX_NAME_LEN
    );

    char *pad;
    CurrencyInfo *p_ci;
    printf("Currency name | ISO4217 code | Banknote avilability ([VALUE] - [AMOUNT])\n");
    for(size_t i = 0; i < ld_code_c; i++) {
        p_ci = findValue (
            getCurrencyMap(),
            ld_codes[i],
            3
        );

        if(!p_ci->ex.mantissa || !p_ci->ex.val_exp)
            continue;

        pad = __getTextPadding (
            p_ci->name, 
            tab_c
        );
        printf (
            "%s%s%s\t", 
            p_ci->name, 
            pad, 
            p_ci->code
        );
        for(size_t j = 0; j < p_ci->cs.banknote_c; j++)
            printf("%d - %d; ", p_ci->cs.banknote_vals[j], p_ci->cs.val_c[j]);        

        printf("\n");
    }
}



void __inputPoll() {
    char buf[2048];

    // TODO: Poll input from end user
    while(true) {
        memset(buf, 0, 2048);
        printf("ATM> ");
        fflush(stdin);
        char *tmp = fgets(buf, 2048, stdin);
        if(!tmp) GEN_ERR("Failed to read user input");
        
        char **params = NULL;
        InputAction ia = __inputParse(buf, &params);

        switch(ia) 
        {
        case ATM_INPUT_ACTION_EXIT:
            exit(EXIT_SUCCESS);
            break;

        case ATM_INPUT_ACTION_HELP:
            printf("%s", main_help_c); 
            break;

        case ATM_INPUT_ACTION_LIST_RATES:
            __listRates();
            break;

        case ATM_INPUT_ACTION_LIST_STATUS:
            __listStatus();
            break;
        default:
            __ATM_UNKNOWN_COMMAND(buf);
            break;
        }
    }
}


int main() {
    __initAtm();
    __inputPoll();

    // TODO: Perform final cleanup
    CurrencyInfo *p_ci = NULL;
    for(size_t i = 0; i < ld_code_c; i++) {
        p_ci = findValue (
            getCurrencyMap(), 
            ld_codes[i], 
            3
        );

        free(p_ci->cs.banknote_vals);
        free(p_ci->cs.val_c);
        free(ld_codes[i]);
    }
    
    for(size_t i = 0; i < rt_code_c; i++)
        free(rt_codes[i]);
    
    free(ld_codes);
    free(rt_codes);
    
    destroyConverter();
    return EXIT_SUCCESS;
}
