#define __USER_C
#include "atm.h"

static char **rt_codes = NULL;
static size_t rt_code_c = 0;

static char **ld_codes = NULL;
static size_t ld_code_c = 0;

static bool __is_basic = false;

#ifdef _WIN32
    /*
     * Set the console std handle for colorful text
     */
    void __setStdHandle() {
        __std_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    }


	/*
	 * Load DLL and all pointers to functions that are used
	 */
	void __loadDLL() {
        __lib_ins = LoadLibrary("libatm.dll");
        if (!__lib_ins) {
            fprintf(stderr, "Could not load libatm.dll\n");
            exit(EXIT_FAILURE);
        }

        // Load all library functions that are used
        initConverter = (void(__stdcall*)())GetProcAddress(__lib_ins, "initConverter");
        destroyConverter = (void(__stdcall*)())GetProcAddress(__lib_ins, "destroyConverter");
        csv_FetchExRates = (void(__stdcall*)(char*, bool))GetProcAddress(__lib_ins, "csv_FetchExRates");

        csv_ParseCurrencyInfo = (void(__stdcall*)(char*, Hashmap*, char***, size_t*, char***, size_t*)) GetProcAddress(
            __lib_ins,
            "csv_ParseCurrencyInfo"
        );

        cash_ParseData = (void(__stdcall*)(char*, Hashmap*, char***, size_t*)) GetProcAddress (
            __lib_ins,
            "cash_ParseData"
        );

        csv_MetaExtractDate = (char* (__stdcall*)(char**, size_t)) GetProcAddress (
            __lib_ins,
            "csv_MetaExtractDate"
        );

        findValue = (void* (__stdcall*)(Hashmap*, void*, size_t)) GetProcAddress (
            __lib_ins,
            "findValue"
        );

        getCurrencyMap = (Hashmap* (__stdcall*)()) GetProcAddress (
            __lib_ins,
            "getCurrencyMap"
        );

        
        sprintSafeFloat = (void(__stdcall*)(char*, char*, SafeFloat)) GetProcAddress (
            __lib_ins,
            "sprintSafeFloat"
        );

        str_ToUpperCase = (void(__stdcall*)(char*, size_t)) GetProcAddress (
            __lib_ins,
            "str_ToUpperCase"
        );

        convertCurrency = (WithdrawReport(__stdcall*)(uint64_t, CurrencyInfo*, CurrencyInfo*, WithdrawMode)) GetProcAddress (
            __lib_ins,
            "convertCurrency"
        );

        al_pow = (uint64_t(__stdcall*)()) GetProcAddress (
            __lib_ins,
            "al_pow"
        );
	}
#endif

void __initAtm() {
    initConverter();
    if(!__is_basic) {
        const char *welcome_msg = "Welcome to Goldstein Bank ATM!";
        #if defined(COLORISE) && defined(__linux__)
            printf (
                "%s%s%s\n", 
                WELCOME_COLOR, 
                welcome_msg, 
                COLOR_CLEAR
            );
        #elif defined(COLORISE) && defined(_WIN32)
            SetConsoleTextAttribute(__std_handle, WELCOME_COLOR);
            printf("%s\n", welcome_msg);
            SetConsoleTextAttribute(__std_handle, COLOR_CLEAR);
        #endif
    }
    csv_FetchExRates(ATM_EX_RATES_FILE, __is_basic);


    char **meta = NULL;
    size_t meta_c = 0;

    // Parse currency information fetched from Eesti Pank
    csv_ParseCurrencyInfo (
        ATM_EX_RATES_FILE,
        getCurrencyMap(), 
        &rt_codes,
        &rt_code_c,
        &meta,
        &meta_c
    );

    // Parse currency cash data information 
    cash_ParseData (
        ATM_CASH_INFO_FILE, 
        getCurrencyMap(),
        &ld_codes,
        &ld_code_c
    );

    char *date = csv_MetaExtractDate(meta, meta_c);
    if(!__is_basic)
        printf("Recieved exchange rates from: %s\n", date);

    // TODO: Free memory for meta data 
    for(size_t i = 0; i < meta_c; i++) {
        if(meta[i])
            free(meta[i]);
    }
}


/*
 * Parse user input and return InputAction value for it
 */
InputAction __inputParse (
    char *in, 
    char ***p_in_params, 
    size_t *p_in_c
) {
    // TODO: Extract user parameters
    char *p_ins = in, *n_ins = in;
    static char *words[512] = {0};
    size_t len[512] = {0};
    if(!in)
        return ATM_INPUT_ACTION_UNKNOWN;

    in[strlen(in) - 1] = 0x00;
    
    size_t i = 0;
    char *tmp = NULL;
    while((tmp = strchr(n_ins, 0x20)) || (tmp = strchr(n_ins, 0x00))) {
        *tmp = 0x00;
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


    (*p_in_c) = i;
    (*p_in_params) = words;
    
    // TODO: Check input actions
    if(i == 1 && !strncmp(words[0], "exit", len[0]))
        return ATM_INPUT_ACTION_EXIT;
    if(i == 1 && !strncmp(words[0], "help", len[0]))
        return ATM_INPUT_ACTION_HELP;
    if(i >= 1 && !strncmp(words[0], "list", len[0])) {
        if(i == 2 && !strncmp(words[1], "rates", len[1])) {
            return ATM_INPUT_ACTION_LIST_RATES;
        }
        else if(i == 2 && !strncmp(words[1], "status", len[1]))
            return ATM_INPUT_ACTION_LIST_STATUS;
        else 
            return ATM_INPUT_ACTION_UNKNOWN;
    }


    if(i >= 5 && !strncmp(words[0], "convert", len[0]) && atoi(words[1])) {
        if(strlen(words[2]) == 3 && strlen(words[4]) == 3 && !strcmp(words[3], "to"))
            return ATM_INPUT_ACTION_CONVERT;
    }

    return ATM_INPUT_ACTION_UNKNOWN;
}


size_t __getReqSpaceIndent (
    Hashmap *p_hm,
    size_t tab_sp,
    size_t tab_from_max
) {
    size_t pos = 0;
    CurrencyInfo *p_ci = NULL;
    for(size_t i = 0; i < rt_code_c; i++) {
        p_ci = (CurrencyInfo*) findValue(p_hm, rt_codes[i], 3);
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
/******* User action functions ********/
/**************************************/

void __listRates() {
    // TODO: Find correct tab count to display assuming that tab spacing is 4 spaces
    if(!__is_basic)
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

        char rate[64] = {0};
        sprintSafeFloat("%.4f", rate, p_ci->ex);
        char *name = p_ci->name;
        pad = __getTextPadding (
            p_ci->name, 
            pos 
        );

        if(!__is_basic) {
            #if defined(COLORISE) && defined(__linux__)
                printf (
                    "%s%s%s%s%s\t%s\n",
                    name,
                    pad,
                    CURRENCY_COLOR,
                    p_ci->code,
                    COLOR_CLEAR,
                    rate
                );
            #elif defined(COLORISE) && defined(_WIN32)
                printf (
                    "%s%s",
                    name,
                    pad
                );
                SetConsoleTextAttribute(__std_handle, CURRENCY_COLOR);
                printf (
                    "%s",
                    p_ci->code
                );
                SetConsoleTextAttribute(__std_handle, COLOR_CLEAR);
                printf (
                    "\t%s\n",
                    rate
                );
            #else
                printf (
                    "%s%s%s\t%s\n",
                    name,
                    pad,
                    p_ci->code,
                    rate
                );
            #endif
        }
        else {
            printf (
                "\"%s\" %s %s\n",
                name,
                p_ci->code,
                rate
            );
        }
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
    if(!__is_basic)
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
        if(!__is_basic) {
            #if defined(COLORISE) && defined(__linux__)
                printf (
                    "%s%s%s%s%s\t", 
                    p_ci->name, 
                    pad, 
                    CURRENCY_COLOR,
                    p_ci->code,
                    COLOR_CLEAR
                );
            #elif defined(COLORISE) && defined(_WIN32)
                printf (
                    "%s%s",
                    p_ci->name,
                    pad
                );
                SetConsoleTextAttribute(__std_handle, CURRENCY_COLOR);
                printf (
                    "%s",
                    p_ci->code
                );
                SetConsoleTextAttribute(__std_handle, COLOR_CLEAR);
                printf("\t");
            #else 
                printf (
                    "%s%s%s\t", 
                    p_ci->name,
                    pad,
                    p_ci->code
                );
            #endif
        }
        else {
            printf (
                "\"%s\" %s ", 
                p_ci->name,
                p_ci->code
            );
        }

        for(size_t j = 0; j < p_ci->cs.banknote_c; j++)
            printf("%ld - %ld; ", p_ci->cs.banknote_vals[j], p_ci->cs.val_c[j]);        

        printf("\n");
    }
}


/* Cli wrapper for currency conversion */
void __convertCurrency (
    char **params,
    size_t param_c
) {
    WithdrawMode cm = ATM_CONVERSION_MODE_MIN;
    if(param_c == 6) {
		str_ToUpperCase(params[5], strlen(params[5]));
        if(!strcmp(params[5], "MAX"))
            cm = ATM_CONVERSION_MODE_MAX;
        else if(!strcmp(params[5], "MIN"))
            cm = ATM_CONVERSION_MODE_MIN;
        else if(!strcmp(params[5], "DIF"))
            cm = ATM_CONVERSION_MODE_ALL_BILLS;
    }
    else cm = ATM_CONVERSION_MODE_MIN;

    uint64_t amount = atol(params[1]);
    str_ToUpperCase(params[2], strlen(params[2]));
    str_ToUpperCase(params[4], strlen(params[4]));

    Hashmap *hashm = getCurrencyMap();
    CurrencyInfo *p_src = (CurrencyInfo*) findValue (
        hashm,
        params[2],
        3
    );

    CurrencyInfo *p_dst = (CurrencyInfo*) findValue (
        hashm,
        params[4],
        3
    );

    WithdrawReport wr = convertCurrency (
        amount,
        p_src,
        p_dst,
        cm
    );


    // Error code handling
    bool is_err = false;
    char *msg = NULL;
    switch(wr.error_code)
    {
    case WDR_ERR_NOT_ENOUGH_CASH:
        msg = "Not enough cash to withdraw from atm";
        is_err = true;
        break;

    case WDR_ERR_CASH_HANDLING_LIMIT_REACHED:
        msg = "You are trying to withdraw more cash than allowed";
        is_err = true;
        break;

    case WDR_ERR_INVALID_SRC_CURRENCY_CODE:
        msg = "Invalid source currency code";
        is_err = true;
        break;

    case WDR_ERR_INVALID_DST_CURRENCY_CODE:
        msg = "Invalid destination currency code";
        is_err = true;
        break;

    default:
        break;
    }

    if(is_err && !__is_basic) {
        #if defined(COLORISE) && defined(__linux__)
            fprintf (
                stderr, 
                "%s%s%s\n",
                ERR_COLOR,
                msg,
                COLOR_CLEAR
            );
        #elif defined(COLORISE) && defined(_WIN32)
            SetConsoleTextAttribute(__std_handle, ERR_COLOR);
            fprintf (
                stderr,
                "%s\n",
                msg
            );
            SetConsoleTextAttribute(__std_handle, COLOR_CLEAR);
        #else 
            fprintf(stderr, "%s\n", msg);
        #endif

    }
    else if(is_err) fprintf(stderr, "%s\n", msg);

    // Print money withdrawal information 
    if(!__is_basic)
		printf("Money withdrawal report\nRecieved bills | Quantity\n");

    for(size_t i = 0; i < wr.cs.banknote_c; i++) {
        if(wr.cs.banknote_vals[i] && wr.cs.val_c[i]) {
            if(!__is_basic) {
                #if defined(COLORISE) && defined(__linux__)
                    printf (
                        "%s%ld%s %s%s%s | %ld\n", 
                        WITHDRAW_COLOR,
                        wr.cs.banknote_vals[i], 
                        COLOR_CLEAR,
                        CURRENCY_COLOR,
                        p_dst->code,
                        COLOR_CLEAR,
                        wr.cs.val_c[i]
                    );
                #elif defined(COLORISE) && defined(_WIN32)
                    SetConsoleTextAttribute(__std_handle, WITHDRAW_COLOR);
                    printf("%ld", wr.cs.banknote_vals[i]);
                    SetConsoleTextAttribute(__std_handle, COLOR_CLEAR);
                    printf(" ");
                    SetConsoleTextAttribute(__std_handle, CURRENCY_COLOR);
                    printf("%s", p_dst->code);
                    SetConsoleTextAttribute(__std_handle, COLOR_CLEAR);
                    printf(" | %ld\n", wr.cs.val_c[i]);
                #else 
                    printf (
                        "%ld %s | %ld\n", 
                        wr.cs.banknote_vals[i], 
                        p_dst->code,
                        wr.cs.val_c[i]
                    );
                #endif
            }

            else {
                printf (
                    "%ld %s %ld\n", 
                    wr.cs.banknote_vals[i], 
                    p_dst->code,
                    wr.cs.val_c[i]
                );
            }
        }
    }


    SafeFloat src_unex_fl = wr.unexchanged;
    src_unex_fl.mantissa = (uint64_t) (
        ((float) wr.unexchanged.mantissa / (float) p_dst->ex.mantissa) * 
        (float) p_src->ex.mantissa
    );

    // Check if rounding needs to be done
    amount *= al_pow(10, abs(src_unex_fl.val_exp));
    if(amount - src_unex_fl.mantissa == 1)
        src_unex_fl.mantissa++;

    char src_unex[64] = {0};
    char dst_unex[64] = {0};
    sprintSafeFloat("%.4f", dst_unex, wr.unexchanged);
    sprintSafeFloat("%.4f", src_unex, src_unex_fl);
    if (!__is_basic) {
        printf (
            "Unexchanged money: %s %s / %s %s\n",
            dst_unex,
            p_dst->code,
            src_unex,
            p_src->code
        );
    }
    else {
        printf (
            "REM: %s %s / %s %s\n",
            dst_unex,
            p_dst->code,
            src_unex,
            p_src->code
        );
    }

    // Cleanup WithdrawReport
    free(wr.cs.banknote_vals);
    free(wr.cs.val_c);
}


/* Wait for user input */
void __inputPoll() {
    // input buffer can theorethically overflow
    char buf[2048];

    // TODO: Poll input from end user
    while(true) {
        memset(buf, 0, 2048);
        if(!__is_basic) {
            const char *msg = "ATM> ";
            #if defined(COLORISE) && defined(__linux__)
                printf("%s%s%s", INPUT_POINT_COLOR, msg, COLOR_CLEAR);
            #elif defined(COLORISE) && defined(_WIN32)
                SetConsoleTextAttribute(__std_handle, INPUT_POINT_COLOR);
                printf("%s", msg);
                SetConsoleTextAttribute(__std_handle, COLOR_CLEAR);
            #else
                printf("%s", msg);
            #endif
            fflush(stdin);
        }
        
        char *tmp = fgets(buf, 2048, stdin);
        if(!tmp) GEN_ERR("Failed to read user input");
        
        char **params = NULL;
        size_t param_c = 0;
        InputAction ia = __inputParse(buf, &params, &param_c);

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

        case ATM_INPUT_ACTION_CONVERT: {
            __convertCurrency(params, param_c);
            break;
       }

        default:
            __ATM_UNKNOWN_COMMAND(buf);
            break;
        }

        if (__is_basic)
            return;
    }
}


int main(int argc, char *argv[]) {
    if (argc == 2 && !strcmp(argv[1], "basic"))
        __is_basic = true;
    else __is_basic = false;

    // WIN32 exclusive function calls
    #ifdef _WIN32
        __setStdHandle();
		__loadDLL();
	#endif

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
