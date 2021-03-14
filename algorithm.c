#define __ALGORITHM_C
#include "algorithm.h"

/*
 * The simplest searching algorithm known to a man
 */
static size_t __al_linsearch (
    void *arr, 
    size_t n, 
    size_t size, 
    void *cmp
) {
    size_t i;
    bool is_found = false;
    for(i = 0; i < n; i++) {
        if(!memcmp((char*) arr + i * size, cmp, size)) {
            is_found = true;
            break;
        }
    }

    if(!is_found)
        i = ULONG_MAX;

    return i;
}

/*
 * This function attempts to fill all the missing sum 
 * cash caps with smaller bills using greedy algorithm, since the minimal bill count
 * doesn't matter in this case anyways
 */
static void __al_FillCashCaps (
    CashStatus *p_ch,
    CashStatus *p_cur,
    size_t sum,
    size_t beg
) {
    uint64_t c_val_c;
    for(int64_t i = beg; i >= 0; i--) {
        c_val_c = p_cur->val_c[i];
        if(sum < p_cur->banknote_vals[i] || !c_val_c)
            continue;

        while(sum >= p_cur->banknote_vals[i] && c_val_c > 0) {
            p_ch->val_c[i]++;
            sum -= p_cur->banknote_vals[i];
            c_val_c--;
        } 
    }
}


static void __al_CashValMergeSort (
    CashStatus *p_in, 
    size_t beg, 
    size_t end, 
    CashStatus *p_tmp
) {
    // "Sorted"
    if(end - beg <= 1)
        return;

    size_t middle = (end + beg) / 2;

    // Split sorting task into two and swap p_in with p_tmp
    __al_CashValMergeSort(p_tmp, beg, middle, p_in);
    __al_CashValMergeSort(p_tmp, middle, end, p_in);

    size_t i, j, k;
    i = beg;
    j = middle;

    for(k = beg; k < end; k++) {
        if(i < middle && (j >= end || p_tmp->banknote_vals[i] <= p_tmp->banknote_vals[j])) {
            p_in->banknote_vals[k] = p_tmp->banknote_vals[i];
            p_in->val_c[k] = p_tmp->val_c[i];
            i++;
        }
        else {
            p_in->banknote_vals[k] = p_tmp->banknote_vals[j];
            p_in->val_c[k] = p_tmp->val_c[j];
            j++;
        }
    }
}


static uint16_t __al_FindMinCashBase10Exp (
    CashStatus *p_cs, 
    uint64_t beg, 
    uint64_t end
) {
    if(p_cs->banknote_vals[beg] % 10)
        return 0;

    uint16_t exp = 0;
    uint64_t tmp_val = p_cs->banknote_vals[beg];
    if(end - beg <= 1) {
        while(!(tmp_val % 10)) {
            tmp_val /= 10;
            exp++;
        }
    }

    else {
        uint16_t middle = (end + beg) / 2;
        uint16_t left = __al_FindMinCashBase10Exp(p_cs, beg, middle);
        uint16_t right = __al_FindMinCashBase10Exp(p_cs, middle, end);

        return left < right ? left : right;
    }
    return exp;
}


/*
 * Find the smallest needed amount of banknotes (quantities are ignored) 
 */
uint64_t __al_ChangeFinder (
    SafeFloat *p_val, 
    CashStatus *p_cs,
    CashStatus *p_ch_data
) {
    uint64_t aprx_val = p_val->mantissa / al_pow (
        10, 
        labs(p_val->val_exp)
    );

    uint16_t cm_exp = __al_FindMinCashBase10Exp (
        p_cs, 
        0, 
        p_cs->banknote_c
    );

    // Reduce the size of memory usage by reducing base 10 exponant
    for(uint16_t i = 0; i < cm_exp; i++) {
        aprx_val /= 10;
        for(uint64_t j = 0; j < p_cs->banknote_c; j++) {
            p_cs->banknote_vals[j] /= 10;
        }
    }

    // Return if there is too much money to convert
    if(aprx_val >= MAX_CHANGE_SUM) {
        for(uint16_t i = 0; i < p_cs->banknote_c; i++)
            p_cs->banknote_vals[i] *= al_pow(10, cm_exp);

        return 0;
    }

    uint64_t *changes = (uint64_t*) malloc((aprx_val + 1) * sizeof(uint64_t));
    uint64_t *last_notes = (uint64_t*) malloc((aprx_val + 1) * sizeof(uint64_t));

    // Zero amount of money always means zero change
    changes[0] = 0;
    for(size_t i = 1; i < aprx_val + 1; i++)
        changes[i] = ULLONG_MAX;

    uint64_t sub_val = 0;
    for(uint64_t i = 0; i <= aprx_val; i++) {
        for(int64_t j = p_cs->banknote_c - 1; j >= 0; j--) {
            if(p_cs->banknote_vals[j] > i)
                continue;

            if(changes[i - p_cs->banknote_vals[j]] + 1 < changes[i]) {
                changes[i] = changes[i - p_cs->banknote_vals[j]] + 1;
                last_notes[i] = p_cs->banknote_vals[j];
            }
        }
    }

    
    uint64_t i = aprx_val;
    uint64_t j;
    while(i > 0) {
        if (
            (j = __al_linsearch (
                p_ch_data->banknote_vals, 
                p_ch_data->banknote_c, 
                sizeof(uint64_t), 
                &last_notes[i]
            )) != ULONG_MAX
        ) p_ch_data->val_c[j]++;

        if(i < last_notes[i])
            break;

        i -= last_notes[i];
    }

    // Redo cashstatus multiplication with 10 power n
    for(uint64_t i = 0; i < p_cs->banknote_c; i++)
        p_cs->banknote_vals[i] *= al_pow(10, cm_exp);

    return changes[aprx_val];
}


LIB_EXPORT uint64_t CALL_CON al_pow(uint64_t base, uint64_t exp) {
    if(!exp)
        return 1;
    else if(exp % 2)
        return base * al_pow(base, exp - 1);
    else {
        uint64_t half = al_pow(base, exp / 2);
        return half * half;
    }
}


/*
 * Wrapper function to sort all banknotes in growing order 
 */
LIB_EXPORT void CALL_CON al_SortBankNotes(CashStatus *p_cs) {
    // TODO: Create copy of banknote values
    CashStatus cpy = {0};
    cpy.banknote_c = p_cs->banknote_c;
    cpy.banknote_vals = (uint64_t*) calloc (
        cpy.banknote_c,
        sizeof(uint64_t)
    );

    cpy.val_c = (int64_t*) calloc (
        cpy.banknote_c,
        sizeof(uint64_t)
    );

    memcpy (
        cpy.banknote_vals,
        p_cs->banknote_vals,
        p_cs->banknote_c * sizeof(uint64_t)
    );

    memcpy (
        cpy.val_c,
        p_cs->val_c,
        p_cs->banknote_c * sizeof(uint64_t)
    );

    // Call static merge sorting algorithm
    __al_CashValMergeSort (
        p_cs,
        0,
        p_cs->banknote_c,
        &cpy
    );

    free(cpy.val_c);
    free(cpy.banknote_vals);
}


/******************************************/
/******* Money withdrawal wrappers ********/
/******************************************/

/*
 * Withdraw the maximum aprx_val of cash possible 
 */
LIB_EXPORT WithdrawReport CALL_CON al_WithdrawMaxBillsC (
    SafeFloat *p_val, 
    CurrencyInfo *p_cur
) {
    WithdrawReport wr = {0};
    wr.cs.banknote_vals = (uint64_t*) calloc (
        p_cur->cs.banknote_c, 
        sizeof(uint64_t)
    );

    wr.cs.val_c = (int64_t*) calloc (
        p_cur->cs.banknote_c, 
        sizeof(uint64_t)
    );

    uint64_t min_note = p_cur->cs.banknote_vals[0];
    for(uint64_t i = 0; i < p_cur->cs.banknote_c && !p_cur->cs.val_c[i]; i++)
        min_note = p_cur->cs.banknote_vals[i];

    min_note *= al_pow(10, abs(p_val->val_exp));
    uint64_t mod_mantissa = p_val->mantissa % min_note;
    uint64_t aprx_val = p_val->mantissa - mod_mantissa;
    for(int i = 0; i < abs(p_val->val_exp); i++)
        aprx_val /= 10;

    uint64_t j = 0;
    for(uint64_t i = 0; i < p_cur->cs.banknote_c; i++) {
        while(p_cur->cs.val_c[i] > 0 && aprx_val >= p_cur->cs.banknote_vals[i]) {
            aprx_val -= p_cur->cs.banknote_vals[i];
            p_cur->cs.val_c[i]--;
            if(j <= i) {
                wr.cs.banknote_vals[j] = p_cur->cs.banknote_vals[i];
                wr.cs.banknote_c++;
                j++;
            }

            wr.cs.val_c[i]++;
        }

        if(aprx_val < p_cur->cs.banknote_vals[i])
            break;
    }


    wr.unexchanged.val_exp = p_val->val_exp;
    wr.unexchanged.mantissa = mod_mantissa;

    return wr;
}


/*
 * Withdraw as little cash as possible
 */
LIB_EXPORT WithdrawReport CALL_CON al_WithdrawMinBillsC (
    SafeFloat *p_val,
    CurrencyInfo *p_cur
) {
    WithdrawReport wr = {0};
    // Check if given currency has cash loaded and return if it doesn't
    if(!p_cur->cs.banknote_c)
        return wr;

    wr.unexchanged.val_exp = p_val->val_exp;
    wr.cs.banknote_vals = (uint64_t*) calloc (
        p_cur->cs.banknote_c,
        sizeof(uint64_t)
    );

    wr.cs.val_c = (int64_t*) calloc (
        p_cur->cs.banknote_c,
        sizeof(uint64_t)
    );

    uint64_t min_note = p_cur->cs.banknote_vals[0];
    for(uint64_t i = 0; i < p_cur->cs.banknote_c && !p_cur->cs.val_c[i]; i++)
        min_note = p_cur->cs.banknote_vals[i];

    min_note *= al_pow(10, abs(wr.unexchanged.val_exp));

    uint64_t mod_mantissa = p_val->mantissa % min_note;
    SafeFloat min_val = { 
        .mantissa = p_val->mantissa - mod_mantissa,
        .val_exp = p_val->val_exp
    };

    CashStatus ch_data;
    ch_data.banknote_c = p_cur->cs.banknote_c;
    ch_data.banknote_vals = p_cur->cs.banknote_vals;
    ch_data.val_c = (int64_t*) calloc (
        ch_data.banknote_c,
        sizeof(uint64_t)
    );

    uint64_t c = __al_ChangeFinder (
        &min_val, 
        &p_cur->cs,
        &ch_data
    );
    wr.cs.banknote_c = ch_data.banknote_c;

    // Sort out all non-zero change data
    for(uint64_t i = 0, j = 0; i < ch_data.banknote_c; i++) {
        if(ch_data.val_c[i]) {
            // Not enough cash in atm move try to compensate it from smaller bills
            if(ch_data.val_c[i] > p_cur->cs.val_c[i]) {
                uint64_t delta_sum = p_cur->cs.banknote_vals[i] * (ch_data.val_c[i] - p_cur->cs.val_c[i]);
                ch_data.val_c[i] = p_cur->cs.val_c[i];
                __al_FillCashCaps (
                    &ch_data, 
                    &p_cur->cs, 
                    delta_sum, 
                    i - 1
                );
                i = 0;
            }

            wr.cs.banknote_vals[i] = ch_data.banknote_vals[i];
            wr.cs.val_c[i] = ch_data.val_c[i];
        }
    }

    wr.unexchanged.mantissa = mod_mantissa;
    wr.unexchanged.val_exp = min_val.val_exp;

    for(uint64_t i = 0; i < wr.cs.banknote_c; i++)
        if(wr.cs.banknote_vals[i] && wr.cs.val_c[i])
            p_cur->cs.val_c[i] -= wr.cs.val_c[i];

    free(ch_data.val_c);

    return wr;
}


/*
 * Withdraw as much different banknotes as possible
 */
LIB_EXPORT WithdrawReport CALL_CON al_WithdrawDifBillsC (
    SafeFloat *p_val,
    CurrencyInfo *p_cur
) {
    WithdrawReport wr = {0};
    wr.cs.banknote_vals = (uint64_t*) calloc (
        p_cur->cs.banknote_c,
        sizeof(uint64_t)
    );

    wr.cs.val_c = (int64_t*) calloc (
        p_cur->cs.banknote_c,
        sizeof(uint64_t)
    );

    uint64_t min_note = p_cur->cs.banknote_vals[0];
    uint64_t bn_offset = 0;
    for(bn_offset = 0; bn_offset < p_cur->cs.banknote_c && !p_cur->cs.val_c[bn_offset]; bn_offset++)
        min_note = p_cur->cs.banknote_vals[bn_offset];
    uint64_t unpow_min_note = min_note;
    
    min_note *= al_pow(10, abs(p_val->val_exp));
    uint64_t unex_mantissa = p_val->mantissa % min_note;
    uint64_t aprx_val = p_val->mantissa - unex_mantissa;
    for(int i = 0; i < abs(p_val->val_exp); i++)
        aprx_val /= 10;

    uint64_t j = 0;

    // Start looking from smallest bills to the biggest ones.
    bool is_exit = true;
    while(aprx_val >= unpow_min_note) {
		is_exit = true;
        for(uint64_t i = 0; i < p_cur->cs.banknote_c; i++) {
            if(p_cur->cs.val_c[i] && aprx_val >= p_cur->cs.banknote_vals[i]) {
                if(j <= i - bn_offset) {
                    wr.cs.banknote_vals[j] = p_cur->cs.banknote_vals[i];
                    wr.cs.banknote_c++;
                    j++;
                }

                wr.cs.val_c[i - bn_offset]++;
                p_cur->cs.val_c[i]--;
                aprx_val -= p_cur->cs.banknote_vals[i];
                is_exit = false;
            }
        }

        if (is_exit) break;
    }

    wr.unexchanged.mantissa = unex_mantissa;
    wr.unexchanged.val_exp = p_val->val_exp;

    return wr;
}
