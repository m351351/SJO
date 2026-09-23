#ifndef TIMEPARSER_H
#define TIMEPARSER_H

// Error codes
#define TIME_LEN_ERROR      -1 // palautetaan jos merkkejä on erimäärä kuin 6
#define TIME_ARRAY_ERROR    -2 // jos palautusarvo on tyhjä (NULL) tai sisältää vääriä merkkejä
#define TIME_VALUE_ERROR    -3 // palautetaan jos sekunteja tai minuutteja yli 59 tai tunteja yli 23
#define EASTEREGG           -4 // palauttaa tämän jos antaa arvoksi mun syntymäkellonajan
#define MARK_LEN_ERROR      -5 // jotain muuta
#define MARK_ARRAY_ERROR    -6
#define MARK_VALUE_ERROR    -7

using namespace std;

int time_parse(char *time);
int liikennevalot(char *merkki);

#endif
