#ifndef USER_H
#define USER_H

#define ATM_EX_RATES_FILE   "ex_rates.csv"
#define ATM_CASH_INFO_FILE  "cash.info"

#ifdef __USER_C
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdint.h>
    #include <stddef.h>
    #include "err_def.h"
    #include "hashmap.h"
    #include "converter.h"
    #include "fm.h"
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
    "  -- [NONE] -- Default conversion, using banknotes optimally\n" \
    " help -- Usage information about Goldstein Bank ATM\n" \
    " exit -- Exit Goldstein Bank ATM program\n";

    /* Initialise the atm for usage */
    void __initAtm();
    /* Poll user input */
    void __inputPoll();
    /* Get the minimal tab count to display names from */
    size_t __getReqSpaceIndent(
        Hashmap *p_hm,
        size_t tab_size,
        size_t tab_from_max
    );
    /* Get indetation string from tab count */
    char *__getTextPadding (
        char *text, 
        size_t req_sp 
    );
    /* Get exchange rate in printable format */
    char *__getPrintableExRate(long mantissa, int exp);
    /* Parse user input and return parameters */ 
    InputAction __inputParse(char *in, char ***p_in_params);

    /* List all available exchange rates */
    void __listRates();
    /* List current cash status */
    void __listStatus();
#endif

#endif
