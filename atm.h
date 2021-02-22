#ifndef USER_H
#define USER_H

#define ATM_EX_RATES_FILE   "ex_rates.info"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifdef __USER_C
    typedef enum InputAction {
        ATM_INPUT_ACTION_EXIT                   = 0,
        ATM_INPUT_ACTION_HELP                   = 1,
        ATM_INPUT_ACTION_LIST_RATES             = 2,
        ATM_INPUT_ACTION_LIST_STATUS            = 3,
        ATM_INPUT_ACTION_CONVERT                = 4,
        ATM_INPUT_ACTION_UNKNOWN                = 5
    } InputAction;

    #define ATM_UNKNOWN_COMMAND(x)     printf("Unknown command %s\n", x)
    char *main_help_c =
    "List of available commands: \n\n" \
    " list\n" \
    "  -- rates -- List current currency rates\n" \
    "  -- status -- List current ATM cash status\n" \
    " convert [SUM] [CUR1] to [CUR2] -- Convert SUM of CUR1 currency to CUR2 currency\n" \
    " help -- Usage information about Goldstein Bank ATM\n" \
    " exit -- Exit Goldstein Bank ATM program\n";

    /* Initialise the atm for usage */
    void __initAtm();
    /* Poll user input */
    void __inputPoll();
    /* Parse user input and return parameters */ 
    InputAction __inputParse(char *in, char ***p_in_params);
#endif

#endif
