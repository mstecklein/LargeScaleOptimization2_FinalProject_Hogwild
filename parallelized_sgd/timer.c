#include <stdlib.h>
#include "timer.h"


//  This module uses clock_gettime to record real times and 
//  uses getrusage to record user/sys times. See their man
//  pages for more info:
//  http://man7.org/linux/man-pages/man2/clock_gettime.2.html
//  http://man7.org/linux/man-pages/man2/getrusage.2.html


int timer_initialize(ttimer_t *timer, int scope) {
	timer->status = TIMER_STATUS_PAUSED;
	timer->scope = scope;
	timer->real_cumulative = 0;
	timer->user_cumulative = 0;
	timer->sys_cumulative = 0;
	timer->start_time_tspec = (struct timespec *) malloc(sizeof(struct timespec));
	timer->start_time_rusg = (struct rusage *) malloc(sizeof(struct rusage));
	return 0;
}


int timer_deinitialize(ttimer_t *timer) {
	free(timer->start_time_tspec);
	free(timer->start_time_rusg);
	return 0;
}


int timer_start(ttimer_t *timer) {
	if (timer->status != TIMER_STATUS_PAUSED)
		return -1;
	// Get start times
	int rc;
	rc = clock_gettime(CLOCK_MONOTONIC, timer->start_time_tspec);
	if (rc)
		return rc;
#ifdef __linux__
	int who = (timer->scope == TIMER_SCOPE_PROCESS) ? RUSAGE_SELF : RUSAGE_THREAD;
#endif // __linux__
#ifdef __APPLE__
	int who = RUSAGE_SELF;
#endif // __APPLE__
	rc = getrusage(who, timer->start_time_rusg);
	if (rc)
		return rc;
	// Timer is now "running"
	timer->status = TIMER_STATUS_RUNNING;
	return 0;
}


int timer_pause(ttimer_t *timer) {
	if (timer->status != TIMER_STATUS_RUNNING)
		return 0;
	// Read current times
	int rc;
	struct rusage curr_rusg;
	struct timespec curr_tspec;
	rc = clock_gettime(CLOCK_MONOTONIC, &curr_tspec);
	if (rc)
		return rc;
#ifdef __linux__
	int who = (timer->scope == TIMER_SCOPE_PROCESS) ? RUSAGE_SELF : RUSAGE_THREAD;
#endif // __linux__
#ifdef __APPLE__
	int who = RUSAGE_SELF;
#endif // __APPLE__
	rc = getrusage(who, &curr_rusg);
	if (rc)
		return rc;
	// Add diff since start times to the accumulator
	long long real_diff = (curr_tspec.tv_sec - timer->start_time_tspec->tv_sec)*1000000000L + (curr_tspec.tv_nsec - timer->start_time_tspec->tv_nsec);
	long long user_diff = (curr_rusg.ru_utime.tv_sec - timer->start_time_rusg->ru_utime.tv_sec)*1000000L + (curr_rusg.ru_utime.tv_usec - timer->start_time_rusg->ru_utime.tv_usec);
	long long sys_diff = (curr_rusg.ru_stime.tv_sec - timer->start_time_rusg->ru_stime.tv_sec)*1000000L + (curr_rusg.ru_stime.tv_usec - timer->start_time_rusg->ru_stime.tv_usec);
	timer->real_cumulative += real_diff;
	timer->user_cumulative += user_diff;
	timer->sys_cumulative  += sys_diff;
	// Timer is now "paused"
	timer->status = TIMER_STATUS_PAUSED;
	return 0;
}


int timer_get_stats(ttimer_t *timer, timerstats_t *stats) {
	if (timer->status != TIMER_STATUS_PAUSED)
		return -1;
	stats->real = (timer->real_cumulative) / ((double) 1000000000L);
	stats->user = (timer->user_cumulative) / ((double) 1000000L);
	stats->sys  = (timer->sys_cumulative)  / ((double) 1000000L);
	return 0;
}


double timer_get_elapsed(long long cumulative_start, long long cumulative_end) {
	return (cumulative_end - cumulative_start ) / ((double) 1000000000L);
}
