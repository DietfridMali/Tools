// Copyright (c) 2025 Dietfrid Mali
// This software is licensed under the MIT License.
// See the LICENSE file for more details.

#pragma once

#include "array.hpp"

//-----------------------------------------------------------------------------
// list of data buffers for other data containers

template < class _T > 
class DataPool {

	template < class _U > 
	class PoolElem {
		public:
			int32_t	prev;
			int32_t	next;
			_U		data;
		};

	class PoolBuffer : public Array< PoolElem <_T> > {};

	private:
		PoolBuffer		m_buffer;
		//_T*			m_null;
		int32_t			m_free;
		int32_t			m_used;

	public:
		DataPool () { Init (); }
		~DataPool() { Destroy (); }

		//_T& operator[] (uint32_t i) { return m_buffer [i].data; }

		inline void Init (void) { 
			m_buffer.Init ();
			m_free = 
			m_used = -1; 
			}

		inline void Destroy (void) { 
			m_buffer.Destroy (); 
			Init (); 
			}

		inline bool Create (uint32_t size, const char* pszName = "DataPool::m_buffer") { 
			Destroy ();
			if (!m_buffer.Create (size, pszName))
				return false;
			uint32_t i;
			for (i = 0; i < size; i++) {
				m_buffer [i].prev = i - 1;
				m_buffer [i].next = i + 1;
				}
			m_buffer [i - 1].next = -1;
			m_free = 0;
			m_used = -1;
			return true;
			}

		inline _T* Pop (void) { 
			if (m_free < 0)
				return NULL;
			PoolElem<_T>& e = m_buffer [m_free];
			int32_t next = e.next;
			e.prev = -1;
			e.next = m_used;
			if (m_used >= 0)
				m_buffer [m_used].prev = m_free;
			m_used = m_free;
			m_free = next;
			return &e.data; 
			}

		void Push (uint32_t i) {
			if (m_used < 0)
				return;
			PoolElem<_T>& e = m_buffer [i];
			if (e.prev < 0)
				m_used = e.next;
			else
				m_buffer [e.prev].next = e.next;
			if (e.next >= 0)
				m_buffer [e.next].prev = e.prev;
			e.prev = -1;
			e.next = m_free;
			if (m_free >= 0)
				m_buffer [m_free].prev = i;
			m_free = i;
			}

		inline int32_t LastIndex (void) { return m_used; }

		inline _T* GetNext (int32_t& nCurrent) { 
			if ((nCurrent < 0) || !m_buffer.Data ())
				return NULL;
			PoolElem<_T>* e;
			try {
				e = &m_buffer [nCurrent]; 
				}
			catch(...) {
#if DBG
				ArrayError ("Data pool element chain broken.");
#endif
				return NULL;
				}
			nCurrent = e->next;
			return &e->data;
			}

		inline _T* GetFirst (int32_t& nCurrent) { 
			if (nCurrent < 0)
				nCurrent = m_used;
			return GetNext (nCurrent);
			}

		inline int32_t Size (void) { return m_buffer.Size (); }

		inline int32_t UsedList (void) { return m_used; }
		inline int32_t FreeList (void) { return m_free; }

		inline _T& operator[] (uint32_t i) { return m_buffer [i].data; }
		inline _T* operator+ (uint32_t i) { return &m_buffer [i].data; }
	};


//-----------------------------------------------------------------------------
