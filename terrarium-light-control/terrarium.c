
/*********************************************
* -------------
* Revision log
* -------------
* Version     Date        Comment
*    7        21-03-2019  bugfixes on new parameter checks
*    6        19-03-2019  changed input parameter to dim-time
*    5        04-02-2018  added macro for dim-time in minutes
*    4        03-01-2018  PWM updates
*    3        18-11-2017  wintermode
*    2        29-09-2017  updates on comments
*    1        29-09-2017  first version
*
*********************************************/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include <wiringPi.h>

#include "PWM.h"


/*********************************************
** global defines
*********************************************/
#define OK   0
#define ERR -1

#define RANGE          4096
#define PWM_PIN          18 /* RasPi Pins 18 */
#define PWM_CLOCK_DIV     2 /* PWM Clock mode for 2,34 kHz PWM */
//#define DIM_TIME_Min     10 /* dimming delay in minutes */

/*********************************************
** local functions
*********************************************/
void sig_handler(int signo);
int  switch_lamp(int dir);


/*********************************************
** MAIN
*********************************************/
int main(int argc, char* argv[])
{
  int input = 0;

  /**** init ****/
  if (signal(SIGINT, sig_handler) == SIG_ERR)
  {
    printf("\ncan't catch SIGINT\n");
  }
  else if (wiringPiSetupGpio() == -1)
  {
    printf("\n wiringPi init failed :-(\n");
  }
  else if (argc != 2)
  {
    printf("\n wrong cmd-line arguments, argc=%d\n", argc);
  }
  else if ( (atoi(argv[1]) < -600) || (atoi(argv[1]) > 600) )
  {
    printf("\n wrong cmd-line arguments, argv[1]=%s\n", argv[1]);
  }
  else
  {
    input = atoi(argv[1]);

    /* check range for the delay */
    if ( ((input > -601) && (input <  -5)) ||
         ((input >    5) && (input < 601))
       )
    {
#ifndef TEST
      pinMode(PWM_PIN,PWM_OUTPUT); /* init wiringPi functions */
      pwmSetClock(PWM_CLOCK_DIV);  /* init PWM divisor (fastest you can get) */
      pwmSetRange(RANGE);          /* init range gives (resolution) */
      pwmSetMode(PWM_MODE_MS);     /* PWM_MODE_MS for "blocks" PWM_MODE_BAL for switching */

      /* turn lamps on or off */
      switch_lamp(input);
#else
      printf("winter mode - %i ", input);
#endif
      printf("done\n");
    }
  }

  return 0;
}/**** main ****/


void sig_handler(int signo)
{
  printf("\n exit and off \n");

  /* turn off the PWM pin */
  pwmWrite (PWM_PIN, 0);

  exit(1);
}


/* @brief    switch_lamp: function sweeps GPIO pin output on or off
 *
 * @param    [in] dir   if >0 sweep from low to high
 *                      if <0 sweep from high to low
 */
int switch_lamp(int dir)
{
  int rv  = ERR;    /* return value */
  int idx  = ERR;
  //int dim_delay  = (60*DIM_TIME_Min*1000000)/RANGE; /* in micro seconds */
  int dim_delay  = 0; /* in micro seconds */

  if ( (dir < -600) || (dir > 600) )
  {
    printf("switch_lamp failed: par = %d\n", dir);
  }
  else
  {
    dim_delay = (abs(dir)*1000000)/RANGE;

    /* determine if we ...*/
    if(dir >= 0)
    { /* sweep up or ... */
      idx = 0;
      dir = 1;
    }
    else
    { /* sweep down */
      idx = RANGE-1;
      dir = -1;
    }
    printf("%d\t->\t", dir);

    while ( (idx >= 0) && (idx <= RANGE-1) )
    {
      /* write value to ouput pin */
      pwmWrite (PWM_PIN, exp_array2[idx]);

      usleep(dim_delay);
      idx += dir;
    }

    rv = OK;
  }

  return rv;
}
