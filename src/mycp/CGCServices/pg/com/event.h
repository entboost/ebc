/*
 * $Id: event.h,v 1.22 2003/10/16 18:50:54 jasta Exp $
 *
 * Copyright (C) 2001-2003 giFT project (gift.sourceforge.net)
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

#ifndef __GIFT_EVENT_H__
#define __GIFT_EVENT_H__

/*****************************************************************************/

/**
 * @file event.h
 *
 * @brief Main select event loop.
 *
 * Provides the low-level framework for giFT file descriptor (yes, that
 * includes sockets) handling.
 */

/*****************************************************************************/

#include <sys/types.h>

/*****************************************************************************/

/* time specifications useful for timer_add calls, which are in msec
 * precision */
#define MSEC         (1)
#define SECONDS      (1000*MSEC)
#define MINUTES      (60*SECONDS)

/* time specs useful for comparing seconds since the epoch */
#define ESECONDS     (1)
#define EMINUTES     (60*ESECONDS)
#define EHOURS       (60*EMINUTES)
#define EDAYS        (24*EHOURS)

/* see input_add */
#define TIMEOUT_DEF  (60*SECONDS)

/*****************************************************************************/

typedef unsigned int input_id;         /**< Input handle */
typedef unsigned int timer_id;         /**< Timer handle */

/*****************************************************************************/

/**
 * Callback used to indicate that the connection has changed state according
 * to the ::input_add state.
 */
typedef void (*InputCallback) (int fd, input_id id, void *udata);

/**
 * Available select sets.
 */
typedef enum
{
	INPUT_READ  = 0x01,
	INPUT_WRITE = 0x02,
	INPUT_ERROR = 0x04
} InputState;

/*****************************************************************************/

/**
 * Timer has reached zero.
 *
 * @param udata Arbitrary user data.
 *
 * @retval FALSE Clear and remove from event loop.
 * @retval TRUE  Reset this timer.
 */
typedef int (*TimerCallback) (void *udata);

/*****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************/

/**
 * Initialize the event subsystem.  This is mandatory and must be called
 * prior to any events being scheduled into the main loop.
 */
 
  void event_init (void);

/**
 * Clear all events and return from the main loop.  This will not exit your
 * program, merely break out of ::event_loop, which should be the core
 * blocking entity.
 *
 * @param sig  Signal delivered that caused this interruption.  This value is
 *             currently unused and may disappear entirely.
 */
 
  void event_quit (int sig);

/**
 * Execute a single blocking poll and handle exactly one event.
 */
 
  void event_poll_once (void);

/**
 * Begin the main event loop.  This will block until there are no fds or
 * timers remaining or until ::event_quit has been called.
 */
 
  void event_loop (void);

/*****************************************************************************/

/**
 * Add a new fd input.
 *
 * @param fd
 * @param udata     Arbitrary user data to pass along to \em callback.
 * @param state     Set to examine.
 * @param callback  Function to raise when the examined set detects a change.
 * @param timeout   If non-zero, the fd will be checked for input.  If the
 *                  callback is not raised in the specified time interval,
 *                  the socket will be closed and the callback raised with
 *                  a closed socket.
 *
 * @return Unique input handle.  You may use this to remove a specific input
 *         or use the file descriptor to remove a group of inputs.
 */
 
  input_id input_add (int fd, void *udata, InputState state,
                      InputCallback callback, time_t timeout);

/**
 * Remove a single input by it's unique identifier.
 */
 
  void input_remove (input_id id);

/**
 * Remove all inputs registered from a single file descriptor.  This is
 * useful when you want to lazily clean up all your slop, as giFT certainly
 * desires.  The usage of this function is highly discouraged unless you know
 * that the socket is no longer connected as it will interfere with queue
 * systems.
 */
 
  void input_remove_all (int fd);

/**
 * Suspend all inputs registered to this connection from the select
 * loop.  This is useful for bandwidth throttling in user space and some
 * other evil things.
 */
 
  void input_suspend_all (int fd);

#if 0
/**
 * Check if the given fd has suspended inputs.
 */
 
  int input_suspended_all (int fd);
#endif

/**
 * Move the inputs back into the select loop.  See ::input_suspend_all.
 */
 
  void input_resume_all (int fd);

/*****************************************************************************/

/**
 * Install a new timer event.
 *
 * @param interval  Time to wait.  Expressed in milliseconds.  See ::SECONDS
 *                  and ::MINUTES for helpers.
 * @param callback  Function to be called when the interval is satisfied.
 * @param udata     Arbitrary user data.
 *
 * @return Unique timer identifier on success, otherwise zero.
 */
 
  timer_id timer_add (time_t interval, TimerCallback callback, void *udata);

/**
 * Reset a timer.  That is, reset the expiration time to now + interval as if
 * the timer was registered for the first time.
 */
 
  void timer_reset (timer_id id);

/**
 * Remove a timer.  This clears all data associated with the timer.
 * The callback will not be raised.
 */
 
  void timer_remove (timer_id id);

/**
 * Wrapper for ::timer_remove which assigns the value at id to zero after
 * removal to invalidate the identifier.
 */
 
  void timer_remove_zero (timer_id *id);

/*****************************************************************************/

int event_gettimeofday (struct timeval *tv, void *unused);
unsigned long event_net_errno (void);



#ifdef __cplusplus
}
#endif

/*****************************************************************************/

#endif /* __GIFT_EVENT_H__ */
