// locklist.h file here
#ifndef __locklist_h__
#define __locklist_h__

// 
#include "list"
#include "stldef.h"
#include <algorithm>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/lock_types.hpp>
//#include <boost/thread/mutex.hpp>

#ifndef AUTO_LOCK
#define AUTO_LOCK(l) boost::unique_lock<boost::shared_mutex> writeLock(l.mutex())
//#define AUTO_LOCK(l) boost::mutex::scoped_lock lock(l.mutex())
typedef boost::shared_lock<boost::shared_mutex> BoostReadLock;  
typedef boost::unique_lock<boost::shared_mutex> BoostWriteLock;  
#define AUTO_RLOCK(l) BoostReadLock rdLock(l.mutex())
#define AUTO_WLOCK(l) BoostWriteLock wtLock(l.mutex())
#define AUTO_CONST_RLOCK(l) BoostReadLock rdLock(const_cast<boost::shared_mutex&>(l.mutex()))
#define AUTO_CONST_WLOCK(l) BoostWriteLock wtLock(const_cast<boost::shared_mutex&>(l.mutex()))
#endif // AUTO_LOCK

template<typename T>
class CLockList
	: public std::list<T>
{
protected:
	boost::shared_mutex m_mutex; 
	//boost::mutex m_mutex;

public:
	boost::shared_mutex & mutex(void) {return m_mutex;}
	const boost::shared_mutex & mutex(void) const {return m_mutex;}
	//boost::mutex & mutex(void) {return m_mutex;}
	//const boost::mutex & mutex(void) const {return m_mutex;}
	const CLockList<T>& operator = (const CLockList<T>& pList)
	{
		BoostWriteLock wtlock(m_mutex);
		BoostReadLock rdlock(const_cast<boost::shared_mutex&>(pList.mutex()));
		typename CLockList<T>::const_iterator pIter = pList.begin();
		for (; pIter!=pList.end(); pIter++)
		{
			std::list<T>::push_back(*pIter);
		}
		return *this;
	}

	void pushfront(const T& t)
	{
		BoostWriteLock wtlock(m_mutex);
		std::list<T>::push_front(t);
	}
	void add(const T& t, bool is_lock=true)
	{
		if (is_lock)
		{
			BoostWriteLock wtlock(m_mutex);
			std::list<T>::push_back(t);
		}else
		{
			std::list<T>::push_back(t);
		}
	}
	bool front(T & out, bool is_pop = true)
	{
		if (is_pop)
		{
			BoostWriteLock wtlock(m_mutex);
			typename std::list<T>::iterator pIter = std::list<T>::begin();
			if (pIter == std::list<T>::end())
				return false;
			out = *pIter;
			std::list<T>::pop_front();
		}else
		{
			BoostReadLock rdlock(m_mutex);
			typename std::list<T>::iterator pIter = std::list<T>::begin();
			if (pIter == std::list<T>::end())
				return false;
			out = *pIter;
		}
		return true;
	}
	bool popfront(void)
	{
		BoostWriteLock wtlock(m_mutex);
		if (std::list<T>::empty())
			return false;
		std::list<T>::pop_front();
		return true;
	}
	bool back(T & out, bool is_pop = true)
	{
		if (is_pop)
		{
			BoostWriteLock wtlock(m_mutex);
			if (std::list<T>::empty())
				return false;
			typename std::list<T>::iterator pIter = std::list<T>::end();
			pIter--;
			out = *pIter;
			if (is_pop)
				std::list<T>::pop_back();
		}else
		{
			BoostReadLock rdlock(m_mutex);
			if (std::list<T>::empty())
				return false;
			typename std::list<T>::iterator pIter = std::list<T>::end();
			pIter--;
			out = *pIter;
		}
		return true;
	}
	bool popback(void)
	{
		BoostWriteLock wtlock(m_mutex);
		if (std::list<T>::empty())
			return false;
		std::list<T>::pop_back();
		return true;
	}

	void clear(bool is_lock = true)
	{
		if (is_lock)
		{
			BoostWriteLock wtlock(m_mutex);
			std::list<T>::clear();
		}else
		{
			std::list<T>::clear();
		}
	}
	size_t size(bool is_lock = true) const
	{
		if (is_lock)
		{
			BoostReadLock rdlock(const_cast<boost::shared_mutex&>(m_mutex));
			return std::list<T>::size();
		}else
		{
			return std::list<T>::size();
		}
	}
	bool empty(bool is_lock = false) const
	{
		if (is_lock)
		{
			BoostReadLock rdlock(const_cast<boost::shared_mutex&>(m_mutex));
			return std::list<T>::empty();
		}else
		{
			return std::list<T>::empty();
		}
	}
public:
	CLockList(void)
	{
	}
	virtual ~CLockList(void)
	{
		clear();
	}
};

template<typename T>
class CLockListPtr
	: public CLockList<T>
{
public:
	T front(void)
	{
		T result = 0;
		CLockList<T>::front(result);
		return result;
	}

	void clear(bool is_lock = true, bool is_delete = true)
	{
		if (is_lock)
		{
			BoostWriteLock wtlock(this->mutex());
			if (is_delete)
				for_each(std::list<T>::begin(), std::list<T>::end(), DeletePtr());
			std::list<T>::clear();
		}else
		{
			if (is_delete)
				for_each(std::list<T>::begin(), std::list<T>::end(), DeletePtr());
			std::list<T>::clear();
		}
	}
public:
	CLockListPtr(void)
	{}
	virtual ~CLockListPtr(void)
	{
		clear();
	}
};

#endif // __locklist_h__
