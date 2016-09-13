
#include <stdlib.h>
#include <stdio.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <string.h>
#include <db/sink.h>
#include <db/pool.h>

#include <db/postsink.h>


static Sink **sinks = 0;
static Sink **sinks_log = 0;
static int    sink_number = 0;
static int    sink_minnumber = 0;
static int    sink_maxnumber = 0;
static int    sink_number_log = 0;
extern SinkOperations sos; /*postgresql*/

static LOCK_T  theAddDelLock;

#ifndef USE_SINK_POOL_NULL 

int sink_pool_init(const char *type, 
		   int         minsize, 
		   int         maxsize, 
		   const char *host,
		   int         port,
		   const char *options,
		   const char *dbname,
		   const char *login,
		   const char *pwd)
{
    int i = 0;
    sinks = (Sink**)malloc(sizeof(Sink*) * maxsize);
	sink_number = minsize;
    sink_minnumber = minsize;
    sink_maxnumber = maxsize;
    for(i = 0; i < maxsize; i++) {
	sinks[i] = (Sink*)malloc(sizeof(Sink));
    }
    
    if(strcmp(type, POSTSINK) == 0) {
	for(i = 0; i < maxsize; i++) {
	    sinks[i]->sos = &sos;
	    strcpy(sinks[i]->user, login);
	    strcpy(sinks[i]->password, pwd);
	    strcpy(sinks[i]->dbname, dbname);
	    strcpy(sinks[i]->host, host);
	    sinks[i]->port = port;
	    
		sink_init(sinks[i]);
		if (i < sink_number)	// add by hd
		{
			if(sink_connect(sinks[i]) < 0) {
				return -1;
			}
			if(sink_state(sinks[i]) != CONN_OK) {
				return -1;
			}
		}
	}
    } else {
	printf("not support this type sink so far yet\n");
	return -1;
    }
	LOCK_INIT (&theAddDelLock);
    return 0;
}

#else  //test demo

int sink_pool_init(const char *type, 
				   int         size, 
				   const char *host,
				   int         port,
				   const char *options,
				   const char *dbname,
				   const char *login,
				   const char *pwd)
{
	int i = 0;
	sinks = (Sink**)malloc(sizeof(Sink*) * size);
	sink_number = size;
	for(i = 0; i < size; i++) {
		sinks[i] = (Sink*)malloc(sizeof(Sink));
	}

	if(strcmp(type, POSTSINK) == 0) {
		for(i = 0; i < size; i++) {
			sinks[i]->sos = &sos;
			strcpy(sinks[i]->user, login);
			strcpy(sinks[i]->password, pwd);
			strcpy(sinks[i]->dbname, dbname);
			strcpy(sinks[i]->host, host);
			sinks[i]->port = port;


			sink_init(sinks[i]);

			/*
			if(sink_connect(sinks[i]) < 0) {
				return -1;
			}
            
			if(sink_state(sinks[i]) != CONN_OK) {
			return -1;
			}*/
		}
	} else {
		printf("not support this type sink so far yet\n");
		return -1;
	}
	return 0;
}

#endif


int sink_pool_init_log(const char *type, 
		   int         size, 
		   const char *host,
		   int         port,
		   const char *options,
		   const char *dbname,
		   const char *login,
		   const char *pwd)
{
    int i = 0;
    sinks_log = (Sink**)malloc(sizeof(Sink*) * size);
    sink_number_log = size;
    for(i = 0; i < size; i++) {
	sinks_log[i] = (Sink*)malloc(sizeof(Sink));
    }
    
    if(strcmp(type, POSTSINK) == 0) {
	for(i = 0; i < size; i++) {
	    sinks_log[i]->sos = &sos;
	    strcpy(sinks_log[i]->user, login);
	    strcpy(sinks_log[i]->password, pwd);
	    strcpy(sinks_log[i]->dbname, dbname);
	    strcpy(sinks_log[i]->host, host);
	    sinks_log[i]->port = port;
	    
	    sink_init(sinks_log[i]);
	    if(sink_connect(sinks_log[i]) < 0) {
		return -1;
	    }
	    if(sink_state(sinks_log[i]) != CONN_OK) {
		return -1;
	    }
	}
    } else {
	printf("not support this type sink so far yet\n");
	return -1;
    }
    return 0;
}

int sink_pool_del()
{
	Sink *sink = 0;
	int ndelindex = sink_number-1;
	if (sink_number <= sink_minnumber)
		return 0;

	//printf("================ sink_pool_del\n");
	sink = sinks[ndelindex];

	sink_lock(sink);
	if(sink_is_busy(sink)) {
		sink_unlock(sink);
		return 0;
	}
	sink_number = ndelindex;
	//if(sink_state(sink) != CONN_OK) {
	//	sink_unlock(sink);
	//	return 0;
	//}

	sink_disconnect(sink);
	//sink_set_busy(sink, 1);	// 用于防止后面
	sink_unlock(sink);
//
//#ifdef WIN32
//	Sleep (500);
//#else
//	usleep( 500000);
//#endif
//	sink_set_busy(sink, 0);

	//printf("================ sink_pool_del OK\n");
	return 0;
}

Sink * sink_pool_add()
{
	Sink *sink = 0;
	if (sink_number >= sink_maxnumber)
		return 0;

	//printf("================ sink_pool_add\n");
	sink = sinks[sink_number];

	sink_lock(sink);
	if(sink_is_busy(sink)) {
		sink_unlock(sink);
		return 0;
	}

	if(sink_connect(sink) < 0) {
		//sink_reset(sink);	// ?
		sink_unlock(sink);
		return 0;
	}
	if(sink_state(sink) != CONN_OK) {
		sink_unlock(sink);
		return 0;
	}

	sink_set_busy(sink, 1);
	sink_number++;
	sink_unlock(sink);
	//printf("================ sink_pool_add OK\n");
	return sink;
}

Sink*  sink_pool_get()
{

	Sink *sink = 0;
	int i =0;
	int nosinkcount = 0;
	static int theminnumbercount = 0;
	//printf("================ %d\n", sink_number);
	while(1) 
	{
		//sink = sinks[random() % sink_number];
		for (i =0; i< sink_number; ++i)
		{
			sink = sinks[i];

			sink_lock(sink);
			if(sink_is_busy(sink)) {
				sink_unlock(sink);
				continue;
			}

			/* check connection status */
			if(sink_state(sink) == CONN_BAD) {
				sink_reset(sink);
			}

			sink_set_busy(sink, 1);
			sink_unlock(sink);
			//break;
			// add by hd
			if (sink_number > sink_minnumber && i <= sink_minnumber-1)
			{
				theminnumbercount++;
				if (theminnumbercount >= 100)	// 连续100个正常数据库连接低于最低连接数，减少一个数据库连接；
				{
					theminnumbercount = 0;
					LOCK (&theAddDelLock);
					sink_pool_del();
					UNLOCK (&theAddDelLock);
				}
			}else
			{
				theminnumbercount = 0;
			}
			return sink;
		}

		// add by hd
		if (nosinkcount++ > 100)	// 全部忙，并且等100次以上，增加一个数据库连接
		{
			nosinkcount = 0;	// 如果找不到还可以重新开始等
			LOCK (&theAddDelLock);
			sink = sink_pool_add();
			UNLOCK (&theAddDelLock);
			if (sink)
			{
				return sink;
			}
		}
		//等待一段时间
#ifdef WIN32
		Sleep (10);
#else
		usleep( 10000);
#endif

	}
	return 0;
}

Sink*  sink_pool_get_log()
{
	Sink *sink = 0;
	int i =0;
	while(1) 
	{
		//sink = sinks[random() % sink_number];
		for (i =0; i< sink_number_log; ++i)
		{
			sink = sinks_log[i];

			sink_lock(sink);
			if(sink_is_busy(sink)) {
				sink_unlock(sink);
				continue;
			}

			/* check connection status */
			if(sink_state(sink) == CONN_BAD) {
				sink_reset(sink);
			}

			sink_set_busy(sink, 1);
			sink_unlock(sink);
			//break;
			return sink;
		}

		//等待一段时间
#ifdef WIN32
		Sleep (5);
#else
		usleep( 5000);
#endif

	}
	return 0;
}

void sink_pool_put(Sink *sink)
{
    sink_lock(sink);
    sink_set_busy(sink, 0);
    sink_unlock(sink);
}
void sink_pool_cleanup()
{
    Sink *sink;
    int i;

    for(i = 0; i < sink_maxnumber; i++) {
	sink = sinks[i];
	sink_disconnect(sink);
	sink_cleanup(sink);
    }
    free(sinks);

	for(i = 0; i < sink_number_log; i++) {
	sink = sinks_log[i];
	sink_disconnect(sink);
	sink_cleanup(sink);
    }
    free(sinks_log);
}




