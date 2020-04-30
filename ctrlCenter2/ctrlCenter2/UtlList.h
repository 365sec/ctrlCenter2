#ifndef _UTILITY_LIST_H__
#define _UTILITY_LIST_H__

#include <list>
#include <map>
#include <assert.h>
using namespace std;
/**
 Dynamic UtlList that uses a circular buffer for performance.

 Faster than std::deque for objects with constructors.
 */

template <class T>
class UtlList {
private:

    //Only m_nMaxSize elements are initialized.
    T*                  m_pData;
	T*					m_pRelocated;

    //Number of elements (including head) that are visible and initialized.
    int                 m_nMaxSize;
	int					m_nRelctd;
    
    //Size of m_pData array in elements.
    //int                 numAllocated;

	int					m_nCurUsed;

	//For Debug use
	int					m_nErr;

public:
	std::list<T*>		m_listUsed;
	std::list<T*>		m_listUnUsed;
	std::map<int, T*>	m_mapUsed;
	
/*
	void Free(T * pItem)
	{
		assert(pItem != NULL && pItem >= &m_pData[0] && pItem < &m_pData[m_nMaxSize]);
		m_listUsed.remove(pItem);
		m_listUnUsed.push_back(pItem);
	}
	void Free(std::list<T *>::iterator &iterator)
	{
		T* pItem = *iterator;
		assert(pItem != NULL && pItem >= &m_pData[0] && pItem < &m_pData[m_nMaxSize]);
		iterator = m_listUsed.erase(iterator);
		m_listUnUsed.push_back(pItem);
	}
	BOOL Push(T &item)
	{
		T *p = Alloc();
		if (p == NULL)
			return FALSE;
		//Attention:::: 拷贝方式不一定合适?????
		memcpy(p, &item, sizeof(T));
		m_listUsed.push_back(p);
		return TRUE;
	}
	*/
	void MapFree(T * pItem)
	{
		assert(pItem != NULL && pItem >= &m_pData[0] && pItem < &m_pData[m_nMaxSize]);
		typename std::map<int, T*>::iterator itr = m_mapUsed.find(pItem->nID);
		if (itr == m_mapUsed.end())
		{
			//cannot find
			++m_nErr;
		}
		else
		{
			assert (itr->second == pItem);
			m_mapUsed.erase(itr);
			m_listUnUsed.push_back(pItem);
		}
	}
	void Free(typename std::map<int, T*>::iterator &itr)
	{
		//Attention!!!!!!!!!!!!!
		// MAP  erase 方法在不同的编译器下面有不同的结果，有的有返回值,有的没有
		// VC下面有返回值, C++BUILDER没有返回值
		if (itr == m_mapUsed.end())
			return;
		T* pItem = itr->second;
		assert(pItem != NULL && pItem >= &m_pData[0] && pItem < &m_pData[m_nMaxSize]);
		m_mapUsed.erase(itr++);
		m_listUnUsed.push_back(pItem);
	}
	T* Alloc()
	{
		if (m_listUnUsed.size() == 0)
			return NULL;
		T * pItem = m_listUnUsed.front();
		m_listUnUsed.pop_front();
		assert(pItem != NULL && pItem >= &m_pData[0] && pItem < &m_pData[m_nMaxSize]);
		return pItem;
	}
	bool Push(const int &nID, T &item)
	{
		//Attention:::: 拷贝方式不一定合适?????
		
		typename std::map<int, T*>::iterator itr = m_mapUsed.find(nID);
		if (itr == m_mapUsed.end())
		{
			//cannot find, append
			T *p = Alloc();
			if (p == NULL)
				return false;
			memcpy(p, &item, sizeof(T));
			m_mapUsed.insert(std::pair<int, T*>(nID, p));
		}
		else
		{
			//if find, replace
			T *pItem = itr->second;
			assert(pItem != NULL && pItem >= &m_pData[0] && pItem < &m_pData[m_nMaxSize]);
			memcpy(pItem, &item, sizeof(T));
		}
		return true;
	}
	void Clear()
	{
		m_listUsed.clear();
		m_listUnUsed.clear();
		m_mapUsed.clear();
		
		if (m_pData)
			delete []m_pData;
		m_pData = NULL;
		m_nMaxSize = 0;

		if (m_pRelocated)
			delete []m_pRelocated;
		m_pRelocated = NULL;
		m_nRelctd = 0;
	}
	T * GetObj(const int &nID)
	{
		typename std::map<int, T*>::iterator itr = m_mapUsed.find(nID);
		if (itr == m_mapUsed.end())
			return NULL;
		else
		{
			T * pItem = itr->second;
			assert(pItem != NULL && pItem >= &m_pData[0] && pItem < &m_pData[m_nMaxSize]);
			return pItem;
		}
	}
	inline int Size(){return m_nMaxSize;}	//返回内存项
	inline int Count(){return m_mapUsed.size();}//m_listUsed.size();} //返回当前使用的项

    //Allocates newSize elements and repacks the array.
    //void repackAndRealloc(int newSize) {    }

    //Ensure that there is at least one element between
    //  the tail and head, wrapping around in the circular buffer.
    //inline void reserveSpace() {    }

public:

    UtlList() : 
      m_pData(NULL), m_pRelocated(NULL),
      m_nMaxSize(0), m_nRelctd(0), m_nErr(0){
    }

	bool Init(const int &num)
	{
		if (num <= 0)
			return false;

		Clear();
		
		m_pData = new T[num];
		if (m_pData == NULL)
			return false;
		m_nMaxSize = num;
		//m_listUsed.clear();
		//m_listUnUsed.clear();
		for (int i=0;i<num;i++)
			m_listUnUsed.push_back(&m_pData[i]);
		return true;
	}
	
    /**
    Copy constructor
    */
    //UtlList(const UtlList& other) : m_pData(NULL) {
    //   _copy(other);
    //}


   /**
    Destructor does not delete() the objects if T is a pointer type
    (e.g. T = int*) instead, it deletes the pointers themselves and 
    leaves the objects.  Call deleteAll if you want to dealocate
    the objects referenced.
    */
    virtual ~UtlList() {
        Clear();
    }

   /**
    Removes all elements (invoking their destructors).

    @param freeStorage If false, the underlying array is not deallocated
    (allowing fast push in the future), however, the size of the UtlList
    is reported as zero.
    
   void clear(bool freeStorage = true) {

       FIND_ENDS;
       
       // Invoke the destructors on the elements
       int i;
       for (i = head; i < firstEnd; ++i) {
           (m_pData + i)->~T();
       }

       for (i = 0; i < secondEnd; ++i) {
           (m_pData + i)->~T();
       }
       
       m_nMaxSize = 0;
       head = 0;
       if (freeStorage) {
           numAllocated = 0;
           System::free(m_pData);
           m_pData = NULL;
       }
   }
    */

   /**
    Assignment operator.
   UtlList& operator=(const UtlList& other) {
       clear();
       _copy(other);
       return *this;
   }
    */


}; // namespace

#endif

