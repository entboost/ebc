#ifndef __DBPOOL_H__
#define __DBPOOL_H__
#include <db/sink.h>
#define POSTSINK   "postgresql"

#ifdef __cplusplus
extern "C" {
#endif

int sink_pool_init(const char *type, 
		   int         minsize, 
		   int         maxsize, 
		   const char *host,
		   int         port,
		   const char *options,
		   const char *dbname,
		   const char *login,
		   const char *pwd);
int sink_pool_init_log(const char *type, 
		   int         size, 
		   const char *host,
		   int         port,
		   const char *options,
		   const char *dbname,
		   const char *login,
		   const char *pwd);
Sink*  sink_pool_get();
Sink*  sink_pool_get_log();
void   sink_pool_put(Sink*);
void   sink_pool_cleanup();

#ifdef __cplusplus
}
#endif

#endif  /*__DBPOOL_H__ */


