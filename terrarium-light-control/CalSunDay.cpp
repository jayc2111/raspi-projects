/*********************************************
* -------------
* Revision log
* -------------
* Version     Date        Comment
*    1        09-02-2019  first version
*
*********************************************/
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <stdint.h>


/*********************************************
** local functions
*********************************************/
uint32_t dec2hour (float dec);
float    hour2dec (uint32_t hour);


/*********************************************
** MAIN
*********************************************/
int main(int argc, char **argv)
{
  uint32_t sunrise, sunset, offset = 0u;
  float    sunrise_d, sunset_d, offset_d  = 0u;

  if ( (argc != 3)            ||
       (atoi(argv[1]) <  515) ||
       (atoi(argv[1]) >  830) ||
       (atoi(argv[2]) < 1615) ||
       (atoi(argv[2]) > 2145)    )

  {
    /* error - return default value */
    sunrise = 1000;
    sunset  = 1600;
  }
  else
  {
    /* extract input argument */
    sunrise = atoi(argv[1]);
    sunset  = atoi(argv[2]);

    /* convert from hour format to decimal */
    sunrise_d = hour2dec(sunrise);
    sunset_d  = hour2dec(sunset);

    /* calc length of the day */
    offset_d    = sunset_d - sunrise_d;

    /* calc offset from today's day length */
    offset_d = 2*(offset_d-9.98)/(16.3-9.98) + 2.0;

    /* back to hour format */
    sunrise = dec2hour(sunrise_d + offset_d);
    sunset  = dec2hour(sunset_d  - offset_d);
    offset  = dec2hour(offset_d);
  }

  printf("%04d\t%04d\t%d:%02d:00\n",
          sunrise,
          sunset,
          offset/100, offset-(offset/100)*100);
  return 0;
}
/*********************************************
*********************************************/

/* @brief    converts from hour-format to
 *           decimal-format
 *
 * @param    [in] time in hour format <HHMM>
 *
 * @return        time in decimal format <HH_10>
 */
float hour2dec(uint32_t hour)
{
  uint32_t h, m = 0u;

  h = hour/100;
  m = ((hour - h*100)*100)/60;

  return h+m/100.0;
}


/* @brief    converts from decimal-format to
 *           hour-format
 *
 * @param    [in] time in decimal format <HH_10>
 *
 * @return        time in hour format <HHMM>
 */
uint32_t dec2hour(float dec)
{
  uint32_t h, m = 0u;

  h = (uint32_t)dec;
  m = (dec - h)*60.0;

  return h*100+m;
}
