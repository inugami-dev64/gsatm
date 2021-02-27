# Valuutavahetus funktsioon
Valuutavahetuse teostamiseks oleks vaja koostada funktsioon / meetod, mis võtab sisendandmeteks järgnevad andmed:  
```
ExchangeInfo n,
ConversionType cv_type,
ExchangeInfo ex_rate,
CashStatus cash_stat
```
Tagastusväärtuseks peab olema CashStatus'e structi / classi array, mis sisaldab informatsiooni väljavõetud sularaha 
koguse iga kupüüri kohta. Samuti sõltub väljastatavate kupüüride kogus `cv_type` väärtusest.

## ExchangeInfo
ExchangeInfo struct sisaldab järgnevaid muutujaid:  
```
uint32_t mantissa;
int val_exp;
```
ExchangeInfo structi on kasutatud siin ujukoma arvude kümnendsüsteemis täpse esitamisega. Näiteks oletame, et
on vaja esitada arv 1.23, siis ExchangeInfo väärtus peaks olema {123, -2}    

## ConversionType
ConversionType on enum, mille väärtused võivad olla järgnevad: 
```
CONVERSION_TYPE_MAX = 0,
CONVERSION_TYPE_MIN = 1,
CONVERSION_TYPE_OPTIMAL = 2
```
ConversionType spetsifitseerib pangaautomaadi poolset raha väljastust. `CONVERSION_TYPE_MAX` annab välja maksimaalse väärtusega
rahatähti, `CONVERSION_TYPE_MIN` annab välja minimaalse väärtusega rahatähti ning `CONVERSION_TYPE_OPTIMAL` annab välja 
võimalikult palju erinevaid rahatähti.    

## CashStatus
CashStatus struct sisaldab järgnevaid muutujaid:
```
uint32_t bn_c;
uint32_t *bn_vals;
uint32_t *val_c;
```
CashStatus struct sisaldab informatsiooni sularaha seisu kohta mingi kindla valuuta suhtes. `bn_c` (banknote count) 
spetsifitseerib olemasolevate üksteisest erinevate rahatähtede kogust. `bn_vals` (banknote values) spetsifitseerib pointerit raha nimiväärtuste 
arrayle. `val_c` (value counts) spetsifitseerib pointerit rahatähe koguste array juurde.  
PS! C#is saab pointeritel allocatemise asemel kasutada `List<T>` ning C++is saab kasutada `std::vector<T>` klassi.
