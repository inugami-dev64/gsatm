#ifndef USER_H
#define USER_H

#define ATM_EX_RATES_FILE           "ex_rates.csv"
#define ATM_CASH_INFO_FILE          "cash.info"
#define ATM_CASH_HANDLING_LIMIT     10000000000

#ifdef __USER_C
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdint.h>
    #include <stddef.h>

    typedef uint8_t bool;
    #define true 1
    #define false 0

    #include "lib_export.h"
    #include "err_def.h"
    #include "hashmap.h"
    #include "fm.h"
    #include "algorithm.h"
    #include "converter.h"
    #include "strparse.h"


    typedef enum InputAction {
        ATM_INPUT_ACTION_EXIT                   = 0,
        ATM_INPUT_ACTION_HELP                   = 1,
        ATM_INPUT_ACTION_LIST_RATES             = 2,
        ATM_INPUT_ACTION_LIST_STATUS            = 3,
        ATM_INPUT_ACTION_CONVERT                = 4,
        ATM_INPUT_ACTION_UNKNOWN                = 5
    } InputAction;

    #define __ATM_UNKNOWN_COMMAND(x)        printf("Unknown command %s\n", x)
    #define __ATM_INDENT_SPACE_C            5
    #define __ATM_TAB_FROM_MAX_NAME_LEN     1     


    char *main_help_c =
    "List of available commands: \n\n" \
    " list\n" \
    "  -- rates -- List current currency rates\n" \
    "  -- status -- List current ATM cash status\n" \
    " convert [SUM] [CUR1] to [CUR2] -- Convert SUM of CUR1 currency to CUR2 currency\n" \
    "  -- max -- Convert using largest banknotes\n" \
    "  -- min -- Convert using smallest banknotes\n" \
    "  -- dif -- Convert using most different banknotes\n" \
    "  -- [NONE] -- Default conversion, using banknotes optimally\n" \
    " help -- Usage information about Goldstein Bank ATM\n" \
    " exit -- Exit Goldstein Bank ATM program\n";

    #ifdef _WIN32
		#include <windows.h>
		static HINSTANCE __lib_ins = NULL;
		void __loadDLL();

        // Function pointer declarations
        void(__stdcall *initConverter)();
        void(__stdcall* destroyConverter)();
        void(__stdcall *csv_FetchExRates)(char* ex_file, bool is_min);

        void(__stdcall *csv_ParseCurrencyInfo) (
            char *csv_file,
            Hashmap *p_cur_map,
            char ***p_codes,
            size_t *p_code_c,
            char ***p_meta,
            size_t *p_meta_c
		);

        void(__stdcall *cash_ParseData) (
            char *cash_file,
            Hashmap *p_cm,
            char ***p_codes,
            size_t *p_code_c
		);

        char* (__stdcall* csv_MetaExtractDate) (
            char **meta,
            size_t meta_c
		);
        
        void*(__stdcall* findValue) (
            Hashmap *p_hm,
            void *key,
            size_t key_len
		);

        Hashmap* (__stdcall *getCurrencyMap)();

        void(__stdcall* sprintSafeFloat) (
            char *fl_format,
            char *str,
            SafeFloat sf
		);

        void(__stdcall* str_ToUpperCase) (
            char* str,
            size_t str_len
		);

        WithdrawReport(__stdcall* convertCurrency) (
            uint64_t amount,
            CurrencyInfo *p_src,
            CurrencyInfo *p_dst,
            WithdrawMode cm
		);

        uint64_t(__stdcall* al_pow) (
            uint64_t base,
            uint64_t exp
		);



    #endif 
    /* 
     * Initialise the atm for usage 
     */
    void __initAtm();


    /* Poll user input */
    void __inputPoll();


    /* Parse user input and return parameters */ 
    InputAction __inputParse (
        char *in, 
        char ***p_in_params,
        size_t *p_in_c
    );


    /* List all available exchange rates */
    void __listRates();


    /* List current cash status */
    void __listStatus();


    /* Currency converter wrapper */
    void __convertCurrency(char **params, size_t param_c);
    

    /*
     * Find the required space indent to satisfy indent between certain tab point
     */
    size_t __getReqSpaceIndent(
        Hashmap *p_hm,
        size_t tab_size,
        size_t tab_from_max
    );


    /* 
     * Get indetation string from tab count 
     */
    char *__getTextPadding (
        char *text, 
        size_t req_sp 
    );
#endif

#endif
