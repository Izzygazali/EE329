/*
 * Engineer(s): Ezzeddeen Gazali and Tyler Starr
 * Create Date: 06/02/2018
 */

#include "string_conv.h"


/*
 * A function that gets the current date from the gps module and converts it to a string in
 * the following format: 2018-06-02
 * INPUTS       NONE
 * RETURN       char * date = pointer to the address of the first character in the string,
 *                            the data is 10 characters long.
 */
char * date_to_string()
{
    static char date[10];
    sprintf(date, "%u-%02.2u-%02.2u", get_curr_year(), get_curr_month(), get_curr_day());
    return date;
}

/*
 * A function that gets the current time from the gps module and converts it to a string in
 * the following format: 24:59:59
 * INPUTS       NONE
 * RETURN       char * time = pointer to the address of the first character in the string,
 *                            the data is 8 characters long.
 */
char * time_to_string()
{
    static char time[8];
    sprintf(time, "%02.2u:%02.2u:%02.2u", get_curr_hour(), get_curr_minute(), get_curr_second());
    return time;
}

/*
 * A function that outputs a string containing a timer that begins time at the start of gathering data.
 * The function utilizes the get_diff_tow() gps function to get the time in ms that the gps has been collecting
 * data, then converts the time to hours and minutes and outputs a string in the following format: 01:01
 *
 * INPUTS       NONE
 * RETURN       char * time = pointer to the address of the first character in the string,
 *                            the data is 8 characters long.
 */
char * tow_to_string()
{
    static char time[8];
    uint8_t hour, min;
    uint32_t diff_tow = get_diff_tow(); //time in ms since start of data gathering
    hour = diff_tow/3600000;
    min = (diff_tow-hour*3600000)/60000;
    sprintf(time, "%02.2u:%02.2u", hour, min);
    return time;
}

/*
 * A function that outputs a string containing the pace that the hiking logger is moving at.
 * The function utilizes the get_diff_tow() and get_curr_dist() gps function to get the time in ms
 * and the distance in meters since the gps started collecting data. The pace is displayed in the
 * minutes and seconds in the following format: 01:01
 *
 * INPUTS       NONE
 * RETURN       char * pace_string = pointer to the address of the first character in the string,
 *                                   the data is 8 characters long.
 */
char * pace_to_string()
{
    static char pace_string[8];
    uint8_t sec, min;
    uint32_t pace = get_diff_tow()/get_curr_dist();     //pace in s/km
    min = pace/60;                                      //min per km
    if (min > 60 || pace == 0)                          //error checking so that when no movement occurs
        return "XX:XX";                                 //the pace is not displayed.
    sec = pace - min*60;                                //sec per km
    sprintf(pace_string, "%02.2u:%02.2u", min, sec);
    return pace_string;
}

/*
 * A function that gets the current longitude from the gps module and converts it to a string in
 * the following format: +/-XXX.XXXXXXX, leading 0's are placed in string if the whole part of the
 * value doesn't occupy the designated 3 locations
 * INPUTS       NONE
 * RETURN       char * longitude = pointer to the address of the first character in the string,
 *                                 the data is 12 characters long.
 */
char * longitude_to_string()
{
    static char longitude[12];
    char temp[3];
    uint8_t i;

    sprintf(longitude, "%0*d", 12,get_curr_lon());

    //formatting the string to the desired format
    for(i = 2; i <= 4; i++)
        temp[i-2] = longitude[i];
    longitude[4] = '.';
    for(i = 1; i <= 3; i++)
        longitude[i] = temp[i-1];
    if(longitude[0] == '0')
        longitude[0] = '+';

    return longitude;
}

/*
 * A function that gets the current longitude from the gps module and converts it to a string in
 * the following format: +/-XX.XXXXXXX, leading 0's are placed in string if the whole part of the
 * value doesn't occupy the designated 2 locations
 * INPUTS       NONE
 * RETURN       char * longitude = pointer to the address of the first character in the string,
 *                                 the data is 11 characters long.
 */
char * latitude_to_string()
{
    static char latitude[11];
    char temp[2];
    uint8_t i;

    sprintf(latitude, "%0*d", 11,get_curr_lat());

    //formatting the string to the desired format
    for(i = 2; i <= 3; i++)
        temp[i-2] = latitude[i];
    latitude[3] = '.';
    for(i = 1; i <= 2; i++)
        latitude[i] = temp[i-1];
    if(latitude[0] == '0')
        latitude[0] = '+';

    return latitude;
}

/*
 * A function that gets the current temperature from the MPL3115A2 and converts it to a string in
 * the following format: XXX.X, leading spaces are placed in string if the whole part of the
 * value doesn't occupy the designated 3 locations.
 * INPUTS       NONE
 * RETURN       char * temp = pointer to the address of the first character in the string,
 *                            the data is 5 characters long.
 */
char * temp_to_string()
{
    uint8_t i,j;
    static char temp[5];
    sprintf(temp, "%0.2f",get_temperature());

    //formatting the string to the desired format
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

/*
 * A function that gets the current altitude from the MPL3115A2 and converts it to a string in
 * the following format: XXXXX.X, leading spaces are placed in string if the whole part of the
 * value doesn't occupy the designated 5 locations.
 * INPUTS       NONE
 * RETURN       char * temp = pointer to the address of the first character in the string,
 *                            the data is 7 characters long.
 */
char * alt_to_string()
{
    uint8_t i,j;
    static char alt[7];
    sprintf(alt, "%0.1f",get_altitude() );

    //formatting the string to the desired format
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

/*
 * A function that gets the current speed from the gps module and converts it to a string in
 * the following format: XXXX.X, leading spaces are placed in string if the whole part of the
 * value doesn't occupy the designated 4 locations.
 * INPUTS       NONE
 * RETURN       char * speed = pointer to the address of the first character in the string,
 *                             the data is 6 characters long.
 */
char * speed_to_string()
{
    uint8_t i,j;
    static char speed[6];
    float curr_speed = get_curr_speed()*0.036; //get current speed and multiply by conversion factor
                                               //to convert from cm/s to km/hr
    sprintf(speed, "%0.1f", curr_speed);

    //formatting the string to the desired format
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

/*
 * A function that gets the current distance from the gps module and converts it to a string in
 * the following format: XXX.X, leading spaces are placed in string if the whole part of the
 * value doesn't occupy the designated 3 locations.
 * INPUTS       NONE
 * RETURN       char * dist_string = pointer to the address of the first character in the string,
 *                                   the data is 5 characters long.
 */
char * dist_to_string()
{
    float dist;
    static char dist_string[5];
    dist = get_curr_dist()/((float)1000); //convert distance from meters to kilometers.
    sprintf(dist_string, "%0.1f",dist);
    return dist_string;
}
