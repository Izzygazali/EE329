#include "string_conv.h"

/*
 * A function that gets the current date from the gps module and converts it to a string
 * INPUTS       NONE
 * RETURN       char * date = pointer to the address of the first character in the string,
 *                            the data is 10 characters long.
 */
char * date_to_string(){
    static char date[10];
    sprintf(date, "%u-%02.2u-%02.2u", get_curr_year(), get_curr_month(), get_curr_day());
    return date;
}

char * time_to_string(){
    static char time[8];
    sprintf(time, "%02.2u:%02.2u:%02.2u", get_curr_hour(), get_curr_minute(), get_curr_second());
    return time;
}

char * tow_to_string(){
    static char time[8];
    uint8_t hour, min;
    uint32_t diff_tow = get_diff_tow();
    hour = diff_tow/3600000;
    min = (diff_tow-hour*3600000)/60000;
    sprintf(time, "%02.2u:%02.2u", hour, min);
    return time;
}

char * longitude_to_string(){
    static char longitude[12];
    char temp[3];
    uint8_t i;
    sprintf(longitude, "%0*d", 12,get_curr_lon());

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

    sprintf(latitude, "%0*d", 11,get_curr_lat());

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
    sprintf(temp, "%0.2f",get_temperature());

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
    sprintf(alt, "%0.1f",get_altitude() );

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
    float dummyVar = 0;
    static char speed[6];
    sprintf(speed, "%0.1f",dummyVar );

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

char * dist_to_string()
{
    float dist;
    static char dist_string[5];
    dist = get_curr_dist()/((float)1000);
    sprintf(dist_string, "%0.1f",dist);
    return dist_string;
}
