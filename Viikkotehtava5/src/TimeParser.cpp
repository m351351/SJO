#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "TimeParser.h"

//tänne lisää varsinaisia tsekkauskoodeja 

// time format: HHMMSS (6 characters)
int time_parse(char *time) {
	
	if(time == NULL){
		return TIME_ARRAY_ERROR;
	}

	if(strlen(time) != 6){
		return TIME_LEN_ERROR;
	}

	for (int i = 0; i < 6; i++){
		if(!isdigit(time[i])){
			return TIME_ARRAY_ERROR;
		}
	}

    int values[3];
	values[2] = atoi(time+4); // seconds
	time[4] = 0;
	values[1] = atoi(time+2); // minutes
	time[2] = 0;
	values[0] = atoi(time); // hours

	if (values[0] < 0 || values[0] > 23 ||
        values[1] < 0 || values[1] > 59 ||
        values[2] < 0 || values[2] > 59) {
        return TIME_VALUE_ERROR;
    }

	if (values[0] == 17 &&
        values[1] == 44 &&
        values[2] == 00) {
        return EASTEREGG;
    }

	int seconds = (values[0] * 3600) + (values[1] * 60) + values[2];
	
	if (seconds == 0){
		return TIME_VALUE_ERROR;
	}


	return seconds;
}


int liikennevalot(char *merkki) {
	if (merkki == NULL){
		return MARK_VALUE_ERROR;
	}
	
	int len = strlen(merkki);
	if (len == 0 || len > 20){
		return MARK_LEN_ERROR;
	}

	for (int i = 0; i < len; i++){
		char c = merkki[i];
		
		if(!isalpha(c) && !isdigit(c) && c != ',')
		{
			return MARK_ARRAY_ERROR;
		}

		if (isalpha(c)) {
				char upper_c = toupper(c);
				if (upper_c != 'R' && upper_c != 'G' && upper_c != 'Y' && 
					upper_c != 'T' && upper_c != 'D') {
					return MARK_VALUE_ERROR; // Tuntematon kirjain / värikoodi
				}
			}
		}
		
		return 0;
}

		
	

