// lockmap.h file here
#ifndef __lockmap_h__
#define __lockmap_h__

// 
#include <vector>
#include <map>
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

template<typename K, typename T,typename P=std::less<K> >
class CLockMap
	: public std::map<K, T, P>
{
public:
	typedef std::pair<K, T>		Pair;

protected:
	boost::shared_mutex m_mutex; 
	//boost::mutex m_mutex;
public:
	boost::shared_mutex & mutex(void) {return m_mutex;}
	const boost::shared_mutex & mutex(void) const {return m_mutex;}
	//boost::mutex & mutex(void) {return m_mutex;}
	//const boost::mutex & mutex(void) const {return m_mutex;}

	bool insert(const K& k, const T& t,bool bInsertForce=true,T* pOutOld=NULL, bool is_lock = true)
	{
		if (is_lock)
		{
			BoostWriteLock wtlock(m_mutex);
			typename std::map<K, T, P>::iterator iter = std::map<K, T, P>::find(k);
			if (iter != std::map<K, T, P>::end())
			{
				if (pOutOld!=NULL)
					*pOutOld = iter->second;
				if (bInsertForce)
				{
					std::map<K, T, P>::erase(iter);
				}else
				{
					return false;
				}
			}
			std::map<K, T, P>::insert(Pair(k, t));
		}else
		{
			typename std::map<K, T, P>::iterator iter = std::map<K, T, P>::find(k);
			if (iter != std::map<K, T, P>::end())
			{
				if (pOutOld!=NULL)
					*pOutOld = iter->second;
				if (bInsertForce)
				{
					std::map<K, T, P>::erase(iter);
				}else
				{
					return false;
				}
			}
			std::map<K, T, P>::insert(Pair(k, t));
		}
		return true;
	}
	bool find(const K& k, T & out, bool erase)
	{
		if (erase)
		{
			BoostWriteLock wtlock(m_mutex);
			typename std::map<K, T, P>::iterator iter = std::map<K, T, P>::find(k);
			if (iter == std::map<K, T, P>::end())
				return false;
			out = iter->second;
			std::map<K, T, P>::erase(iter);
		}else
		{
			BoostReadLock rdlock(m_mutex);
			typename std::map<K, T, P>::iterator iter = std::map<K, T, P>::find(k);
			if (iter == std::map<K, T, P>::end())
				return false;
			out = iter->second;
		}
		return true;
	}
	bool find(const K& k, T & out) const
	{
		BoostReadLock rdlock(const_cast<boost::shared_mutex&>(m_mutex));
		typename std::map<K, T, P>::const_iterator iter = std::map<K, T, P>::find(k);
		if (iter == std::map<K, T, P>::end())
			return false;
		out = iter->second;
		return true;
	}
	bool exist(const K& k, bool is_lock = true) const
	{
		if (is_lock)
		{
			BoostReadLock rdlock(const_cast<boost::shared_mutex&>(m_mutex));
			typename std::map<K, T, P>::const_iterator iter = std::map<K, T, P>::find(k);
			return iter != std::map<K, T>::end();
		}else
		{
			typename std::map<K, T, P>::const_iterator iter = std::map<K, T, P>::find(k);
			return iter != std::map<K, T>::end();
		}
	}

	bool remove(const K& k, bool is_lock = true)
	{
		if (is_lock)
		{
			BoostWriteLock wtlock(m_mutex);
			typename std::map<K, T>::iterator iter = std::map<K, T>::find(k);
			if (iter != std::map<K, T, P>::end())
			{
				std::map<K, T, P>::erase(iter);
				return true;
			}
		}else
		{
			typename std::map<K, T>::iterator iter = std::map<K, T>::find(k);
			if (iter != std::map<K, T, P>::end())
			{
				std::map<K, T, P>::erase(iter);
				return true;
			}
		}
		return false;
	}

	void clear(bool is_lock = true)
	{
		if (is_lock)
		{
			BoostWriteLock wtlock(m_mutex);
			std::map<K, T, P>::clear();
		}else
		{
			std::map<K, T, P>::clear();
		}
	}
	size_t size(bool is_lock = true) const
	{
		if (is_lock)
		{
			BoostReadLock rdlock(const_cast<boost::shared_mutex&>(m_mutex));
			return std::map<K, T, P>::size();
		}else
		{
			return std::map<K, T, P>::size();
		}
	}
	bool empty(bool is_lock = false) const
	{
		if (is_lock)
		{
			BoostReadLock rdlock(const_cast<boost::shared_mutex&>(m_mutex));
			return std::map<K, T, P>::empty();
		}else
		{
			return std::map<K, T, P>::empty();
		}
	}

public:
	CLockMap(void) {}
	virtual ~CLockMap(void)
	{
		clear();
	}
};

template<typename K, typename T, typename P=std::less<K> >
class CLockMapPtr
	: public CLockMap<K, T, P>
{
public:
	T find(const K& k, bool erase)
	{
		T out = 0;
		CLockMap<K, T, P>::find(k, out, erase);
		return out;
	}
	const T find(const K& k) const
	{
		T out = 0;
		CLockMap<K, T, P>::find(k, out);
		return out;
	}
	bool remove(const K& k)
	{
		T t = find(k, true);
		if (t != 0)
			delete t;
		return t != 0;
	}
	void clear(bool is_lock = true, bool is_delete = true)
	{
		if (is_lock)
		{
			BoostWriteLock wtlock(this->mutex());
			if (is_delete)
				for_each(CLockMap<K, T, P>::begin(), CLockMap<K, T, P>::end(), DeletePair());
			std::map<K, T, P>::clear();
		}else
		{
			if (is_delete)
				for_each(CLockMap<K, T, P>::begin(), CLockMap<K, T, P>::end(), DeletePair());
			std::map<K, T, P>::clear();
		}
	}

public:
	CLockMapPtr(void) {}
	virtual ~CLockMapPtr(void)
	{
		clear();
	}
};

template<typename K, typename T>
class CLockMultiMap
	: public std::multimap<K, T>
{
public:
	typedef std::pair<K, T> Pair;
	typedef typename std::multimap<K, T>::iterator IT;
	typedef typename std::multimap<K, T>::const_iterator CIT;
	typedef typename std::pair<CIT, CIT> Range;

protected:
	boost::shared_mutex m_mutex; 
	//boost::mutex m_mutex;

public:
	boost::shared_mutex & mutex(void) {return m_mutex;}
	const boost::shared_mutex & mutex(void) const {return m_mutex;}
	//boost::mutex & mutex(void) {return m_mutex;}
	//const boost::mutex & mutex(void) const {return m_mutex;}

	void insert(const K& k, const T& t, bool clear = false, bool is_lock = true)
	{
		if (is_lock)
		{
			BoostWriteLock wtlock(m_mutex);
			if (clear)
				std::multimap<K, T>::erase(k);
			std::multimap<K, T>::insert(Pair(k, t));
		}else
		{
			if (clear)
				std::multimap<K, T>::erase(k);
			std::multimap<K, T>::insert(Pair(k, t));
		}
	}
	bool find(const K& k, T & out, bool erase)
	{
		if (erase)
		{
			BoostWriteLock wtlock(m_mutex);
			IT iter = std::multimap<K, T>::find(k);
			if (iter == std::multimap<K, T>::end())
				return false;
			out = iter->second;
			std::multimap<K, T>::erase(iter);
		}else
		{
			BoostReadLock rdlock(const_cast<boost::shared_mutex&>(m_mutex));
			IT iter = std::multimap<K, T>::find(k);
			if (iter == std::multimap<K, T>::end())
				return false;
			out = iter->second;
		}
		return true;
	}
	bool find(const K& k, std::vector<T> & out, bool erase)
	{
		bool result = false;
		if (erase)
		{
			BoostWriteLock wtlock(m_mutex);
			Range range = std::multimap<K, T>::equal_range(k);
			for(CIT iter=range.first; iter!=range.second; ++iter)
			{
				result = true;
				out.push_back(iter->second);
			}
			if (result)
				std::multimap<K, T>::erase(k);
		}else
		{
			BoostReadLock rdlock(const_cast<boost::shared_mutex&>(m_mutex));
			Range range = std::multimap<K, T>::equal_range(k);
			for(CIT iter=range.first; iter!=range.second; ++iter)
			{
				result = true;
				out.push_back(iter->second);
			}
		}
		return result;
	}
	bool find(const K& k, T & out) const
	{
		BoostReadLock rdlock(const_cast<boost::shared_mutex&>(m_mutex));
		CIT iter = std::multimap<K, T>::find(k);
		if (iter == std::multimap<K, T>::end())
			return false;
		out = iter->second;
		return true;
	}
	bool find(const K& k, std::vector<T> & out) const
	{
		BoostReadLock rdlock(const_cast<boost::shared_mutex&>(m_mutex));
		bool result = false;
		Range range = std::multimap<K, T>::equal_range(k);
		for(CIT iter=range.first; iter!=range.second; ++iter)
		{
			result = true;
			out.push_back(iter->second);
		}
		return result;
	}
	size_t sizek(const K& k) const
	{
		BoostReadLock rdlock(const_cast<boost::shared_mutex&>(m_mutex));
		size_t result = 0;
		Range range = std::multimap<K, T>::equal_range(k);
		for(CIT iter=range.first; iter!=range.second; ++iter)
		{
			result++;
		}
		return result;
	}
	bool exist(const K& k, bool is_lock = true) const
	{
		if (is_lock)
		{
			BoostReadLock rdlock(const_cast<boost::shared_mutex&>(m_mutex));
			CIT iter = std::multimap<K, T>::find(k);
			return iter != std::multimap<K, T>::end();
		}else
		{
			CIT iter = std::multimap<K, T>::find(k);
			return iter != std::multimap<K, T>::end();
		}
	}

	void remove(const K& k, bool is_lock = true)
	{
		if (is_lock)
		{
			BoostWriteLock wtlock(m_mutex);
			std::multimap<K, T>::erase(k);
		}else
		{
			std::multimap<K, T>::erase(k);
		}
	}

	void clear(bool is_lock = true)
	{
		if (is_lock)
		{
			BoostWriteLock wtlock(m_mutex);
			std::multimap<K, T>::clear();
		}else
		{
			std::multimap<K, T>::clear();
		}
	}

public:
	CLockMultiMap(void) {}
	virtual ~CLockMultiMap(void)
	{
		clear();
	}
};

#endif // __lockmap_h__

