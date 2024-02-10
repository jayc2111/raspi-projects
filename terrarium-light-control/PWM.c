#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <unistd.h>

#include "PWM.h"

#define RANGE   4096
#define PWM_PIN   18


#define LOAD_BAR                                        \
printf("|                                       |\n");  \
printf(">");                                            \

/*********************************************
** local functions
*********************************************/
void sig_handler(int signo);


/*********************************************
** MAIN
*********************************************/
int main (void)
{
  short dir = 1;    /* direction */
  int   cnt = 1;    /* index counter */
  int   tmp = 0;
  

  /* set up signal handler to exit with CTRL+C */
  if (signal(SIGINT, sig_handler) == SIG_ERR)
  {
    printf("\ncan't catch SIGINT\n");
  }
  else if (wiringPiSetupGpio() == -1)
  {
    printf("\n wiringPi init failed :-(\n");
  }
  else
  {
    
    pinMode(PWM_PIN,PWM_OUTPUT); /* init wiringPi functions */
    pwmSetClock(2);              /* set PWM divisor: 9.6MHz, fastest you can get */
    pwmSetRange (RANGE) ;        /* range gives us the resolution */

    LOAD_BAR
    
    /**** main loop ****/
    while ((cnt >= 0) && (cnt <= RANGE))
    {
      /* ---------------------- debug */
      if ((cnt == RANGE) || (cnt == 0) )
      {
        dir = -dir;
        printf("\n reset from %d to %d \n", -dir, dir);
        
        scanf("%d", &tmp);
        
        LOAD_BAR
      }
      else if(cnt % 100 == 0)
      {
        printf("*");
        fflush(NULL);
      }
      else
      {
      }
      /* ---------------------- debug */
      
      
      /* write value to ouput pin */
      pwmWrite (PWM_PIN, exp_array[cnt]);
      
      cnt += dir;
      usleep(15000); /* one minute dimming up: 15 ms */
    }/**** main loop ****/
  }
  
  /* exit on error */
  sig_handler(SIGINT);
 
  return 0; 
}/**** main ****/


void sig_handler(int signo)
{
  printf("\n exit and off \n");
  
  /* turn off the PWM pin */
  pwmWrite (PWM_PIN, 0);
  
  exit(1);
}
