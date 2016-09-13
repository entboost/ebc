
#include <stdlib.h>

#include <db/sink.h>
#ifdef WIN32
#include "libpq-fe.h"
//#include "libpq/libpq-fe.h"
#else
#include <libpq-fe.h>
#endif

#include "db/postsink.h"


#ifndef USE_SINK_POOL_NULL
int post_connect(Sink *sink)
{
	int ret;
    PGconn *conn;
    char    buf[100];
    if (!sink)
	return 0;
    
    sprintf(buf, "%d", sink->port);
    conn = PQsetdbLogin(sink->host,
			buf,
			0,
			0,
			sink->dbname,
			sink->user,
			sink->password);
    if(conn) {
	sink->u.c_pq = conn;  //PGconn*
	//set client encoding
	//ret = PQsetClientEncoding(conn, "GBK");
	//ret = PQsetClientEncoding(conn, "LATIN2");
	ret = PQsetClientEncoding(conn, "UNICODE");
	return 0;
    }
    return -1;
}
int post_disconnect(Sink *sink)
{
    if(sink) {
	PQfinish( (PGconn *)sink->u.c_pq);
	return 0;
    }
    return -1;
}
int post_reset(Sink *sink)
{
    if(sink) {
	PQreset( (PGconn *)sink->u.c_pq);
	return 0;
    }
    return -1;
}

Result * post_exec(Sink *sink, const char *query)
{
    Result *res;
    if(sink) {
	PGresult *r;
	r = PQexec( (PGconn *)sink->u.c_pq, query);
	if(r) {
	    res = (Result*)malloc(sizeof(Result));
	    res->priv = r;
	    return res;
	}
    }
    return 0;
}

int post_conn_state(Sink *sink)
{
    if(sink) {
	switch (PQstatus( (PGconn *)sink->u.c_pq)) {
	case CONNECTION_OK:
	    return CONN_OK;
	case CONNECTION_BAD:
	    return CONN_BAD;
	case CONNECTION_STARTED:
	    return CONN_STARTED;
	case CONNECTION_MADE:
	    return CONN_MADE;
	case CONNECTION_AWAITING_RESPONSE:
	    return CONN_AWAITING_RESPONSE;
	case CONNECTION_AUTH_OK:
	    return CONN_AUTH_OK;
	case CONNECTION_SETENV:
	    return CONN_SETENV;
	}
    }
    return UNKNOWN_STATE;
}
const char *post_get(Result *res, int row, int col)
{
    PGresult *result;
    if(!res)
	return 0;
    result = (PGresult*)res->priv;
    return (const char*)PQgetvalue(result, row, col);
}
void post_clean(Result* res)
{
    PGresult *result;
    if(!res)
	return ;
    result = (PGresult*)res->priv;
    PQclear(result);
}
int post_get_row_number(Result *res)
{
    PGresult *result;
    if(!res)
	return 0;
    result = (PGresult*)res->priv;
    
    return PQntuples(result);
}
int post_get_col_number(Result *res)
{
    PGresult *result;
    if(!res)
	return 0;
    result = (PGresult*)res->priv;
    
    return PQnfields(result);
}
const char* post_fname(Result* res, int n)
{
    PGresult *result;
    if(!res)
	return 0;
    result = (PGresult*)res->priv;
    return PQfname(result, n);
}
int post_cmd_state(Result *res)
{
    PGresult *result;
    if(!res)
	return RES_FATAL_ERROR;
    result = (PGresult*)res->priv;
    switch (PQresultStatus(result)) {
    case PGRES_EMPTY_QUERY:
	/* -- The string sent to the backend was empty.*/
	return RES_EMPTY_QUERY;
    case PGRES_COMMAND_OK:
	/*-- Successful completion of a command returning no data*/
	return RES_COMMAND_OK;
    case PGRES_TUPLES_OK:
	/*-- The query successfully executed*/
	return RES_TUPLES_OK;
/*
	    * PGRES_COPY_OUT -- Copy Out (from server) data transfer started
	    * PGRES_COPY_IN -- Copy In (to server) data transfer started
     * PGRES_BAD_RESPONSE -- The server's response was not understood
     * PGRES_NONFATAL_ERROR
     */
    case PGRES_FATAL_ERROR:
	return RES_FATAL_ERROR;
    default:
	return UNKNOWN_STATE;
    }
    
    return UNKNOWN_STATE;
}

#endif

#ifdef USE_SINK_POOL_NULL
SinkOperations sos = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
};
#else
SinkOperations sos = {
    post_connect,
    post_conn_state,
    post_disconnect,
    post_reset,
    post_exec,
    post_get,
    post_clean,
    post_get_row_number,
    post_get_col_number,
    post_fname,
    post_cmd_state,
};
#endif
