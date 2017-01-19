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
	const CLockMap<K,T,P>& operator =(const CLockMap<K,T,P>& pMap)
	{
		BoostWriteLock wtlock(m_mutex);
		BoostReadLock rdlock(const_cast<boost::shared_mutex&>(pMap.mutex()));
		typename CLockMap<K,T,P>::const_iterator pIter = pMap.begin();
		for (; pIter!=pMap.end(); pIter++)
		{
			std::map<K, T, P>::insert(Pair(pIter->first, pIter->second));
		}
		return *this;
	}
	const CLockMap<K,T,P>& operator =(const CLockMap<K,T,P>* pMap)
	{
		if (pMap!=NULL)
			return this->operator =(*pMap);
		return *this;
	}

	bool insert(const K& k, const T& t,bool bInsertForce=true,T* pOutOld=NULL, bool is_lock = true)
	{
		std::pair< typename std::map<K,T,P>::iterator, bool> ret;
		if (is_lock)
		{
			BoostWriteLock wtlock(m_mutex);
			if (bInsertForce || pOutOld!=NULL)
			{
				typename std::map<K, T, P>::iterator iter = std::map<K, T, P>::find(k);
				if (iter != std::map<K, T, P>::end())
				{
					if (pOutOld!=NULL)
						*pOutOld = iter->second;
					if (bInsertForce)
					{
						iter->second = t;
						return true;
						//std::map<K, T, P>::erase(iter);
					}else
					{
						return false;
					}
				}
			}
			ret = std::map<K, T, P>::insert(Pair(k, t));
		}else
		{
			if (bInsertForce || pOutOld!=NULL)
			{
				typename std::map<K, T, P>::iterator iter = std::map<K, T, P>::find(k);
				if (iter != std::map<K, T, P>::end())
				{
					if (pOutOld!=NULL)
						*pOutOld = iter->second;
					if (bInsertForce)
					{
						iter->second = t;
						return true;
						//std::map<K, T, P>::erase(iter);
					}else
					{
						return false;
					}
				}
			}
			ret = std::map<K, T, P>::insert(Pair(k, t));
		}
		return ret.second;
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
	// pOutK, pOutT can = NULL
	bool get_begin(K* pOutK, T* pOutT, bool erase)
	{
		if (erase)
		{
			BoostWriteLock wtlock(m_mutex);
			typename std::map<K, T, P>::iterator iter = std::map<K, T, P>::begin();
			if (iter == std::map<K, T, P>::end())
				return false;
			if (pOutK!=NULL)
				*pOutK = iter->first;
			if (pOutT!=NULL)
				*pOutT = iter->second;
			std::map<K, T, P>::erase(iter);
		}else
		{
			BoostReadLock rdlock(m_mutex);
			typename std::map<K, T, P>::iterator iter = std::map<K, T, P>::begin();
			if (iter == std::map<K, T, P>::end())
				return false;
			if (pOutK!=NULL)
				*pOutK = iter->first;
			if (pOutT!=NULL)
				*pOutT = iter->second;
		}
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
	bool exist(const K& k, const T& t, bool is_lock) const
	{
		if (is_lock)
		{
			BoostReadLock rdlock(const_cast<boost::shared_mutex&>(m_mutex));
			typename std::map<K, T, P>::const_iterator iter = std::map<K, T, P>::find(k);
			return iter != std::map<K, T>::end() && iter->second==t;
		}else
		{
			typename std::map<K, T, P>::const_iterator iter = std::map<K, T, P>::find(k);
			return iter != std::map<K, T>::end() && iter->second==t;
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
	bool remove(const K& k, const T& t, bool is_lock, T* pOutOld=NULL)
	{
		if (is_lock)
		{
			BoostWriteLock wtlock(m_mutex);
			typename std::map<K, T>::iterator iter = std::map<K, T>::find(k);
			if (iter != std::map<K, T, P>::end())
			{
				if (pOutOld!=NULL)
					*pOutOld = iter->second;
				if (iter->second==t)
				{
					std::map<K, T, P>::erase(iter);
					return true;
				}
			}
		}else
		{
			typename std::map<K, T>::iterator iter = std::map<K, T>::find(k);
			if (iter != std::map<K, T, P>::end())
			{
				if (pOutOld!=NULL)
					*pOutOld = iter->second;
				if (iter->second==t)
				{
					std::map<K, T, P>::erase(iter);
					return true;
				}
			}
		}
		return false;
	}
	size_t removet(const T& t, bool is_lock = true)
	{
		size_t result = 0;
		if (is_lock)
		{
			BoostWriteLock wtlock(m_mutex);
			typename std::map<K, T, P>::iterator iter = std::map<K, T, P>::begin();
			for (; iter!=std::map<K, T, P>::end(); )
			{
				if (iter->second==t)
				{
					result++;
					std::map<K, T, P>::erase(iter++);
				}else
				{
					iter++;
				}
			}
		}else
		{
			typename std::map<K, T, P>::iterator iter = std::map<K, T, P>::begin();
			for (; iter!=std::map<K, T, P>::end(); )
			{
				if (iter->second==t)
				{
					result++;
					std::map<K, T, P>::erase(iter++);
				}else
				{
					iter++;
				}
			}
		}
		return result;
		//size_t result = 0;
		//bool bContinue = true;
		//if (is_lock)
		//{
		//	BoostWriteLock wtlock(m_mutex);
		//	while (bContinue)
		//	{
		//		bContinue = false;
		//		typename std::map<K, T, P>::iterator iter = std::map<K, T, P>::begin();
		//		for (; iter!=std::map<K, T, P>::end(); iter++)
		//		{
		//			if (iter->second==t)
		//			{
		//				result++;
		//				bContinue = true;
		//				std::map<K, T, P>::erase(iter);
		//				break;
		//			}
		//		}
		//	}
		//}else
		//{
		//	while (bContinue)
		//	{
		//		bContinue = false;
		//		typename std::map<K, T, P>::iterator iter = std::map<K, T, P>::begin();
		//		for (; iter!=std::map<K, T, P>::end(); iter++)
		//		{
		//			if (iter->second==t)
		//			{
		//				result++;
		//				bContinue = true;
		//				std::map<K, T, P>::erase(iter);
		//				break;
		//			}
		//		}
		//	}
		//}
		//return result;
	}
	size_t updatet(const T& ot, const T& nt, bool update_all = true, bool is_lock = true)
	{
		size_t result = 0;
		if (is_lock)
		{
			BoostWriteLock wtlock(m_mutex);
			typename std::map<K, T, P>::iterator iter = std::map<K, T, P>::begin();
			for (; iter!=std::map<K, T, P>::end(); iter++)
			{
				if (iter->second==ot)
				{
					result++;
					iter->second = nt;
					if (!update_all) break;
				}
			}
		}else
		{
			typename std::map<K, T, P>::iterator iter = std::map<K, T, P>::begin();
			for (; iter!=std::map<K, T, P>::end(); iter++)
			{
				if (iter->second==ot)
				{
					result++;
					iter->second = nt;
					if (!update_all) break;
				}
			}
		}
		return result;
	}
	// *** 使用可以，KEY改变，要注意索引排序问题，一般用在小数据量，对排序无要求场景
	size_t updatek(const K& ok, const K& nk, bool update_all = true, bool is_lock = true)
	{
		size_t result = 0;
		if (is_lock)
		{
			BoostWriteLock wtlock(m_mutex);
			typename std::map<K, T, P>::iterator iter = std::map<K, T, P>::begin();
			for (; iter!=std::map<K, T, P>::end(); iter++)
			{
				if (iter->first==ok)
				{
					result++;
					const_cast<K&>(iter->first) = nk;
					if (!update_all) break;
				}
			}
		}else
		{
			typename std::map<K, T, P>::iterator iter = std::map<K, T, P>::begin();
			for (; iter!=std::map<K, T, P>::end(); iter++)
			{
				if (iter->first==ok)
				{
					result++;
					const_cast<K&>(iter->first) = nk;
					if (!update_all) break;
				}
			}
		}
		return result;
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
	typedef typename std::pair<CIT, CIT> ConstRange;
	typedef typename std::pair<IT, IT> Range;

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
	//bool insert(const K& k, const T& t,bool bInsertForce,T* pOutOld, bool is_lock = true)
	//{
	//	if (is_lock)
	//	{
	//		BoostWriteLock wtlock(m_mutex);
	//		IT iter = std::multimap<K, T>::find(k);
	//		if (iter != std::multimap<K, T>::end())
	//		{
	//		}
	//	}else
	//	{
	//	}
	//}
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
			ConstRange range = std::multimap<K, T>::equal_range(k);
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
			ConstRange range = std::multimap<K, T>::equal_range(k);
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
		ConstRange range = std::multimap<K, T>::equal_range(k);
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
		return std::multimap<K, T>::count(k);
		//size_t result = 0;
		//ConstRange range = std::multimap<K, T>::equal_range(k);
		//for(CIT iter=range.first; iter!=range.second; ++iter)
		//{
		//	result++;
		//}
		//return result;
	}
	size_t sizet(const T& t) const
	{
		size_t result = 0;
		BoostReadLock rdlock(const_cast<boost::shared_mutex&>(m_mutex));
		CIT iter = std::multimap<K, T>::begin();
		for (; iter!=std::multimap<K, T>::end(); iter++)
		{
			if (iter->second==t)
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
	bool exist(const K& k, const T& t, bool is_lock = true) const
	{
		if (is_lock)
		{
			BoostReadLock rdlock(const_cast<boost::shared_mutex&>(m_mutex));
			ConstRange range = std::multimap<K, T>::equal_range(k);
			for(CIT iter=range.first; iter!=range.second; ++iter)
			{
				if (iter->second==t) return true;
			}
		}else
		{
			ConstRange range = std::multimap<K, T>::equal_range(k);
			for(CIT iter=range.first; iter!=range.second; ++iter)
			{
				if (iter->second==t) return true;
			}
		}
		return false;
	}
	bool existt(const T& t, bool is_lock = true) const
	{
		if (is_lock)
		{
			BoostReadLock rdlock(const_cast<boost::shared_mutex&>(m_mutex));
			CIT iter = std::multimap<K, T>::begin();
			for (; iter!=std::multimap<K, T>::end(); iter++)
			{
				if (iter->second==t) return true;
			}
		}else
		{
			CIT iter = std::multimap<K, T>::begin();
			for (; iter!=std::multimap<K, T>::end(); iter++)
			{
				if (iter->second==t) return true;
			}
		}
		return false;
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
	bool remove(const K& k, const T& t, bool is_lock)
	{
		if (is_lock)
		{
			BoostWriteLock wtlock(m_mutex);
			Range range = std::multimap<K, T>::equal_range(k);
			for(IT iter=range.first; iter!=range.second; ++iter)
			{
				if (iter->second==t)
				{
					std::multimap<K, T>::erase(iter);
					return true;
				}
			}
		}else
		{
			Range range = std::multimap<K, T>::equal_range(k);
			for(IT iter=range.first; iter!=range.second; ++iter)
			{
				if (iter->second==t)
				{
					std::multimap<K, T>::erase(iter);
					return true;
				}
			}
		}
		return false;
	}
	size_t removet(const T& t, bool is_lock)
	{
		size_t result = 0;
		bool bContinue = true;
		if (is_lock)
		{
			BoostWriteLock wtlock(m_mutex);
			while (bContinue)
			{
				bContinue = false;
				IT iter = std::multimap<K, T>::begin();
				for (; iter!=std::multimap<K, T>::end(); iter++)
				{
					if (iter->second==t)
					{
						result++;
						std::multimap<K, T>::erase(iter);
						bContinue = true;
						break;
					}
				}
			}
		}else
		{
			while (bContinue)
			{
				bContinue = false;
				IT iter = std::multimap<K, T>::begin();
				for (; iter!=std::multimap<K, T>::end(); iter++)
				{
					if (iter->second==t)
					{
						result++;
						std::multimap<K, T>::erase(iter);
						bContinue = true;
						break;
					}
				}
			}
		}
		return result;
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

