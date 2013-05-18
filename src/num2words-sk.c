#include <stdbool.h>

#include "num2words-sk.h"
#include "string.h"

// jednotky bez desiatok
static const char* const ONES[] = {
  "nula",
  "jedna",
  "dve",
  "tri",
  "štyri",
  "päť",
  "šesť",
  "sedem",
  "osem",
  "deväť"
};

// jednotky po desiatkach
static const char* const ONEST[] = {
  "nula",
  "jeden",
  "dva",
  "tri",
  "štyri",
  "päť",
  "šesť",
  "sedem",
  "osem",
  "deväť"
};

static const char* const TEENS[] ={
  "",
  "jedenásť",
  "dvanásť",
  "trinásť",
  "štrnásť",
  "pätnásť",
  "šestnásť",
  "sedem- násť",
  "osemnásť",
  "devät- násť"
};

static const char* const TENS[] = {
  "",
  "desať",
  "dvadsať",
  "tridsať",
  "štyridsať",
  "päťdesiat",
  "šesťdesiat",
  "sedemdesiat",
  "osemdesiat",
  "deväťdesiat"
};

static char* STR_OH_CLOCK = "hodín";
static char* MINUTES = "minút";
static char* STR_OH_CLOCK_1 = "hodina";
static char* MINUTES_1 = "minúta";
static char* STR_OH_CLOCK_234 = "hodiny";
static char* MINUTES_234 = "minúty";
//static const char* STR_NOON = "poludnie";
//static const char* STR_MIDNIGHT = "polnoc";

static size_t append_number(char* words, int num, int add_inf) {
  int tens_val = num / 10 % 10;
  int ones_val = num % 10;
  char* minutes_string = MINUTES;
  char* hours_string = STR_OH_CLOCK;

  if (num == 1) {
    minutes_string = MINUTES_1;
    hours_string = STR_OH_CLOCK_1;      
  }
  else if (num == 2 || num == 3 || num == 4) {      
    minutes_string = MINUTES_234;
    hours_string = STR_OH_CLOCK_234;      
  }
  
  size_t len = 0;

    if (add_inf == -1) {
        strcat(words, hours_string);
        return strlen(hours_string);       
    }

    if (add_inf == 1) {
        strcat(words, " \n");
        len += 2;
    }


  if (tens_val > 0) {
    if (tens_val == 1 && num != 10) {
      strcat(words, TEENS[ones_val]);
      len += strlen(TEENS[ones_val]);
      //return len;
    }
    else {
        strcat(words, TENS[tens_val]);
        len += strlen(TENS[tens_val]);
        if (ones_val > 0) {
          strcat(words, " ");
          len += 1;

          // jednotky maju iny gramaticky tvar, ak su za desiatkami
          if (ones_val > 0 || num == 0) {
            strcat(words, ONEST[ones_val]);
            len += strlen(ONEST[ones_val]);
          }
        }
        //return len;
    }
  }
  else if (ones_val > 0 || num == 0) {
    strcat(words, ONES[ones_val]);
    len += strlen(ONES[ones_val]);
  }
  
    if (add_inf == 2) {
        strcat(words, " ");
        len += 1;
        strcat(words, hours_string);
        return strlen(hours_string);       
    }
  
    if (add_inf == 3) {
        strcat(words, " ");
        len += 1;
        strcat(words, minutes_string);
        return strlen(minutes_string);       
    }
  
  
  return len;
}

void time_to_2words(int hours, int minutes, char* words, size_t length, char* words2, size_t length2) {
    
    bool hour_ones = hours < 10;
    bool hour_tens = ((hours % 10) == 0);
    bool hour_teens = ((hours > 10) && (hours < 20));
    bool hour_twoline_teens = ((hours == 17) || (hours == 19));
    bool hour_is_single_line = hour_tens || hour_ones || (hour_teens && !hour_twoline_teens);

    bool minute_ones = minutes < 10;
    bool minute_tens = ((minutes % 10) == 0);
    bool minute_teens = ((minutes > 10) && (minutes < 20));
    bool minute_twoline_teens = ((minutes == 17) || (minutes == 19));
    bool minute_is_single_line = minute_tens || minute_ones || (minute_teens && !minute_twoline_teens);
    
    int hour_add_inf = 0;
    int minute_add_inf = 0;
    
    if (minutes == 0 && hour_is_single_line) {
        hour_add_inf = 1;
        minute_add_inf = -1;
        minutes = hours;
    }
    else if (minutes == 0 && !hour_is_single_line) {
        hour_add_inf = 0;
        minute_add_inf = -1;        
        minutes = hours;
    }
    else if (hour_is_single_line && minute_is_single_line) {
        hour_add_inf = 2;
        minute_add_inf = 3;
    }
    else if (hour_is_single_line) {
        hour_add_inf = 1;
    }

  size_t remaining = length;
  memset(words, 0, length);

  remaining -= append_number(words, hours, hour_add_inf);

  size_t remaining2 = length2;
  memset(words2, 0, length2);

  remaining2 -= append_number(words2, minutes, minute_add_inf);

}

