#include "string_conv.h"

uint16_t temp_year = 2018;
uint8_t temp_month = 6;
uint8_t temp_day = 1;

uint8_t temp_hour = 9;
uint8_t temp_min = 36;
uint8_t temp_sec = 1;

int32_t temp_long = 1206629544;
int32_t temp_lat = -352986824;

float temperature = 26.0;
float altitude = 110.553453;
float temp_speed = 110.553453;



/*
 * A function that gets the current date from the gps module and converts it to a string
 * INPUTS       NONE
 * RETURN       char * date = pointer to the address of the first character in the string,
 *                            the data is 10 characters long.
 */
char * date_to_string(){
    static char date[10];
    sprintf(date, "%u-%02.2u-%02.2u", temp_year, temp_month, temp_day);
    return date;
}

char * time_to_string(){
    static char time[8];
    sprintf(time, "%02.2u:%02.2u:%02.2u", temp_hour, temp_min, temp_sec);
    return time;
}

char * longitude_to_string(){
    static char longitude[12];
    char temp[3];
    uint8_t i;
    sprintf(longitude, "%0*d", 12,temp_long);

    for(i = 2; i <= 4; i++)
        temp[i-2] = longitude[i];

    longitude[4] = '.';

    for(i = 1; i <= 3; i++)
        longitude[i] = temp[i-1];

    if(longitude[0] == '0')
        longitude[0] = '+';

    return longitude;
}

char * latitude_to_string(){
    static char latitude[11];
    char temp[2];
    uint8_t i;

    sprintf(latitude, "%0*d", 11,temp_lat);

    for(i = 2; i <= 3; i++)
        temp[i-2] = latitude[i];

    latitude[3] = '.';

    for(i = 1; i <= 2; i++)
        latitude[i] = temp[i-1];

    if(latitude[0] == '0')
        latitude[0] = '+';

    return latitude;
}

char * temp_to_string(){
    uint8_t i,j;
    static char temp[5];
    sprintf(temp, "%0.2f",temperature );//get_temperature()

    for(i = 4; i > 0; i--){
        if((temp[i] == 48)||(temp[i] == 0)&&(temp[i-1] != '.'))
        {
            for(j=4; j > 0; j--)
                temp[j] = temp[j-1];
            temp[0] = ' ';
        }
        else
            return temp;
    }
    return temp;
}


char * alt_to_string(){
    uint8_t i,j;
    static char alt[7];
    sprintf(alt, "%0.1f",altitude );

    for(i = 6; i > 0; i--){
        if((alt[i] == 48)||(alt[i] == 0)&&(alt[i-1] != '.'))
        {
            for(j=6; j > 0; j--)
                alt[j] = alt[j-1];
            alt[0] = ' ';

        }else
            return alt;
    }
    return alt;
}

char * speed_to_string(){
    uint8_t i,j;
    static char speed[6];
    sprintf(speed, "%0.1f",temp_speed );

    for(i = 5; i > 0; i--){
        if((speed[i] == 48)||(speed[i] == 0)&&(speed[i-1] != '.'))
        {
            for(j=5; j > 0; j--)
                speed[j] = speed[j-1];
            speed[0] = ' ';

        }else
            return speed;
    }
    return speed;
}
