/*
 * $Id: event.c,v 1.88 2004/09/09 11:31:48 mkern Exp $
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

#ifdef WIN32
#include "config_win32.h"
#else
#include "config_unix.h"
#include <time.h>
#include <sys/time.h>
#endif

#include <errno.h>

#include "list.h"
#include "array.h"
#include "dataset.h"
//#include "com/log.h"

//#include "network.h"
#include "event.h"
//#include "tcpc.h"



#ifdef HAVE_POLL
# include <sys/poll.h>
#else

struct pollfd1
{
	int   fd;
	short events;
	short revents;
};

# define POLLIN   0x0001
# define POLLOUT  0x0004
# define POLLERR  0x0008
# define POLLHUP  0x0010
# define POLLNVAL 0x0020

#endif /* HAVE_POLL */

/*****************************************************************************/

/**
 * File descriptor handler.
 */
typedef struct
{
	int           fd;                  /**< file descriptor */
	input_id      id;
	input_id      poll_id;
	InputState    set;

	InputCallback cb;
	void         *udata;               /**< arbitrary data associated with
	                                    *   this input fd */
	time_t        timeout;             /**< see ::input_add */
	timer_id      validate;            /**< timeout for incomplete sockets */
	unsigned char complete : 1;        /**< complete is 0 until first
	                                    *   state change.  used for
	                                    *   automated connection timeouts. */
	unsigned char suspended : 1;       /**< suspended is 1 when input
	                                    *   should not be put in select loop */
	signed char   dirty : 2;           /**< write queue */
} Input;

/**
 * Timer event.
 */
typedef struct timer
{
	timer_id       id;                 /**< index in timers array */
	List          *link;               /**< link chain in timers_sorted */

	int           used;               /**< active or inactive, used
	                                    *   internally to look for new ids */
	signed char    dirty : 2;          /**< if 1 or -1 don't remove timer */

	struct timeval expiration;         /**< exact time this timer will
	                                    *   expire */
	struct timeval interval;           /**< interval this timer is set for */

	TimerCallback  cb;
	void          *udata;              /**< arbitrary user data */
} Timer;

/*****************************************************************************/
/* blatantly ripped from sys/time.h for reasons somewhat beyond me */

#define TIME_LT  <
#define TIME_GT  >
#define TIME_EQ  ==

#define TIME_SET(t,m)                                       \
	do                                                      \
	{                                                       \
		(t)->tv_sec = m / 1000;                             \
		(t)->tv_usec = (m % 1000) * 1000;                   \
	} while (0)

#define TIME_CMP(t,v,__OP__)                                \
	(((t)->tv_sec == (v)->tv_sec) ?                         \
	 ((t)->tv_usec __OP__ (v)->tv_usec) :                   \
	 ((t)->tv_sec __OP__ (v)->tv_sec))

#define TIME_ADD(t,v,r)                                     \
	do                                                      \
	{                                                       \
		(r)->tv_sec  = (t)->tv_sec + (v)->tv_sec;           \
		(r)->tv_usec = (t)->tv_usec + (v)->tv_usec;         \
		if ((r)->tv_usec >= 1000000)                        \
		{                                                   \
			++(r)->tv_sec;                                  \
			(r)->tv_usec -= 1000000;                        \
		}                                                   \
	} while (0)

#define TIME_ADDI(t,i,r)                                    \
	do                                                      \
	{                                                       \
		(r)->tv_sec  = (t)->tv_sec;                         \
		(r)->tv_usec = (t)->tv_usec + (i);                  \
		if ((r)->tv_usec >= 1000000)                        \
		{                                                   \
			(r)->tv_sec += (r)->tv_usec / 1000000;          \
			(r)->tv_usec %= 1000000;                        \
		}                                                   \
	} while (0)

#define TIME_SUB(t, v, r)                                   \
	do                                                      \
	{                                                       \
		(r)->tv_sec = (t)->tv_sec - (v)->tv_sec;            \
		(r)->tv_usec = (t)->tv_usec - (v)->tv_usec;         \
		if ((r)->tv_usec < 0) {                             \
			--(r)->tv_sec;                                  \
			(r)->tv_usec += 1000000;                        \
		}                                                   \
	} while (0)

#define TIME_CLEAR(t) ((t)->tv_sec = (t)->tv_usec = 0)
#define TIME_ISSET(t) ((t)->tv_sec ? 1 : (t)->tv_usec)

/*****************************************************************************/

/* this is used for error reporting only */
#ifdef HAVE_POLL
# define POLLFN "poll"
#else
# define POLLFN "select"
#endif

/* if you feel like being brave, change to #if 1 */
#if 0
# define POLL_TRACE(args) TRACE(args)
#else
# define POLL_TRACE(args)
#endif

static volatile int  polling       = 0; /* keeps event_loop alive */

static input_id      input_ids     = 0;     /* number of inputs active */
static input_id      input_ids_max = 0;     /* absolute highest input id */
static input_id      poll_ids      = 0;     /* same as input_ids for now */

#define              MAX_INPUTS      4096   /* maximum number of inputs */

static struct pollfd1 poll_fds[MAX_INPUTS];  /* poll argument */
static Input         inputs  [MAX_INPUTS];  /* maintain all inputs */

static Array        *inputs_add    = NULL;  /* add queue */
static Array        *inputs_remove = NULL;  /* remove queue */

static Dataset      *fds           = NULL;  /* index of input ids by fd */

/*****************************************************************************/

static Timer        *timers        = NULL;  /* main data structure, indices
                                             * are returne id - 1 */
static timer_id      timers_len    = 0;     /* total number of active timers */
static timer_id      timers_size   = 0;     /* total number of allocd timers */
#define              TIMERS_SIZE     1024   /* default size */

static List         *timers_sorted = NULL;  /* sorted by expiration (first
                                             * to expire, first in list) */

/*****************************************************************************/
int event_gettimeofday (struct timeval *tv, void *unused)
{
	int ret;

#ifndef WIN32
	ret = gettimeofday (tv, NULL);
#else /* WIN32 */
	struct _timeb tb;

	/* TODO: assign ret? */
	_ftime (&tb);

	tv->tv_sec  = tb.time;
	tv->tv_usec = tb.millitm * 1000;

	ret = 0;
#endif /* !WIN32 */

	return ret;
}

unsigned long event_net_errno (void)
{
#ifndef WIN32
	return (unsigned long)errno;
#else /* WIN32 */
	/* F*cking windows has a different error function for socket functions. */
	return (unsigned long)WSAGetLastError();
#endif /* !WIN32 */
}


/*
 * Perl representation of this data structure to ease understanding.
 *
 * my $fds = { fd3 => { id1 => 'id',
 *                      id2 => 'id',
 *                      id3 => 'id' },
 *             fd4 => { id4 => 'id' },
 *             fd5 => { id5 => 'id',
 *                      id6 => 'id' } };
 *
 * NOTE:
 *
 * We are using dynamic allocation for the fd index as it is faulty to assume
 * that file descriptors can be the index to a staticly sized array in a
 * portable fashion.
 */
static Dataset *get_fd_index (int fd)
{
	assert (fd >= 0);

	return dataset_lookup (fds, &fd, sizeof (fd));
}

static void foreach_fd_index (int fd, DatasetForeachExFn func, void *udata)
{
	Dataset *ids;

	if (!(ids = get_fd_index (fd)))
		return;

	dataset_foreach_ex (ids, func, udata);
}

static void add_fd_index (int fd, input_id id)
{
	Dataset *ids = NULL;

	if (!(ids = get_fd_index (fd)))
	{
		if (!(ids = dataset_new (DATASET_LIST)))
			return;

		if (!fds)
			fds = dataset_new (DATASET_HASH);

		dataset_insert (&fds, &fd, sizeof (fd), ids, 0);
	}

	dataset_insert (&ids, &id, sizeof (id), "id", 0);
}

static void del_fd_index (int fd, input_id *id)
{
	Dataset      *ids = NULL;
	unsigned long ids_left = 0;

	if (!(ids = get_fd_index (fd)))
		return;

	/*
	 * If a valid id was specified, remove exactly that id.  If this happened
	 * to be the last id left for this fd, remove the fd as well.  In the
	 * event that an invalid id is supplied, ids_left will be left
	 * initialized to 0 to indicate that the set should be cleared and
	 * removed regardless of remaining entries.
	 */
	if (id)
	{
		dataset_remove (ids, id, sizeof (*id));
		ids_left = dataset_length (ids);
	}

	if (ids_left == 0)
	{
		dataset_clear (ids);
		dataset_remove (fds, &fd, sizeof (fd));
	}
}

/*****************************************************************************/

/*
 * Having this functionality in event.c was a mistake, but I haven't had the
 * time/energy to move to tcpc.[ch] and deal with all the backwards
 * compatability issues.  Lots of code now depends on this, and not all of it
 * written by myself :(
 */
static int validate_timeout (Input *input)
{
	InputCallback cb;
	void         *udata;

	/* set this just in case this input manages to find its way back into
	 * some of the functions here while its being destroyed */
	input->complete = 1;
	input->validate = 0;

	/* the input is long since dead, but this timer is still around? */
	if (input->fd < 0)
	{
		/*
		GIFT_TRACE (("%d, %d [%d]: FIXME",
		             input->fd, input->id, input->validate));
	    */
		return 0;
	}

	/* required to call `cb' after input_remove_all */
	cb    = input->cb;
	udata = input->udata;

	/*
	 * Cleanup this descriptor and implicitly remove from the loop as a
	 * simple helper.  We then raise the callback to notify the caller that
	 * the descriptor has reached the maximum alloted time to respond.
	 */
#ifdef WIN32
	closesocket(input->fd);
#else
	close (input->fd);
#endif
	input_remove_all (input->fd);

	/*
	 * Raise the requested callback with an invalid socket and input id, as
	 * they are both invalid now.  This is a safe way for the caller to
	 * differentiate between a timeout and some other socket error.
	 */
	cb (-1, 0, udata);

	/* do not renew the timer, it's meant to be a one-shot deal */
	return 0;
}

static input_id next_input_id (void)
{
	input_id id;

	for (id = 0 /* input_ids */; inputs[id].fd >= 0; id++)
		assert (id < MAX_INPUTS);

	if (input_ids_max < id)
		input_ids_max = id;

	input_ids++;
	return id;
}

static input_id next_poll_id (void)
{
	return poll_ids++;
}

static void input_add_queue (Input *input)
{
	/* this input was removed before the dirty flag was unset, so simply
	 * ignore since we know input_remove_queue will tackle the rest */
	if (input->dirty < 0)
		return;

	assert (input->fd >= 0);
	assert (input->dirty > 0);
	assert (poll_fds[input->poll_id].fd == input->fd);

	input->dirty = 0;
}

static void set_pollfd (input_id pid, Input *input)
{
	struct pollfd1 *pfd;

	assert (input->poll_id == pid);
	pfd = &poll_fds[pid];

	pfd->fd      = input->fd;
	pfd->events  = 0;
	pfd->revents = 0;

	if (input->set & INPUT_READ)
		pfd->events |= POLLIN;

	if (input->set & INPUT_WRITE)
		pfd->events |= POLLOUT;

	if (input->set & INPUT_ERROR)
		pfd->events |= POLLERR;
}

input_id input_add (int fd, void *udata, InputState set, InputCallback cb,
                    time_t timeout)
{
	Input   *input;
	input_id id;
	input_id pid;

	if (fd < 0)
		return 0;

	if (input_ids >= MAX_INPUTS)
	{
/*
		GIFT_TRACE (("maximum input count exceeded"));
*/
		return 0;
	}

	id = next_input_id();
	pid = next_poll_id();

	POLL_TRACE (("%d => %d, fd = %d", id, pid, fd));

	input = &inputs[id];
	input->dirty   = 1;
	input->id      = id;
	input->poll_id = pid;
	input->fd      = fd;
	input->udata   = udata;
	input->set     = set;
	input->cb      = cb;
	input->timeout = timeout;

#if 0
	/* temporary hack to ensure that i don't miss any updates to the new api */
	assert (timeout != 1);
#endif

	if (!timeout)
	{
		input->complete = 1;
		input->validate = 0;
	}
	else
	{
		input->complete = 0;
		input->validate =
		    timer_add (timeout, (TimerCallback)validate_timeout, input);
	}

	/* maintain an index of all identifiers for a specific file descriptor to
	 * prevent fatal errors when using the api */
	add_fd_index (input->fd, input->id);

	/* after the next pass to poll, iterate through this write queue and
	 * remove the dirty elements from this input and finish it's population
	 * into the poll_fds array */
	array_push (&inputs_add, input);

	/* this is somewhat dangerous to do while the input is still dirty, but
	 * it is required to keep up the efficiency improvements when removing
	 * inputs, even if they are dirty */
	set_pollfd (pid, input);

	/* zero is an invalid id as far as the interface is concerned, but as an
	 * implementation it is perfectly valid */
	return id + 1;
}

static int eq_input (Input *input, input_id poll_id)
{
	return (input->poll_id == poll_id);
}

static int match_input (ds_data_t *key, ds_data_t *value, input_id *poll_id)
{
	input_id id = *((input_id *)key->data);

	return eq_input (&inputs[id], *poll_id);
}

static Input *find_input (input_id poll_id, input_id *retid)
{
	Dataset     *ids;
	DatasetNode *node;
	input_id     id;

	if (!(ids = get_fd_index (poll_fds[poll_id].fd)))
	{
		GIFT_TRACE (("unable to locate fd index %d", poll_fds[poll_id].fd));
		return NULL;
	}

	if (!(node = dataset_find_node (ids, DS_FIND(match_input), &poll_id)))
	{
		GIFT_TRACE (("unable to locate id %d in fd index %d",
		             poll_id, poll_fds[poll_id].fd));
		return NULL;
	}

	id = *((input_id *)node->key->data);

	if (retid)
		*retid = id;

	return &inputs[id];
}

static void move_pollfd (input_id dst_id, input_id src_id)
{
	Input         *input;
	struct pollfd1 *dst = &poll_fds[dst_id];
	struct pollfd1 *src = &poll_fds[src_id];

	/* if the id's are identical, we still want to invalidate the source, but
	 * we can optimize away the lookup and writes */
	if (dst_id != src_id)
	{
		/* update the poll_id (used for unique correlation between inputs and
		 * pollfds) */
		if ((input = find_input (src_id, NULL)))
		{
			POLL_TRACE (("adjusting %d to pollid %d", input->id, dst_id));
			input->poll_id = dst_id;
		}

		/* move the memory */
		dst->fd      = src->fd;
		dst->events  = src->events;
		dst->revents = 0;
	}

#ifdef DEBUG
	memset (src, 0, sizeof (struct pollfd1));
#endif /* DEBUG */

	/* nullify the old location */
	src->fd = -1;
}

static void remove_pollfd (input_id poll_id)
{
	assert (poll_id >= 0);
	assert (poll_id < poll_ids);

	/* swap the last entry in the poll_fds array with the one being deleted so
	 * that we do not need to rebuild the entire array after each removal */
	move_pollfd (poll_id, --poll_ids);
}

static void input_remove_queue (Input *input)
{
	assert (input->fd >= 0);
	assert (input->dirty < 0);

#ifdef DEBUG
	memset (input, 0, sizeof (*input));
#endif /* DEBUG */

	input->dirty = 0;
	input->fd = -1;
}

static void remove_full (input_id id, int delete_idx)
{
	assert (inputs[id].fd >= 0);

	/* already removed, bow out gracefully */
	if (inputs[id].dirty < 0)
		return;

	/* decrement the input id counter */
	input_ids--;

	if (input_ids_max == id)
	{
		/* cant use MAX() here because id - 1 will not be less than 0 when
		 * id is 0 */
		if (id == 0)
			input_ids_max = 0;
		else
			input_ids_max = id - 1;
	}

	/* remove this input from the global poll array */
	if (!inputs[id].suspended)
		remove_pollfd (inputs[id].poll_id);

	/* it's ok to delete the fd indexes for removed sources, we just need
	 * to be careful with find_input calls */
	if (delete_idx)
		del_fd_index (inputs[id].fd, &id);

	/* remove the validate timer if it exists */
	if (inputs[id].validate)
		timer_remove_zero (&(inputs[id].validate));

	inputs[id].dirty = -1;

	/* schedule this entry for complete removal */
	array_push (&inputs_remove, &inputs[id]);
}

void input_remove (input_id id)
{
	if (id == 0)
		return;

	/* subtract one for API reasons...see ::input_add */
	remove_full (id - 1, 1);
}

static int remove_by_fd (ds_data_t *key, ds_data_t *value, int *fd)
{
	input_id *id = key->data;

	/* it is important that we not disturb the index while we are walking
	 * along it, obviously */
	remove_full (*id, 0);

	return DS_CONTINUE | DS_REMOVE;
}

void input_remove_all (int fd)
{
	if (fd < 0)
		return;

	/* remove each individual entry, maintaining the index so that it may
	 * be removed more efficiently after all removals have taken place */
	foreach_fd_index (fd, DS_FOREACH_EX(remove_by_fd), &fd);
	del_fd_index (fd, NULL);
}

static void input_suspend (Input *input)
{
	if (input->suspended)
		return;

	remove_pollfd (input->poll_id);

	input->poll_id   = 0;
	input->suspended = 1;

	if (input->validate)
		timer_remove_zero (&input->validate);
}

static int suspend_by_fd (ds_data_t *key, ds_data_t *value, int *fd)
{
	input_id *id = key->data;

	input_suspend (&inputs[*id]);

	return DS_CONTINUE;
}

void input_suspend_all (int fd)
{
	if (fd < 0)
		return;

	foreach_fd_index (fd, DS_FOREACH_EX(suspend_by_fd), &fd);
}

static void input_resume (Input *input)
{
	if (!input->suspended)
		return;

	assert (input->validate == 0);
	assert (input->poll_id == 0);

	/* acquire a new poll id */
	input->poll_id   = next_poll_id();
	input->suspended = 0;

	/* add the validate timer back if necessary */
	if (!input->complete && input->timeout)
	{
		assert (input->validate == 0);

		input->validate =
		    timer_add (input->timeout, (TimerCallback)validate_timeout, input);
	}

	/* reset the poll_fds data */
	set_pollfd (input->poll_id, input);
}

static int resume_by_fd (ds_data_t *key, ds_data_t *value, int *fd)
{
	input_id *id = key->data;

	input_resume (&inputs[*id]);

	return DS_CONTINUE;
}

void input_resume_all (int fd)
{
	if (fd < 0)
		return;

	foreach_fd_index (fd, DS_FOREACH_EX(resume_by_fd), &fd);
}

/*****************************************************************************/

static void time_current (struct timeval *curr_time)
{
	/* this abstraction is provided so that we may one day cache the value to
	 * avoid unnecessary gettimeofday calls */
	event_gettimeofday (curr_time, NULL);
}

static Timer *timer_find (timer_id id)
{
	Timer *timer;

	timer = &timers[id];

	if (!timer->used)
		return NULL;

	assert (timer->link != NULL);
	return timer;
}

static timer_id next_timer_id (void)
{
	timer_id id;
	timer_id start;

	/* begin searching at a random index in the currently allocated array */
	start = ((int)((float)timers_size * rand() / (RAND_MAX + 1.0)));
	assert (start >= 0);
	assert (start < timers_size);

	id = start;

	while (timers[id].used)
	{
		if (++id >= timers_size)
			id = 0;
	}

#if 0
	GIFT_TRACE (("%u: %u to %u: %u traversed",
	             (unsigned int)timers_len,
	             (unsigned int)start, (unsigned int)id,
	             (unsigned int)(id - start)));
#endif

	return id;
}

static void zero_timers (timer_id start, size_t num)
{
	while (num-- > 0)
		timers[start++].used = 0;
}

static int resize_timers (void)
{
	void  *newalloc;
	size_t newsize;
	float  used;

	/* handle initial allocation */
	if (!timers || timers_size == 0)
	{
		assert (timers == NULL);
		assert (timers_size == 0);

		if (!(timers = malloc ((sizeof (Timer)) * TIMERS_SIZE)))
			return 0;

		zero_timers (0, TIMERS_SIZE);
		timers_size = TIMERS_SIZE;
	}

	used = ((float)timers_len / (float)timers_size);

	/* we want to maintain at least 50% availability of new ids */
	if (used > 0.7)
	{
		newsize = timers_size * 2;
		assert (newsize < RAND_MAX);

		/* double the current size */
		if (!(newalloc = realloc (timers, (sizeof (Timer)) * newsize)))
			return 0;

		timers = newalloc;

		/* zero only the new entries */
		zero_timers (timers_size, newsize - timers_size);
		timers_size = newsize;
	}

	return 1;
}

static Timer *timer_new (time_t interval, TimerCallback cb, void *udata)
{
	Timer         *timer;
	timer_id       id;
	struct timeval current_time;

	/* make sure we can hold a new timer */
	if (!(resize_timers ()))
		return NULL;

	id = next_timer_id ();
	assert (id >= 0);
	assert (id < timers_size);

	timer = &timers[id];
	timer->id    = id;
	timer->link  = NULL;
	timer->used  = 1;
	timer->cb    = cb;
	timer->udata = udata;
	timer->dirty = 0;

	/*
	 * TIME_SET corrects a problem with the old method of initializing timers
	 * (TIME_CLEAR, TIME_ADDI).  Timers on the order of magnitude of hours
	 * overflowed the tv_usec value resulting in timers with incorrect
	 * intervals.
	 */
	TIME_SET (&timer->interval, interval);

	/* determine the expiration time */
	time_current (&current_time);
	TIME_ADD (&current_time, &timer->interval, &timer->expiration);

	return timer;
}

static int sort_timer (timer_id *a_id, timer_id *b_id)
{
	Timer *a = &timers[*a_id];
	Timer *b = &timers[*b_id];

	if (TIME_CMP (&a->expiration, &b->expiration, TIME_GT))
		return 1;
	else if (TIME_CMP (&a->expiration, &b->expiration, TIME_LT))
		return -1;

	return 0;
}

/* this should be in list.c, but unfortunately it doesn't fit the API
 * so we would pretty much need to rewrite the whole thing for consistency */
static List *list_insort (List **list, CompareFunc cmpfn, void *data)
{
	List *ptr, *prev = NULL;
	List *link;
	int   ret;

	if (!list)
		return NULL;

	assert (cmpfn != NULL);

	if (!(*list))
	{
		*list = list_prepend (*list, data);
		return *list;
	}

	for (ptr = *list; ptr; prev = ptr, ptr = list_next (ptr))
	{
		ret = (*cmpfn) (ptr->data, data);

		/* keep searching until we find the first node >= the supplied
		 * node data */
		if (ret < 0)
			continue;

		assert (prev == ptr->prev);

		if (prev)
		{
			/*
			 * Insert the data immediately after the previous node, which is
			 * different as inserting immediately before the current node, as
			 * any prepend operation will not check for ptr->prev nor will it
			 * update it.  Since the link chain doesn't know whether or not
			 * the 1st index is the end of the list, it will have to check
			 * for ptr->next to function at all.
			 */
			list_insert (prev, 1, data);

			/* make sure we are getting the new updated entry */
			assert (prev->next != ptr);
			link = prev->next;
		}
		else
		{
			/*
			 * We know that the 0th entry is the place for this new node, and
			 * we also know that this will produce a new list head, so lets
			 * just use prepend for ease
			 */
			*list = list_prepend (*list, data);

			/* make sure we have a new head */
			assert (*list == ptr->prev);
			link = *list;
		}

		return link;
	}

	/* we never found a place for it, so just add it to the end, but do so
	 * without an inefficient call to list_append when we already have the
	 * last entry */
	assert (prev->next == NULL);
	list_insert (prev, 1, data);
	assert (prev->next != NULL);

	return prev->next;
}

static void insort_timer (Timer *timer)
{
	timer_id *id;

	assert (timer != NULL);

	if (!(id = dateset_memdup (&timer->id, sizeof (timer->id))))
		return;

	/* insert so that the closest expiration to the current time is placed
	 * first for efficiency purposes */
	timer->link = list_insort (&timers_sorted, (CompareFunc)sort_timer, id);
	assert (timer->link != NULL);
}

timer_id timer_add (time_t interval, TimerCallback cb, void *udata)
{
	Timer *timer;

	if (!(timer = timer_new (interval, cb, udata)))
		return 0;

	/* insert sorted so that the first entry in the sorted list is the
	 * closest to expiration */
	insort_timer (timer);

	/* increment the total number of active timers */
	timers_len++;

	return timer->id + 1;
}

void timer_reset (timer_id id)
{
	Timer         *timer;
	struct timeval current_time;

	if (id == 0)
		return;

	if (!(timer = timer_find (id - 1)))
		return;

	/* this timer has already been scheduled for removal by its callback,
	 * ignore it here */
	if (timer->dirty < 0)
		return;

	/* remove its current place in the sorted list */
	free (timer->link->data);
	timers_sorted = list_remove_link (timers_sorted, timer->link);

	/* set the new expiration */
	time_current (&current_time);
	TIME_ADD (&current_time, &timer->interval, &timer->expiration);

	/* put back in the new sorted position */
	insort_timer (timer);
}

static void remove_timer (Timer *timer)
{
	timer_id *id;

	assert (timer != NULL);
	assert (timer->id >= 0 && timer->id < timers_size);
	assert (timer->used == 1);
	assert (timer->link != NULL);
	assert (timer->link->data != NULL);

	if (timer->dirty)
	{
		/* we will remove the timer after the dirty flag is unset */
		timer->dirty = -1;
		return;
	}

	id = timer->link->data;
	assert (&timers[*id] == timer);

	/* TODO: resize timers array downward */

	/* one less active timer */
	timers_len--;

	timer->used = 0;

	/* free the duplicated id and remove the link */
	free (id);
	timers_sorted = list_remove_link (timers_sorted, timer->link);
}

void timer_remove (timer_id id)
{
	Timer *timer;

	if (id == 0)
		return;

	if (!(timer = timer_find (id - 1)))
		return;

	remove_timer (timer);
}

void timer_remove_zero (timer_id *id)
{
	if (id && *id)
	{
		timer_remove (*id);
		*id = 0;
	}
}

/*****************************************************************************/

void event_init (void)
{
	int i;

	/* initialize the random number generator for use by next_timer_id */
	srand (time (NULL));

	for (i = 0; i < MAX_INPUTS; i++)
	{
#ifdef DEBUG
		memset (&inputs[i], 0, sizeof (inputs[i]));
		memset (&poll_fds[i], 0, sizeof (poll_fds[i]));
#endif /* DEBUG */

		inputs[i].fd   = -1;
		poll_fds[i].fd = -1;
	}
}

void event_quit (int sig)
{
	polling = 0;
}

static int calc_timeout (Timer *t)
{
	struct timeval current_time;
	struct timeval timeout;
	int next_msec;

	time_current (&current_time);
	TIME_SUB(&t->expiration, &current_time, &timeout);

	/* get the next timeout in milliseconds instead of microseconds */
	next_msec = ((timeout.tv_sec * 1000) + (timeout.tv_usec / 1000));
	return next_msec;
}

static int event_poll (struct pollfd1 *pfds, unsigned int nfds, int timeout)
{
	int             ret;
#ifndef HAVE_POLL
	int             i;
	int             maxfd = -1;
	fd_set          rset;
	fd_set          wset;
	fd_set          xset;
	struct timeval  timeout_tv;
	struct timeval *tvp = NULL;
#endif /* !HAVE_POLL */

#ifdef HAVE_POLL
	ret = poll (pfds, nfds, timeout);
#else
	if (timeout >= 0)
	{
		timeout_tv.tv_sec = timeout / 1000;
		timeout_tv.tv_usec = (timeout % 1000) * 1000;
		tvp = &timeout_tv;
	}

	FD_ZERO (&rset);
	FD_ZERO (&wset);
	FD_ZERO (&xset);

	for (i = 0; i < nfds; i++)
	{
		if (pfds[i].events == 0)
			continue;

		if (pfds[i].fd > maxfd)
			maxfd = pfds[i].fd;

		if (pfds[i].events & POLLIN)
			FD_SET (pfds[i].fd, &rset);

		if (pfds[i].events & POLLOUT)
			FD_SET (pfds[i].fd, &wset);

#ifdef WIN32
		/* POSIX specifies that a failed non-blocking connect should signal a
		 * writable socket. Windows only signals this case through the except
		 * set so we add the fd there as well if a write signal is requested. 
		 */
		if (pfds[i].events & (POLLERR | POLLOUT))
#else
		if (pfds[i].events & POLLERR)
#endif
			FD_SET (pfds[i].fd, &xset);
	}

	ret = select (maxfd + 1, &rset, &wset, &xset, tvp);

	for (i = 0; i < nfds; i++)
	{
		pfds[i].revents = 0;

		if (pfds[i].events == 0)
			continue;

		if (FD_ISSET (pfds[i].fd, &rset))
			pfds[i].revents |= POLLIN;

		if (FD_ISSET (pfds[i].fd, &wset))
			pfds[i].revents |= POLLOUT;

		if (FD_ISSET (pfds[i].fd, &xset))
		{
			pfds[i].revents |= POLLERR;
#ifdef WIN32
			/* POSIX bug fix. See comment above. */
			pfds[i].revents |= (pfds[i].events & POLLOUT);
#endif
		}
	}
#endif /* HAVE_POLL */

	return ret;
}

/*
 * NOTE:
 *
 * If the timer callback tries to remove the timer by calling timer_remove
 * directly, we'll catch it and remove the timer after the callback instead.
 * Otherwise, a random timer could get removed or reset if the callback
 * replaced this timer in the timers array.
 *
 * TODO for 0.12.0: This makes the return value not very pertinent. As a
 * result, we should change TimerCallback to return void.
 */
static void dispatch_timer (Timer *timer)
{
	struct timeval current_time;
	timer_id       id;
	int            ret;

	if (!timer)
		return;

	/* save this in case timer->cb removes this timer */
	id = timer->id;

	/* make sure timer_remove will leave the timer alone until we're done
	 * playing with it */
	timer->dirty = 1;

	/* raise the callback before we remove this from the timers list to
	 * ensure that we do not disturb next_input_id */
	ret = timer->cb (timer->udata);

	/* timer->cb may have reallocated the timers array */
	timer = &timers[id];

	/* check if the timer cb tried to remove the timer */
	if (timer->dirty < 0)
	{
		/* if the callback tried to re-add after calling timer_remove() by
		 * returning 1, that's probably a bug */
		assert (ret == 0);
	}

	timer->dirty = 0;
	assert (timer->used == 1);

	if (!ret)
		remove_timer (timer);
	else
	{
		/* WARNING: duplicated in timer_reset */
		time_current (&current_time);
		TIME_ADD (&current_time, &timer->interval, &timer->expiration);

		/* add the timer back into the list in the same sorted order */
		free (timer->link->data);
		timers_sorted = list_remove_link (timers_sorted, timer->link);
		insort_timer (timer);
	}
}

static void dispatch_input (Input *input)
{
	if (input->suspended)
	{
		assert (input->poll_id == 0);
		return;
	}

	if (input->validate)
	{
		timer_remove_zero (&input->validate);
		input->complete = 1;
	}

	assert (input->fd == poll_fds[input->poll_id].fd);
	input->cb (input->fd, input->id + 1, input->udata);
}

/*
 * Trying to catch a bug with bad fds.
 */
static void bad_fd_abort (struct pollfd1 *pfds, unsigned int nfds)
{
	int   ret, err;
	#ifdef WIN32
	int len;
	#else
	socklen_t len;
	#endif
	List *bad_pfds = NULL;

	len = sizeof (err);

	while (nfds-- > 0)
	{
		/* use a quick getsockopt test for a bad fd without actually caring
		 * about the socket options */
		ret = getsockopt (pfds[nfds].fd,
		                  SOL_SOCKET, SO_ERROR, (void *)&err, &len);

		if (ret < 0 && event_net_errno() == EBADF)
		{
			bad_pfds = list_prepend (bad_pfds, &pfds[nfds]);
			GIFT_TRACE (("%p: %i", &pfds[nfds], pfds[nfds].fd));
		}
	}

	assert (0xEBADFBAD == 0);
}

static int poll_once (void)
{
	Timer *t       = NULL;
	int    timeout = -1;
	int    ret;

	if (timers_len > 0)
	{
		timer_id *id = list_nth_data (timers_sorted, 0);
		assert (id != NULL);

		t = timer_find (*id);
		assert (t != NULL);
		assert (t->used == 1);

		/* if the timer should've already been called by now, completely
		 * bypass poll and handle the event */
		if ((timeout = calc_timeout (t)) <= 0)
		{
			dispatch_timer (t);
			return 0;
		}
	}

	ret = event_poll (poll_fds, poll_ids, timeout);

	switch (ret)
	{
	 case -1:
		{
			/*
			 * If select() or poll() returns EBADF we loop endlessly, so trap
			 * it.  Note that poll documentation says it can do this, but
			 * the standard says that it should silently ignore bad file
			 * descriptors...we'll handle it anyway.
			 */
			if (event_net_errno() == EBADF)
				bad_fd_abort (poll_fds, poll_ids);

			/* Show error if it's not an interrupted syscall */
			if (event_net_errno() != EINTR)
			{
#ifdef DEBUG
				GIFT_ERROR ((POLLFN ": %d", event_net_errno()));
#endif
			}
#ifdef WIN32
			Sleep (3);
#else
			usleep (3000);
#endif

		}
		break;
	 case 0:
		dispatch_timer (t);
		break;
	 default:
		{
			int i;
			int nfds = ret;
			input_id ids;
			struct pollfd1 *pfd;

			/* we need to make a copy of this as a dispatched input may
			 * add or remove an input, changing this value */
			ids = input_ids_max + 1;

			for (i = 0; i < ids && nfds > 0; i++)
			{
				if (inputs[i].fd < 0 || inputs[i].dirty || inputs[i].suspended)
					continue;

				pfd = &poll_fds[inputs[i].poll_id];

				if (pfd->revents & pfd->events ||
				    pfd->revents & (POLLERR | POLLHUP | POLLNVAL))
				{
					dispatch_input (&inputs[i]);
					nfds--;
				}
			}

			POLL_TRACE (("%d/%d/%d", nfds, ret, ids));
		}
		break;
	}

	return ret;
}

void event_poll_once (void)
{
	Input *input;

	poll_once ();

	/* finish inserting all queued input additions */
	while ((input = array_shift (&inputs_add)))
		input_add_queue (input);

	/* ...and then removed inputs */
	while ((input = array_shift (&inputs_remove)))
		input_remove_queue (input);
}

void event_loop (void)
{
	polling = 1;

	while (polling)
	{
		if (!input_ids && !timers_len)
			break;

		event_poll_once ();
	}
}

/*****************************************************************************/

#if 0
static void read_stdin (int fd, input_id id, void *udata)
{
	static char buf[1024];
	ssize_t     n;

	if ((n = read (fd, buf, sizeof (buf) - 1)) <= 0)
	{
		fprintf (stderr, "fd=%d exhausted\n", fd);
		input_remove (id);
		return;
	}

	buf[n] = 0;
	printf ("%s", buf);
}

/* tests the input loop */
int main (int argc, char **argv)
{
	input_id id;

	event_init ();

	id = input_add (fileno (stdin), NULL, INPUT_READ, read_stdin, 0);
	assert (id != 0);

	event_loop ();
	printf ("loop finished...\n");

	return 0;
}
#endif

/*****************************************************************************/

#if 0
static int sec_10 (void *udata)
{
	fprintf (stderr, "Finished!\n");
	event_quit (0);
	return 0;
}

static int sec_1 (void *udata)
{
	fprintf (stderr, "\n");
	return 1;
}

static int msec_50 (void *udata)
{
	fprintf (stderr, ".");
	return 1;
}

static int msec_100 (void *udata)
{
	fprintf (stderr, "Started!\n");

	timer_add (50 * MSEC,     msec_50, NULL);
	timer_add ( 1 *  SECONDS,  sec_1,  NULL);
	timer_add (10 *  SECONDS,  sec_10, NULL);

	return 0;
}

/* tests consistency of the timer */
int main (int argc, char **argv)
{
	event_init ();

	timer_add (100 * MSEC, msec_100, NULL);

	event_loop ();

	return 0;
}
#endif

#if 0
#include "stopwatch.h"

#define STRESS_TIMERS 16384            /* 2 ^ 14 */
static timer_id ids[STRESS_TIMERS];

static int timer_cb (timer_id *id)
{
	printf ("id = %u\n", (unsigned int)(*id));
	return 0;
}

/* benchmark timer management */
int main (int argc, char **argv)
{
	StopWatch *sw;
	timer_id   i;

	event_init ();

	printf ("using %i timers...\n", STRESS_TIMERS);

	sw = stopwatch_new (0);
	assert (sw != NULL);

	stopwatch_start (sw);

	for (i = 0; i < STRESS_TIMERS; i++)
	{
		ids[i] = timer_add ((time_t)(i % (5 * MINUTES)),
		                    (TimerCallback)timer_cb, &ids[i]);
	}

	stopwatch_stop (sw);
	printf ("insertion: %.06f seconds elapsed\n",
	        stopwatch_elapsed (sw, NULL));

	stopwatch_start (sw);

	/* now remove them backwards */
	while (i-- > 0)
		timer_remove (ids[i]);

	stopwatch_stop (sw);
	printf ("removal: %.06f seconds elapsed\n",
	        stopwatch_elapsed (sw, NULL));

	event_quit (0);

	stopwatch_free (sw);

	return 0;
}
#endif
