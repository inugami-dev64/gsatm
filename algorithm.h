#ifndef __ALGORITHM_H
#define __ALGORITHM_H

#ifdef __ALGORITHM_C
    #include <stdlib.h>
    #include <string.h>
    #include <stdint.h>
    #include <stdio.h>
    #include <limits.h>
    #include "hashmap.h"
    #include "fm.h"

    typedef uint8_t bool;
    #define true 1
    #define false 0
   
    /*
     * Maximum change making sum after dividing sum with 10 ^ n, where
     * n is the maximum common base 10 exponant.
     * Solving change making problem this amount of cash 
     * uses at least 16mb of memory on stack
     */
    #define MAX_CHANGE_SUM  1000000
    
    /*
     * This function attempts to fill all the missing sum 
     * cash caps with smaller bills
     */
    static void __al_FillCashCaps (
        CashStatus *p_ch, 
        CashStatus *p_cur, 
        size_t sum,
        size_t beg
    );

    /* Perform merge sort algorithm on banknote values */
    static void __al_CashValMergeSort (
        CashStatus *p_in, 
        size_t beg, 
        size_t end,
        CashStatus *p_tmp
    );

    /* Find the minimum common base 10 exponent for banknotes */ 
    static uint16_t __al_FindMinCashBase10Exp (
        CashStatus *p_cs, 
        uint64_t beg, 
        uint64_t end
    );

    /* 
     * Find the smallest needed amount of banknotes 
     * NOTE: p_ch_data must have a capacity of atleast banknote_c in p_cs
     */
    static uint64_t __al_ChangeFinder (
        SafeFloat *p_val, 
        CashStatus *p_cs, 
        CashStatus *p_ch_data
    );

    /* 
     * Perform linear search for n bytes to cmp in arr 
     * RETURN: index of the found instance (ULONG_MAX if not found)
     */
    static size_t __al_linsearch (
        void *arr, 
        size_t size, 
        size_t n, 
        void *cmp
    );

#endif

/* 
 * Store information about currency conversion
 */
typedef struct WithdrawReport {
    CashStatus cs;
    SafeFloat unexchanged;
} WithdrawReport;

// For debugging
#ifdef __ALGORITHM_C
    #include "converter.h" // sprintSafeFloat()
#endif

/* 
 * Use merge sort algorithm to sort all currency banknotes 
 * by value in ascending order 
 */
void al_SortBankNotes(CashStatus *p_cs);


/* 64 bit integer exponentiation */
uint64_t al_pow(uint64_t base, uint64_t exp);

/**************************************************/
/******** Algorithms for money withdrawal *********/
/**************************************************/

/*
 * Withdraw as much cash as possible
 */
WithdrawReport al_WithdrawMaxBillsC (
    SafeFloat *p_val, 
    CurrencyInfo *p_cur
);


/*
 * Withdraw as little cash as possible
 */
WithdrawReport al_WithdrawMinBillsC (
    SafeFloat *p_val,
    CurrencyInfo *p_cur
);


/*
 * Withdraw as much different banknotes as possible
 */
WithdrawReport al_WithdrawDifBillsC (
    SafeFloat *p_val,
    CurrencyInfo *p_cur
);


#endif
