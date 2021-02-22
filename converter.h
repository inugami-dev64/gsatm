#ifndef CONVERTER_H
#define CONVERTER_H

#ifdef __CONVERTER_C
    #include <stdlib.h>
    #include <stdint.h>
    #include "hashmap.h"
    #define ARR_LEN(x) sizeof(x) / sizeof(x[0])
    char *iso4217[] = {
        "AFN", 
        "ALL", 
        "DZD", 
        "AOA", 
        "XCD", 
        "ARS", 
        "AMD", 
        "AWG", 
        "AUD", 
        "AZN", 
        "BSD", 
        "BHD", 
        "BDT", 
        "BBD", 
        "BYN", 
        "BZD", 
        "BMD", 
        "INR", 
        "BAM", 
        "BWP", 
        "BRL", 
        "BND", 
        "BGN", 
        "BIF", 
        "CVE", 
        "KHR", 
        "CAD", 
        "KYD", 
        "CLP", 
        "CNY", 
        "COP", 
        "KMF", 
        "CDF", 
        "CRC", 
        "HRK", 
        "CUP", 
        "CUC", 
        "ANG", 
        "CZK", 
        "DKK", 
        "DJF", 
        "XCD", 
        "DOP", 
        "EGP", 
        "SVC", 
        "ERN", 
        "SZL", 
        "ETB", 
        "EUR", 
        "FKP", 
        "FJD", 
        "GMD", 
        "GEL", 
        "GHS", 
        "GIP", 
        "XCD", 
        "GTQ", 
        "GNF", 
        "GYD", 
        "HTG", 
        "HNL", 
        "HKD", 
        "HUF", 
        "ISK", 
        "INR", 
        "IRR", 
        "IQD", 
        "ILS", 
        "JMD", 
        "JPY", 
        "JOD", 
        "KZT", 
        "KES", 
        "KPW", 
        "KWD", 
        "KGS", 
        "LAK", 
        "LBP", 
        "LRD", 
        "LYD", 
        "CHF", 
        "MKD", 
        "MGA", 
        "MWK", 
        "MYR", 
        "MVR", 
        "MRU", 
        "MUR", 
        "MXN", 
        "MDL", 
        "MNT", 
        "MAD", 
        "MZN", 
        "MMK", 
        "NAD", 
        "NPR", 
        "NZD", 
        "NIO", 
        "NOK", 
        "OMR", 
        "PKR", 
        "PHP", 
        "PLN", 
        "QAR", 
        "RON", 
        "RUB", 
        "RWF", 
        "SHP", 
        "XCD", 
        "SAR", 
        "RSD", 
        "SCR", 
        "SLL", 
        "SGD", 
        "ANG", 
        "SBD", 
        "SOS", 
        "ZAR", 
        "SSP", 
        "LKR", 
        "SDG", 
        "SRD", 
        "SEK", 
        "CHF", 
        "SYP", 
        "TWD", 
        "TZS", 
        "TTD", 
        "TND", 
        "TRY", 
        "TMT", 
        "UGX", 
        "UAH", 
        "AED", 
        "GBP", 
        "USD", 
        "UYU", 
        "UYI", 
        "UZS", 
        "VUV", 
        "VES", 
        "VND", 
        "MAD", 
        "YER", 
        "ZMW", 
        "ZWL" 
    };

    char *cur_names[] = {
        "Afganistani Afghani",
        "Albanian Lek",
        "Algerian Dinar",
        "Angolan Kwanza",
        "East Caribbean Dollar",
        "Argentine Peso",
        "Armenian Dram",
        "Aruban Florin",
        "Australian Dollar",
        "Azerbaijan Manat",
        "Bahamian Dollar",
        "Bahraini Dinar",
        "Bangladeshi Taka",
        "Barbados Dollar",
        "Belarusian Ruble",
        "Belize Dollar",
        "Bermudian Dollar",
        "Indian Rupee",
        "Boznia and Herzegovina Convertible Mark",
        "Botswana Pula",
        "Brazilian Real",
        "Brunei Dollar",
        "Bulgarian Lev",
        "Burundi Franc",
        "Cabo Verde Escudo",
        "Cambodian Riel",
        "Canadian Dollar",
        "Cayman Islands Dollar",
        "Chilean Peso",
        "Chinese Yuan Renminbi",
        "Colombian Peso",
        "Comorian Franc ",
        "Congolese Franc",
        "Costa Rican Colon",
        "Croatian Kuna",
        "Cuban Peso",
        "Peso Convertible",
        "Netherlands Antillean Guilder",
        "Czech Koruna",
        "Danish Krone",
        "Djibouti Franc",
        "East Caribbean Dollar",
        "Dominican Peso",
        "Egyptian Pound",
        "El Salvador Colon",
        "Eritrean Nakfa",
        "Swazi Lilangeni",
        "Ethiopian Birr",
        "Euro",
        "Falkland Islands Pound",
        "Fiji Dollar",
        "Gambian Dalasi",
        "Georgian Lari",
        "Ghana Cedi",
        "Gibraltar Pound",
        "East Caribbean Dollar",
        "Guatemalan Quetzal",
        "Guinean Franc",
        "Guyana Dollar",
        "Haitan Gourde",
        "Honduran Lempira",
        "Hong Kong Dollar",
        "Hungriant Forint",
        "Iceland Krona",
        "Indian Rupee",
        "Iranian Rial",
        "Iraqi Dinar",
        "New Israeli Sheqel",
        "Jamaican Dollar",
        "Japanese Yen",
        "Jordanian Dinar",
        "Kazakhstani Tenge",
        "Kenyan Shilling",
        "North Korean Won",
        "Kuwaiti Dinar",
        "Kyrgyzstani Som",
        "Lao Kip",
        "Lebanese Pound",
        "Liberian Dollar",
        "Libyan Dinar",
        "Swiss Franc",
        "Macedonian Denar",
        "Malagasy Ariary",
        "Malawi Kwacha",
        "Malaysian Ringgit",
        "Rufiyaa",
        "Ouguiya",
        "Mauritius Rupee",
        "Mexican Peso",
        "Moldovan Leu",
        "Mongolian Tugrik",
        "Moroccan Dirham",
        "Mozambique Metical",
        "Burmese Kyat",
        "Namibia Dollar",
        "Nepalese Rupee",
        "New Zealand Dollar",
        "Cordoba Oro",
        "Norwegian Krone",
        "Rial Omani",
        "Pakistan Rupee",
        "Philippine Peso",
        "Polish Zloty",
        "Qatari Rial",
        "Romanian Leu",
        "Russian Ruble",
        "Rwanda Franc",
        "Saint Helena Pound",
        "East Caribbean Dollar",
        "Saudi Riyal",
        "Serbian Dinar",
        "Seychelles Rupee",
        "Sierra Leone",
        "Singapore Dollar",
        "Netherlands Antillean Guilder",
        "Solomon Islands Dollar",
        "Somali Shilling",
        "South African Rand",
        "South Sudanese Pound",
        "Sri Lanka Rupee",
        "Sudanese Pound",
        "Surinam Dollar",
        "Swedish Krona",
        "Swiss Franc",
        "Syrian Pound",
        "New Taiwan Dollar",
        "Tanzanian Shilling",
        "Trinidad and Tobago Dollar",
        "Tunisian Dinar",
        "Turkish Lira",
        "Turkmenistan New Manat",
        "Uganda Shilling",
        "Ukrainian Hryvnia",
        "UAD Dirham",
        "Pound Sterling",
        "US Dollar",
        "Peso Uruguayo",
        "Uruguay Peso en Unidades ",
        "Uzbekistan Sum",
        "Vanuatu Vatu",
        "Bolívar Soberano",
        "Vietnamese Dong",
        "Moroccan Dirham",
        "Yemeni Rial",
        "Zambian Kwacha",
        "Zimbabwe Dollar"
    };

    Hashmap cur_map;
#endif
    
    void initConverter();
#endif
