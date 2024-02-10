
/*********************************************
* -------------
* Revision log
* -------------
* Version     Date        Comment
*    4        19-05-2019  bugfix on weekday
*    3        28-04-2019  replaced gettimeofday + clean up
*    2        27-04-2019  added timer for 59th second
*    1        10-04-2019  first version
*
*********************************************/

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <wiringPi.h>


/*********************************************
** global defines
*********************************************/

/* Clock status enum */
typedef enum {
  unsynced,
  init,
  started
} SyncState;

/* Clock status enum */
const char *wdayN[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

// Which GPIO pin we're using
#define PIN 0 //BCM Pin 17
// How much time a change must be since the last in order to count as a change
#define IGNORE_CHANGE_BELOW_USEC 10000


// Current state of the pin
static volatile int state;
// Time of last change
static struct timespec last;
// first minute flag
static volatile SyncState ClkState = unsynced;
//time of the last rising edge
static volatile unsigned int t_rise;
// vector holding the current received time
static volatile long long time_vec;
// seperate variable for the time
static unsigned char sec = 0;
static unsigned char hour = 0u;
static unsigned char min = 0u;
static unsigned char wday = 0u;
static unsigned char day = 0u;
static unsigned char mnth = 0u;
static unsigned char year = 0u;
// timer for 59th second
static timer_t Timerid1;


/*********************************************
** local functions
*********************************************/
void handle(void);
static void update_current_time(void);

static void init_timer(void);
static void start_timer(void);
static void print_to_console(int sig, siginfo_t *si, void *uc);


/*********************************************
** local function definitions
*********************************************/

/* @brief    init_timer: init signal handler for 59th second
 *
 * @param    void
 *
 */
void init_timer(void)
{
  struct sigaction sa;

  /* install callback-Handler */
  memset(&sa, 0, sizeof (sa));
  sa.sa_flags = SA_SIGINFO;
  sa.sa_sigaction = print_to_console;
  sigemptyset(&sa.sa_mask);
  
  /* register signal for timer expiration */
  if (sigaction(SIGALRM, &sa, NULL) == -1)
    perror("sigaction");
}


/* @brief    print_to_console: callback for 59th second
 *
 * @param    [in]      sig only to conform to signal handler
 *           [in/out] *si  only to conform to signal handler
 *           [in/out] *uc  only to conform to signal handler
 */
void print_to_console(int sig, siginfo_t *si, void *uc)
{
  if (ClkState == started) 
    printf("\t\t(%s) %02u-%02u-%u [%02u:%02u:%02u]\n", wdayN[wday-1], day, mnth, year, hour, min, sec);
  else
    printf("\n");
}


/* @brief    start_timer: timer for 1 second
 *
 * @param    void
 *
 */
void start_timer(void)
{
  struct itimerspec timer;

  /* configure timer for 1 second */
  timer.it_value.tv_sec  = 1;
  timer.it_value.tv_nsec = 0;
  timer.it_interval.tv_sec  = 0;
  timer.it_interval.tv_nsec = 0;
  
  /* create and set timer */
  timer_create (CLOCK_MONOTONIC, NULL, &Timerid1);
  timer_settime (Timerid1, 0, &timer, NULL);
}


/* @brief    handle: Handler for rising/ falling edge
 *
 * @param    void
 *
 */
void handle(void)
{
  unsigned int d_t = 0u;
  unsigned long diff = 0u;
  struct timespec now;

  //~ gettimeofday(&now, NULL);
  clock_gettime(CLOCK_MONOTONIC, &now);

  // Time difference in usec
  diff = (now.tv_sec - last.tv_sec) * 1000000 + (now.tv_nsec - last.tv_nsec) / 1000;

  // Filter jitter
  if (diff > IGNORE_CHANGE_BELOW_USEC)
  {
    /* transition to the next minute*/
    if (diff/1000 > 1500)
    {
      update_current_time();
      printf("*new minute* %lu\n", diff/1000);
    
      memset((void*)&time_vec, 0x00, sizeof(time_vec));
      sec = 0;
    }

    if (state)
    { 
      /* falling edge */
      d_t = (int)(now.tv_sec * 1000 + now.tv_nsec/1000000) - t_rise;
      printf("dt[ms]\t%d\t", d_t);
    
      if ( (d_t > 150u) && (ClkState >= init) )
      {
        /* found a '1' */
        time_vec |= ((long long)1 << sec);
      }
      
      /* show current time */
      print_to_console(0, NULL, NULL );
      
      /* increment seconds counter */
      sec++;
      
      /* start timer for 59th second */
      if (59 == sec)
      {
        start_timer();
      }
      
    }
    else
    { 
      /* rising edge */
      t_rise = (int)(now.tv_sec * 1000 + now.tv_nsec/1000000);
    }

    state = !state;
  }
  else
  {
    printf("jitter ....\n");
  }

  last = now;
}


/* @brief    update_current_time: read time from received vector
 *
 * @param    void
 *
 */
static void update_current_time(void)
{
/*
    long variable -> 8 byte = 64 bit

    0x000000000000000
    0x000000 0    0    0    0    0    0    0    0    0

    hour:
    ... 0000 0XXx xxx0 0000 0000 0000 0000 0000 0000 0000
    ... |    |    |    |    |    |    |    |    |    |
    ... 39   35   31   27   23   19   15   11   7    3

    X: 0x0000000600000000 >> 8*4+1 = 33

    x: 0x00000001E0000000 >> 7*4+1 = 29

    minute:
    ... 0000 0000 0000 XXXx xxx0 0000 0000 0000 0000 0000
    ... |    |    |    |    |    |    |    |    |    |
    ... 39   35   31   27   23   19   15   11   7    3

    X: 0x000000000E000000 >> 5*4+1 = 25

    x: 0x0000000001E00000 >> 6*4+1 = 21
*/


  if (ClkState >= init)
  { 
    /* synchronised */
    
    // '0'      (time_vec & 0x0000000000000001)
    // 1-14 weather information
    // 15 Rufbit
    // 16 flag anouncing daylight saving switching
    // 17 CEST
    // 18 CET
    // 19 Schaltsekunde
    // '1'      (time_vec & 0x0000000000100000) >> 20
    min  =      (time_vec & 0x0000000001E00000) >> 21;
    min  += 10*((time_vec & 0x000000000E000000) >> 25);
    // parity   (time_vec & 0x0000000010000000) >> 28
    
    hour =      (time_vec & 0x00000001E0000000) >> 29;
    hour += 10*((time_vec & 0x0000000600000000) >> 33);
    // parity   (time_vec & 0x0000000800000000) >> 35
    
    day  =      (time_vec & 0x000000F000000000) >> 36;
    day  += 10*((time_vec & 0x0000030000000000) >> 40);
    
    wday =      (time_vec & 0x00001C0000000000) >> 42;
    
    mnth =      (time_vec & 0x0001E00000000000) >> 45;
    mnth += 10*((time_vec & 0x0002000000000000) >> 49);
    
    year =      (time_vec & 0x003C000000000000) >> 50;
    year += 10*((time_vec & 0x03C0000000000000) >> 54);
    // parity   (time_vec & 0x0400000000000000) >> 58
    
    
    ClkState = started;
  }
  else if (ClkState == unsynced)
  { /* 1. first minute transition */
    ClkState = init;
  }
  else
  {
    /* tbd */
  }

}



/*********************************************
** MAIN
*********************************************/
int main(void)
{
  // init signal handler for 59th second
  init_timer();

  // Init wiringPi lib
  wiringPiSetup();

  // Set pin to output in case it's not
  pinMode(PIN, OUTPUT);

  // Time now
  clock_gettime(CLOCK_MONOTONIC, &last);

  // Bind to interrupt
  wiringPiISR(PIN, INT_EDGE_BOTH, &handle);

  // Get initial state of pin
  state = digitalRead(PIN);

  if (state) {
    printf("Started! Initial state is on\n");
  }
  else {
    printf("Started! Initial state is off\n");
  }

  // Waste time but not CPU
  for (;;) 
  {
    sleep(1);
  }
}/**** main ****/
