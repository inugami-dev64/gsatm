#define __CONVERTER_C
#include "converter.h"

LIB_EXPORT void CALL_CON initConverter() {
    // TODO: Initialise hashmap for ISO 4217 currency codes
    newHashmap (
        &cur_map, 
        (int) 2 * ARR_LEN(iso4217)
    );
    
    cis = (CurrencyInfo*) calloc (
        ARR_LEN(iso4217),
        sizeof(CurrencyInfo)
    );
    for(size_t i = 0; i < ARR_LEN(iso4217); i++) {
        cis[i].code = iso4217[i];
        cis[i].name = cur_names[i];
        if(!strcmp(cis[i].code, "EUR")) {
            cis[i].ex.mantissa = 10000;
            cis[i].ex.val_exp = -4;
        }

        pushToHashmap (
            &cur_map, 
            iso4217[i],
            3,
            &cis[i]
        );
    }
}


/* 
 * Create printable string from SafeFloat 
 * str must be large enough for SafeFloat
 */
LIB_EXPORT void CALL_CON sprintSafeFloat (
    char *fl_format, 
    char *str, 
    SafeFloat sf
) {
    int frac_accuracy = sf.val_exp < 0 ? abs(sf.val_exp) : 1; 

    // TODO: Perform quick format parsing
    if(fl_format && strlen(fl_format) >= 4) {
        char fbuf[64] = {0};
        if (
            fl_format[0] == '%' && 
            fl_format[1] == '.' && 
            fl_format[strlen(fl_format) - 1] == 'f'
        ) {
            strncpy(fbuf, fl_format + 2, strlen(fl_format) - 3);
            frac_accuracy = atoi(fbuf);
        }
    }

    #if defined(__linux__)
        sprintf (
            str,
            "%ld",
            sf.mantissa
        );
    #elif defined(_WIN32)
        sprintf (
            str,
            "%lld",
            sf.mantissa
        );
    #endif 

    // Negative value exponant
    char *ptr;
    if(sf.val_exp < 0) {
        ptr = str + strlen(str) + sf.val_exp; 
        // SafeFloat exponant is smaller than mantissa's decimal scientific notation exponant
        if(ptr > str) {
            // TODO: Shift string bytes to right by one
            if(*ptr != 0x00)
                memmove(ptr + 1, ptr, strlen(ptr));
            else *(ptr + 1) = '0';
            *ptr = '.';
        }
        
        else {
            size_t mantissa_len = strlen(str);
            ptr = str;
            size_t shift_c = abs(sf.val_exp) - mantissa_len + 2;
            memmove(ptr + shift_c, ptr, strlen(ptr));
            *ptr = '0';
            *(ptr + 1) = '.';

            for(size_t i = 0; i < shift_c - 2; i++)
                *(ptr + i + 2) = '0';
        }
    }

    // Positive exponant 
    else {
        ptr = strlen(str) + str;       
        size_t zero_c = sf.val_exp;
        for(size_t i = 0; i < zero_c; i++)
            *(ptr + i) = '0';
    }

    // Perform fraction accuracy adjustments
    ptr = strchr(str, '.');
    if(!ptr) {
        ptr = str + strlen(str);
        *ptr = '.';
        ptr++;
        for(size_t i = 0; i < frac_accuracy; i++, ptr++)
            *ptr = '0';
    }

    else if(strlen(ptr + 1) < frac_accuracy) {
        size_t limit = frac_accuracy - strlen(ptr + 1);
        ptr = str + strlen(str);  
        for(size_t i = 0; i < limit; i++, ptr++)
            *ptr = '0';
    }

    else if(strlen(ptr + 1) > frac_accuracy) {
        ptr += frac_accuracy + 1;
        while(*ptr) {
            *ptr = 0x00;
            ptr++;
        }   
    }
}


/* Convert currency from p_src to p_dst and change cash status */
LIB_EXPORT WithdrawReport CALL_CON convertCurrency ( 
    uint64_t amount,
    CurrencyInfo *p_src,
    CurrencyInfo *p_dst,
    WithdrawMode cm
) {
    WithdrawReport wr = {0};

    // Check if source currency is correct 
    if(!p_src){
        wr.error_code = WDR_ERR_INVALID_SRC_CURRENCY_CODE;
        return wr;
    }

    // Check if destination currency is correnct
    if(!p_dst) {
        wr.error_code = WDR_ERR_INVALID_DST_CURRENCY_CODE;
        return wr;
    }

    
    // Make sure that exponants are even (bigger exponant is taken into count)
    int exp;
    int src_exp = p_src->ex.val_exp;
    int dst_exp = p_dst->ex.val_exp;
    SafeFloat src_ex = p_src->ex;
    SafeFloat dst_ex = p_dst->ex;

    exp = src_exp;
    if(abs(src_exp) > abs(dst_exp)) {
        dst_ex.val_exp = exp;
        dst_ex.mantissa *= al_pow(10, abs(src_exp) - abs(dst_exp));
    }

    else if(abs(dst_exp) > abs(src_exp)) {
        exp = dst_exp;
        src_ex.val_exp = exp;
        src_ex.mantissa *= al_pow(10, abs(dst_exp) - abs(src_exp));
    }


    uint64_t pr_amount = amount;
    amount *= al_pow(10, abs(exp));

    SafeFloat cn_value;
    cn_value.mantissa = (uint64_t) (((double) amount / (double) p_src->ex.mantissa) * (double) p_dst->ex.mantissa);
    cn_value.val_exp = exp;

    char buf[64] = { 0 };
    sprintSafeFloat("%.4f", buf, cn_value);
    printf (
        "%ld %s is about %s %s\n", 
        pr_amount, 
        p_src->code, 
        buf, 
        p_dst->code
    );

    // Verify that at least 90% of destination currency is available
    // Otherwise seg fault can happen
    uint64_t dst_c = 0;
    for(uint64_t i = 0; i < p_dst->cs.banknote_c; i++)
        dst_c += p_dst->cs.banknote_vals[i] * p_dst->cs.val_c[i];

    if(amount > ATM_CASH_HANDLING_LIMIT) {
        wr.error_code = WDR_ERR_CASH_HANDLING_LIMIT_REACHED;
        return wr;
    }

    // Check if ATM even has the amount of money needed for withdrawal
    dst_c *= al_pow(10, abs(exp));
    if(dst_c * ATM_MIN_WITHDRAWAL_CASH / 100 < cn_value.mantissa) {
        wr.error_code = WDR_ERR_NOT_ENOUGH_CASH;
        return wr;
    }

    // Start converting the currency
    switch(cm)
    {
    // Give out the maximum amount of bills
    case ATM_CONVERSION_MODE_MAX: 
        wr = al_WithdrawMaxBillsC(&cn_value, p_dst);
        break;

    // Give out minimum amount of bills
    case ATM_CONVERSION_MODE_MIN:
        wr = al_WithdrawMinBillsC(&cn_value, p_dst);
        break;

    case ATM_CONVERSION_MODE_ALL_BILLS:
        wr = al_WithdrawDifBillsC(&cn_value, p_dst);
        break;

    default: 
        break;
    }

    return wr;
}


LIB_EXPORT void CALL_CON destroyConverter() {
    free(cis);
    free(cur_map.map_data);
}


LIB_EXPORT Hashmap* CALL_CON getCurrencyMap() {
    return &cur_map;
}
