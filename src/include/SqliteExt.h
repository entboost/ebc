// SqliteExt.h file here
#ifndef __SqliteExt_h__
#define __SqliteExt_h__

#include <CGCBase/cgcvalueinfo.h>
#include <sqlite3.h>
//#include <boost/thread/shared_mutex.hpp>
#ifdef _MSC_VER //WIN32
#ifdef _DEBUG
#pragma comment(lib,"sqlite3sd.lib")
#else
#pragma comment(lib,"sqlite3s.lib")
#endif
#endif // WIN32
using namespace mycp;

namespace entboost {

class CCDBCResultSet
{
public:
    typedef boost::shared_ptr<CCDBCResultSet> pointer;

    mycp::bigint size(void) const
    {
        return m_rows;
    }
    mycp::bigint index(void) const
    {
        return m_currentIndex;
    }
    mycp::cgcValueInfo::pointer index(mycp::bigint moveIndex)
    {
        if (m_resultset == NULL || m_rows == 0) return mycp::cgcNullValueInfo;
        if (moveIndex < 0 || (moveIndex+1) > m_rows) return mycp::cgcNullValueInfo;
        m_currentIndex = moveIndex;
        return getCurrentRecord();
    }
    mycp::cgcValueInfo::pointer first(void)
    {
        if (m_resultset == NULL || m_rows == 0) return mycp::cgcNullValueInfo;
        m_currentIndex = 0;
        return getCurrentRecord();
    }
    mycp::cgcValueInfo::pointer next(void)
    {
        if (m_resultset == NULL || m_rows == 0) return mycp::cgcNullValueInfo;
        if (m_currentIndex+1 == m_rows) return mycp::cgcNullValueInfo;
        ++m_currentIndex;
        return getCurrentRecord();
    }
    mycp::cgcValueInfo::pointer previous(void)
    {
        if (m_resultset == NULL || m_rows == 0) return mycp::cgcNullValueInfo;
        if (m_currentIndex == 0) return mycp::cgcNullValueInfo;
        --m_currentIndex;
        return getCurrentRecord();
    }
    mycp::cgcValueInfo::pointer last(void)
    {
        if (m_resultset == NULL || m_rows == 0) return mycp::cgcNullValueInfo;
        m_currentIndex = m_rows - 1;
        return getCurrentRecord();
    }
    void reset(void)
    {
        if (m_resultset != 0)
        {
            try
            {
                sqlite3_free_table( m_resultset );
            }catch(...)
            {}
            m_resultset = NULL;
        }
        m_rows = 0;
    }

    CCDBCResultSet(char** resultset, mycp::bigint rows, short fields)
        : m_resultset(resultset), m_rows(rows), m_fields(fields)
    {
    }
    virtual ~CCDBCResultSet(void)
    {
        reset();
    }

protected:
    mycp::cgcValueInfo::pointer getCurrentRecord(void) const
    {
        assert (m_resultset != NULL);
        assert (m_currentIndex >= 0 && m_currentIndex < m_rows);

        std::vector<mycp::cgcValueInfo::pointer> record;
        try
        {
            const mycp::bigint offset = (m_currentIndex+1)*m_fields;	// +1 is column head info
            for (int i=0 ; i<m_fields; i++ )
            {
                const mycp::tstring s( m_resultset[offset+i]==NULL ? "" : (const char*)m_resultset[offset+i]);
                record.push_back(mycp::CGC_VALUEINFO(s));
            }
        }catch(...)
        {
            // ...
        }

        return mycp::CGC_VALUEINFO(record);
    }

private:
    char** m_resultset;
    mycp::bigint	m_rows;
    short		m_fields;
    mycp::bigint	m_currentIndex;
};

#define CDBC_RESULTSET(r,row,field) CCDBCResultSet::pointer(new CCDBCResultSet(r,row,field))

const int escape_in_size = 1;
const mycp::tstring escape_in[] = {"''"};
const mycp::tstring escape_out[] = {"'"};
//const int escape_old_out_size = 2;	// ?兼容旧版本
//const mycp::tstring escape_old_in[] = {"&lsquo;","&pge0;"};
//const mycp::tstring escape_old_out[] = {"'","\\"};

//const int escape_size = 2;
//const mycp::tstring escape_in[] = {"&lsquo;","&mse0;"};
//const mycp::tstring escape_out[] = {"'","\\"};


class CSqliteCdbc
{
public:
    typedef boost::shared_ptr<CSqliteCdbc> pointer;
    static CSqliteCdbc::pointer create(void)
    {
        return CSqliteCdbc::pointer(new CSqliteCdbc());
    }

    CSqliteCdbc(void)
        : m_pSqlite(NULL), m_tLastTime(0)
    {}
    virtual ~CSqliteCdbc(void)
    {
        close();
        //finalService();
    }
    //virtual bool initService(mycp::cgcValueInfo::pointer parameter)
    //{
    //	if (isServiceInited()) return true;
    //	m_bServiceInited = true;
    //	return isServiceInited();
    //}
    //virtual void finalService(void)
    //{
    //	close();
    //	m_cdbcInfo.reset();
    //	m_bServiceInited = false;
    //}
public:
    static const mycp::tstring & replace(mycp::tstring & strSource, const mycp::tstring & strFind, const mycp::tstring &strReplace)
    {
        std::string::size_type pos=0;
        std::string::size_type findlen=strFind.size();
        std::string::size_type replacelen=strReplace.size();
        while ((pos=strSource.find(strFind, pos)) != std::string::npos)
        {
            strSource.replace(pos, findlen, strReplace);
            pos += replacelen;
        }
        return strSource;
    }

    static void escape_string_in(mycp::tstring & str)
    {
        for (int i=0; i<escape_in_size; i++)
            replace(str, escape_out[i], escape_in[i]);
    }
    static void escape_string_out(mycp::tstring & )
    {
        //for (int i=0; i<escape_old_out_size; i++)
        //	replace(str, escape_old_in[i], escape_old_out[i]);
    }

    const std::string &lastErrorString(void) const {return m_lastErrorString;}


    // UTF-8
    virtual bool open(const char* sDatabaseFile)
    {
        if (m_sDatabaseFile == sDatabaseFile && this->isopen()) return true;

        // close old database;
        close();
        m_sDatabaseFile = sDatabaseFile;
        m_tLastTime = time(0);

        try
        {
            //  rc = sqlite3_open(":memory:", &db);
            //  ATTACH DATABASE ':memory:' AS aux1;
            //
            const int rc = sqlite3_open(m_sDatabaseFile.c_str(), &m_pSqlite);
            if ( rc!=SQLITE_OK )
            {
                //CGC_LOG((mycp::LOG_ERROR, "Can't open database: %s\n", sqlite3_errmsg(m_pSqlite)));
                //sqlite3_close(m_pSqlite);
                m_pSqlite = 0;
                return false;
            }
        }catch(...)
        {
            return false;
        }

        return true;
    }
    //virtual bool open(void) {return open(m_sDatabaseFile.c_str());}
    virtual void close(void)
    {
        try
        {
            if (m_pSqlite!=0)
            {
                sqlite3_close(m_pSqlite);
                m_pSqlite = 0;
                m_tLastTime = time(0);
            }
        }catch(...)
        {
        }
        m_results.clear();
    }
    virtual bool isopen(void) const
    {
        return (m_pSqlite==0)?false:true;
    }
    virtual time_t lasttime(void) const {return m_tLastTime;}

    //static int sqlite_callback(
    //	void* pv,    /* 由 sqlite3_exec() 的第四个参数传递而来 */
    //	int argc,        /* 表的列数 */
    //	char** argv,    /* 指向查询结果的指针数组, 可以由 sqlite3_column_text() 得到 */
    //	char** col        /* 指向表头名的指针数组, 可以由 sqlite3_column_name() 得到 */
    //	)
    //{
    //	return 0;
    //}
    virtual mycp::bigint execute(const char * exeSql)
    {
        if (exeSql == NULL) return -1;
        if (!isopen()) return -1;

        //rc = sqlite3_exec(db, "BEGIN;", 0, 0, &zErrMsg);
        ////执行SQL语句
        //rc = sqlite3_exec(db, "COMMIT;", 0, 0, &zErrMsg);

        mycp::bigint ret = 0;
        try {
            char *zErrMsg = 0;
            //boost::mutex::scoped_lock lock(m_mutex);
            const int rc = sqlite3_exec( m_pSqlite , exeSql, 0, 0, &zErrMsg);
            if ( rc!=SQLITE_OK ) {
                if (zErrMsg!=0)
                    m_lastErrorString = zErrMsg;
                //CGC_LOG((mycp::LOG_ERROR, "Can't execute SQL: (%s); %s\n", exeSql,zErrMsg));
                sqlite3_free(zErrMsg);
                return -1;
            }
            ret = (mycp::bigint)sqlite3_changes(m_pSqlite);
        }catch(...)
        {
            return -1;
        }
        return ret;
    }

    virtual mycp::bigint select(const char * selectSql, int& outCookie)
    {
        outCookie = 0;
        if (selectSql == NULL) return -1;
        if (!isopen()) return -1;

        mycp::bigint rows = 0;
        try
        {
            int nrow = 0, ncolumn = 0;
            char *zErrMsg = 0;
            char **azResult = 0; //二维数组存放结果
            const int rc = sqlite3_get_table( m_pSqlite , selectSql , &azResult , &nrow , &ncolumn , &zErrMsg );
            if ( rc!=SQLITE_OK ) {
                if (zErrMsg!=0)
                    m_lastErrorString = zErrMsg;
                //CGC_LOG((mycp::LOG_ERROR, "Can't select SQL: (%s); %s\n", selectSql,zErrMsg));
                sqlite3_free(zErrMsg);
                return -1;
            }
            if (azResult != NULL && nrow>0 && ncolumn>0)
            {
                outCookie = (int)(long)azResult;
                m_results.insert(outCookie, CDBC_RESULTSET(azResult, nrow, ncolumn));
                rows = nrow;
            }else
            {
                rows = 0;
                sqlite3_free_table( azResult );
            }
        }catch(...)
        {
            //CGC_LOG((mycp::LOG_ERROR, "Select SQL exception: (%s) \n", selectSql));
            return -1;
        }
        return rows;
    }
    virtual mycp::bigint select(const char * selectSql)
    {
        if (selectSql == NULL) return -1;
        if (!isopen()) return -1;
        mycp::bigint rows = 0;
        try
        {
            int nrow = 0, ncolumn = 0;
            char *zErrMsg = 0;
            //const int rc = sqlite3_get_table( m_pSqlite , selectSql , 0, &nrow , &ncolumn , &zErrMsg );
            char **azResult = 0; //二维数组存放结果
            const int rc = sqlite3_get_table( m_pSqlite , selectSql , &azResult , &nrow , &ncolumn , &zErrMsg );
            if ( rc!=SQLITE_OK ) {
                if (zErrMsg!=0)
                    m_lastErrorString = zErrMsg;
                //CGC_LOG((mycp::LOG_ERROR, "Can't select SQL: (%s); %s\n", selectSql,zErrMsg));
                sqlite3_free(zErrMsg);
                return -1;
            }
            rows = nrow;
            sqlite3_free_table( azResult );
        }catch(...)
        {
            //CGC_LOG((mycp::LOG_ERROR, "Select SQL exception: (%s) \n", selectSql));
            return -1;
        }
        return rows;
    }
    virtual mycp::bigint size(int cookie) const
    {
        CCDBCResultSet::pointer cdbcResultSet;
        return m_results.find(cookie, cdbcResultSet) ? cdbcResultSet->size() : -1;
    }
    virtual mycp::bigint index(int cookie) const
    {
        CCDBCResultSet::pointer cdbcResultSet;
        return m_results.find(cookie, cdbcResultSet) ? cdbcResultSet->index() : -1;
    }

    virtual mycp::cgcValueInfo::pointer index(int cookie, mycp::bigint moveIndex)
    {
        CCDBCResultSet::pointer cdbcResultSet;
        return m_results.find(cookie, cdbcResultSet) ? cdbcResultSet->index(moveIndex) : mycp::cgcNullValueInfo;
    }
    virtual mycp::cgcValueInfo::pointer first(int cookie)
    {
        CCDBCResultSet::pointer cdbcResultSet;
        return m_results.find(cookie, cdbcResultSet) ? cdbcResultSet->first() : mycp::cgcNullValueInfo;
    }
    virtual mycp::cgcValueInfo::pointer next(int cookie)
    {
        CCDBCResultSet::pointer cdbcResultSet;
        return m_results.find(cookie, cdbcResultSet) ? cdbcResultSet->next() : mycp::cgcNullValueInfo;
    }
    virtual mycp::cgcValueInfo::pointer previous(int cookie)
    {
        CCDBCResultSet::pointer cdbcResultSet;
        return m_results.find(cookie, cdbcResultSet) ? cdbcResultSet->previous() : mycp::cgcNullValueInfo;
    }
    virtual mycp::cgcValueInfo::pointer last(int cookie)
    {
        CCDBCResultSet::pointer cdbcResultSet;
        return m_results.find(cookie, cdbcResultSet) ? cdbcResultSet->last() : mycp::cgcNullValueInfo;
    }
    virtual void reset(int cookie)
    {
        CCDBCResultSet::pointer cdbcResultSet;
        if (m_results.find(cookie, cdbcResultSet, true))
        {
            cdbcResultSet->reset();
        }
    }

    virtual bool auto_commit(bool /*autocommit*/)
    {
        if (!isopen()) return false;
        try
        {
            char *zErrMsg = 0;
            const int rc = sqlite3_exec( m_pSqlite , "BEGIN;", 0, 0, &zErrMsg);
            if ( rc!=SQLITE_OK ) {
                if (zErrMsg!=0)
                    m_lastErrorString = zErrMsg;
                //CGC_LOG((mycp::LOG_ERROR, "Can't BEGIN: %s\n", zErrMsg));
                sqlite3_free(zErrMsg);
                return false;
            }
            return true;
        }catch(...)
        {
            return false;
        }
        return false;
    }
    virtual bool commit(void)
    {
        if (!isopen()) return false;
        try
        {
            char *zErrMsg = 0;
            const int rc = sqlite3_exec( m_pSqlite , "COMMIT;", 0, 0, &zErrMsg);
            if ( rc!=SQLITE_OK ) {
                if (zErrMsg!=0)
                    m_lastErrorString = zErrMsg;
                //CGC_LOG((mycp::LOG_ERROR, "Can't COMMIT: %s\n", zErrMsg));
                sqlite3_free(zErrMsg);
                return false;
            }
            return true;
        }catch(...)
        {
            return false;
        }
        return false;
    }
    //virtual bool rollback(void)
    //{
    //	if (!isopen()) return false;
    //	return false;
    //}
private:
    sqlite3 * m_pSqlite;
    //boost::mutex m_mutex;

    time_t m_tLastTime;
    CLockMap<int, CCDBCResultSet::pointer> m_results;
    mycp::tstring m_sDatabaseFile;
    std::string m_lastErrorString;
};

} // namespace entboost

#endif //__SqliteExt_h__
