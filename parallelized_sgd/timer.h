/*
 * Timer
 *   Object to measure runtime information.
 *   "real" time is the wall-clock time, as observed externally.
 *   "user" time is the amount of CPU time spent in user mode.
 *   "sys"  time is the amount of CPU time spent in system/kernel mode.
 *   "process" scope tracks the above stats for the entire process.
 *   "thread" scope tracks the above stats for the calling thread.
 */

#ifndef _timer_hpp_
#define _timer_hpp_


#ifdef __linux__
#include <time.h>
#endif // __linux__
#ifdef __APPLE__
#endif // __APPLE__
#include <sys/time.h>
#include <sys/resource.h>


#define TIMER_STATUS_RUNNING	1
#define TIMER_STATUS_PAUSED		2
#define TIMER_SCOPE_PROCESS		5
#define TIMER_SCOPE_THREAD		6
typedef struct _timer_t {
	int status; // running/paused
	int scope; // process/thread
	struct timespec *start_time_tspec; // real times
	struct rusage *start_time_rusg; // user/sys times
	long long real_cumulative; // nanoseconds
	long long user_cumulative; // microseconds
	long long sys_cumulative; // microseconds
} ttimer_t;


typedef struct _timerstats_t {
	// The "wall clock" time, as viewed external to the process.
	double real; // seconds
	// The amount of CPU time spent in user mode.
	double user; // seconds
	// The amount of CPU time spent in system (kernel) mode.
	double sys; // seconds
} timerstats_t;


// Initializes the timer struct
int timer_initialize(ttimer_t *timer, int scope);

// Deinitializes the timer struct
int timer_deinitialize(ttimer_t *timer);

// Starts/resumes the timer.
//    The timer must be paused.
int timer_start(ttimer_t *timer);

// Pauses/stops the timer.
//    If timer is already paused, no op occurs.
int timer_pause(ttimer_t *timer);

// Returns the timer's stats at the current state.
//    The timer must be paused.
int timer_get_stats(ttimer_t *timer, timerstats_t *stats);

// Returns time elapsed in seconds.
double timer_get_elapsed(long long cumulative_start, long long cumulative_end);

// Returns the timer's stats for the internal gradient
//   and coordinate update timers.
//   (Implemented by HOGWILD! module only)
int timer_get_internal_timer_stats(timerstats_t *gradient_stats, timerstats_t *coord_update_stats);


#endif // _timer_hpp_
