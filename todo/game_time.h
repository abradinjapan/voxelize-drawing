#ifndef VOXELIZE__time
#define VOXELIZE__time

/* Include */
// basic
#include "../base/basic.h"

// time
#if defined(__linux__)
#include <sys/time.h>
#endif

/* Define */
// time fragments
typedef u64 TIME__seconds;
typedef u64 TIME__milliseconds;
typedef u64 TIME__delta__u64;
typedef u32 TIME__delta__f32;

// milliseconds per second
#define TIME__milliseconds_per_second 1000000

// time
typedef struct TIME__time {
    TIME__seconds p_seconds;
    TIME__milliseconds p_milliseconds;
} TIME__time;

/* Time */
#if defined(__linux__)
TIME__time BASIC__fetch__time() {
    TIME__time output;
    struct timeval current_time;

    // get time
    gettimeofday(&current_time, 0);

    // setup output
    output.p_seconds = (TIME__seconds)current_time.tv_sec;
    output.p_milliseconds = (TIME__milliseconds)current_time.tv_usec;

    return output;
}
#endif

/* Interval */
// time interval
typedef struct TIME__interval {
    TIME__time p_start;
    TIME__time p_end;
} TIME__interval;

// setup custom interval
TIME__interval TIME__create__interval(TIME__time start, TIME__time end) {
    TIME__interval output;

    // setup output
    output.p_start = start;
    output.p_end = end;

    return output;
}

// set the current time to last time and the new time to current time
TIME__interval TIME__next__interval(TIME__interval interval, TIME__time new_time) {
    // set new past time and discard oldest one
    interval.p_end = interval.p_start;

    // set new time
    interval.p_start = new_time;

    return interval;
}

/* Frame Delta */
// calculate frame interval as a u64 integer
TIME__delta__u64 TIME__calculate__delta__u64(TIME__interval between_frame_interval) {
    // calculate
    return ((between_frame_interval.p_end.p_seconds * TIME__milliseconds_per_second) + between_frame_interval.p_end.p_milliseconds) - ((between_frame_interval.p_start.p_seconds * TIME__milliseconds_per_second) + between_frame_interval.p_start.p_milliseconds);
}

#endif
