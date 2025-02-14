// Copyright (c) 2025 Dietfrid Mali
// This software is licensed under the MIT License.
// See the LICENSE file for more details.

#pragma once

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <type_traits>
#include <cstddef>
#include <utility>
#include <iostream>
#include "array.hpp"

#if 0

#include <list>

template < class DATA_T >
class List : public std::list {

private:
	auto DataPtr (int i) {
		for (auto iter = this->begin (); iter != this->end (); iter++)
			if (!i--)
				return iter;
		return nullptr;
	}

public:
	inline bool Append (DATA_T data) {
		return this->push_back (data);
	}

	DATA_T* Add (int i) {
		auto p = DataPtr (i);
		if (p != nullptr)
			p = this->emplace (p);
		return (p == nullptr) ? nullptr ? *p;
};

#else

#define USE_DATA_PTR	0
#if USE_DATA_PTR
#	define DATA_PTR(data)	(data)
#	define DATA_VALUE(data)	(*(data))
#else
#	define DATA_PTR(data)	(&data)
#	define DATA_VALUE(data)	(data)
#endif

//-----------------------------------------------------------------------------

template < typename DATA_T >
class List {

	private:
		class ListNode;

		//----------------------------------------

		class ListNodePtr {
			public:
				ListNode* m_ptr;

				explicit ListNodePtr () : m_ptr (nullptr) {}
/*
				ListNodePtr(const ListNodePtr& other) {
					m_ptr = other.m_ptr;
				}
*/
				explicit ListNodePtr(ListNode * ptr) : m_ptr(ptr) {}

				ListNodePtr& operator= (ListNodePtr other) {
					m_ptr = other.m_ptr;
					return *this;
				}

				ListNodePtr& operator= (ListNode * p) {
					m_ptr = p;
					return *this;
				}

				void operator++ (int) {
					*this = m_ptr->m_next;
				}

				void operator-- (int) {
					*this = m_ptr->m_prev;
				}

				ListNodePtr operator+ (int n) {
					ListNodePtr ptr = *this;
					for (; n; n--)
						ptr++;
					return ptr;
				}

				ListNodePtr operator- (int n) {
					ListNodePtr ptr = *this;
					for (; n; n--)
						ptr--;
					return ptr;
				}

				ListNode* operator->() const {
					return m_ptr;
				}

				bool operator== (const ListNodePtr& other) {
					return m_ptr == other.m_ptr;
				}

				bool operator!= (const ListNodePtr& other) {
					return m_ptr != other.m_ptr;
				}

				operator bool() {
					return (m_ptr != nullptr);
				}
			};

		//----------------------------------------

		class ListNode {
			public:
				ListNodePtr	m_prev;
				ListNodePtr	m_next;
#if USE_DATA_PTR
				DATA_T *	m_data;
#else
				DATA_T		m_data;
#endif

#if USE_DATA_PTR
				explicit ListNode() : m_prev (nullptr), m_next (nullptr), m_data (nullptr) {}
#else
				explicit ListNode () : m_prev (nullptr), m_next (nullptr) {}
#endif

				~ListNode() {
#if USE_DATA_PTR
					if (m_data) {
						delete m_data;
						m_data = nullptr;
					}
#endif
				}

		};

		//----------------------------------------

		class Iterator {

			private:
				ListNodePtr 	m_first;
				ListNodePtr 	m_last;
				ListNodePtr 	m_current;
				size_t			m_index;

			public:
				explicit Iterator() {}

				Iterator(List< DATA_T >& l) : m_first(l.First ()), m_last(l.Last () + 1), m_index (0) {}

				operator bool() const { return m_current != nullptr; }

				//DATA_T* operator*() const { return &m_current->m_data; }
				// return the current value and the index of the value in the list
				constexpr std::pair<size_t, DATA_T&> operator*() const { 
					return std::pair<size_t, DATA_T&> {m_index, DATA_VALUE (m_current->m_data) };
				}

				Iterator& operator++() {
					m_current++;
					m_index++;
					return *this;
				}

				Iterator& operator--() {
					m_current--;
					return *this;
				}

				bool operator== (Iterator& other) {
					return m_current == other.m_current;
				}

				bool operator!= (Iterator& other) {
					return m_current != other.m_current;
				}

				//Iterator& Start(void) {
				constexpr Iterator& First(void) {
					m_index = 0;
					m_current = m_first;
					return *this;
				}

				Iterator& Last(void) {
					m_current = m_last;
					return *this;
				}
			};

	// ----------------------------------------

	private:
		ListNode		m_head;
		ListNode		m_tail;
#if USE_DATA_PTR
		DATA_T*		m_none;
#else
		DATA_T		m_none;
#endif
		ListNodePtr	m_headPtr;
		ListNodePtr 	m_tailPtr;
		ListNodePtr 	m_nullPtr;
		size_t		m_length;
		bool		m_result;

	public:
		void Destroy(void) {
			if (m_headPtr) {
				ListNodePtr p, n;
				for (n = m_headPtr + 1; n != m_tailPtr; ) {
					p = n;
					n++;
					if (p.m_ptr) {
						delete p.m_ptr;
						p.m_ptr = nullptr;
					}
				}
			}
#if USE_DATA_PTR
			if (m_none) {
				delete m_none;
				m_none = nullptr;
			}
#endif
			Init();
		}

		List<DATA_T>& Copy(const List<DATA_T> & other) {
			Destroy();
			ListNodePtr p = other.m_headPtr;
			for (p++; p != other.m_tailPtr; p++)
				Insert (-1, DATA_VALUE (p.m_ptr->m_data));
			return *this;
		}

		inline void Init(void) {
			m_headPtr = &m_head;
			m_tailPtr = &m_tail;
			m_headPtr->m_prev = m_tailPtr->m_next = nullptr;
			m_headPtr->m_next = &m_tail;
			m_tailPtr->m_prev = &m_head;
#if USE_DATA_PTR
			m_none = new DATA_T;
#endif
			m_length = 0;
		}

		Iterator begin() {
			return Iterator(*this).First();
		}

		Iterator end() {
			return Iterator(*this).Last();
		}

		inline ListNodePtr Head(void) {
			return m_headPtr;
		}

		inline ListNodePtr Tail(void) {
			return m_tailPtr;
		}

		inline ListNodePtr First(void) {
			return m_head.m_next;
		}

		inline ListNodePtr Last(void) {
			return m_tail.m_prev;
		}

		inline size_t Length(void) {
			return m_length;
		}

		inline const bool Empty(void) const {
			return m_length == 0;
		}

		inline DATA_T& operator[] (const size_t i) {
			ListNodePtr p = NodePtr(int (i), m_headPtr + 1, m_tailPtr - 1);
			if (p) {
				m_result = true;
				return DATA_VALUE (p->m_data);
			}
			m_result = true;
			return DATA_VALUE (m_none);
		}
#if 0
		inline DATA_T& operator[] (DATA_T& d) {
			int i = Find(d);
			if (i < 0)
				return DATA_VALUE (m_none);
			ListNodePtr p = NodePtr(int(i), m_headPtr + 1, m_tailPtr - 1);
			return p ? DATA_VALUE (p->m_data) : DATA_VALUE (m_none);
		}
#endif
		inline List<DATA_T>& operator= (List<DATA_T> const& other) {
			Copy(other);
			return *this;
		}

		inline List<DATA_T>& operator= (List<DATA_T>&& other) noexcept {
			return Grab (other);
		}

		inline List<DATA_T>& operator= (std::initializer_list<DATA_T> data) {
			Destroy();
			for (auto const& d : data)
				Append(d);
			return *this;
		}

		explicit List() : 
#if USE_DATA_PTR
			m_none (nullptr),
#endif
			m_length (0), m_result (true) {
			Init();
		}

		List(List<DATA_T> const& other) :
#if USE_DATA_PTR
			m_none (nullptr),
#endif
			m_length (0), m_result (true) {
			Copy(other);
		}
		
		List(List<DATA_T>&& other) noexcept :
#if USE_DATA_PTR
			m_none (nullptr),
#endif
			m_length (0), m_result (true) {
			Init ();
			Grab(other);
		}

		explicit List(DATA_T& data) : 
#if USE_DATA_PTR
			m_none(nullptr),
#endif
			m_length(0), m_result(true) {
#if USE_DATA_PTR
			m_none = new DATA_T;
#endif
			Append(data);
		}

		explicit List(Array<DATA_T>& data) : 
#if USE_DATA_PTR
			m_none (nullptr),
#endif
			m_length (0), m_result (true) {
			Init ();
			for (auto const& v : data)
				Append(*v);
		}

		List(std::initializer_list<DATA_T> data) : 
#if USE_DATA_PTR
			m_none (nullptr),
#endif
			m_length (0), m_result (true) {
			Init ();
			for (auto const& d : data)
				Append(d);
		}

		~List() {
			Destroy();
		}

//-----------------------------------------------------------------------------

private:
ListNodePtr NodePtr(int i, ListNodePtr first, ListNodePtr last) {
	if (i == 0)
		return first;
	if (i == -1)
		return last;
	ListNodePtr p;
	if (i > 0) {
		int32_t h = int (m_length - i);
		if (h < i)
			i = -h;
	}
	if (i >= 0) {
		p = first;
		for (; i && (p != m_tailPtr); i--, p++)
			;
	}
	else
		for (p = last; ++i && (p != m_headPtr); p = p->m_prev)
			;
	return i ? m_nullPtr : p;

}

//-----------------------------------------------------------------------------

public:
DATA_T* Add(int i) {
	ListNodePtr p = NodePtr(i, m_headPtr + 1, m_tailPtr);
	if (!p)
		return nullptr;
	ListNode* n = new ListNode;
	if (!n)
		return nullptr;
#if USE_DATA_PTR
	if (!(n->m_data = new DATA_T)) {
		delete n;
		return nullptr;
	}
#else
#endif
	n->m_prev = p->m_prev;
	n->m_prev->m_next = n;
	n->m_next = p;
	p->m_prev = n;
	m_length++;
#if USE_DATA_PTR
	return n->m_data;
#else
	return &n->m_data;
#endif
}

//-----------------------------------------------------------------------------

public:
bool Insert (int i, DATA_T data) {
	DATA_T * dataPtr = Add (i);
	if (!dataPtr)
		return false;
	*dataPtr = data;
	return true;
}

//-----------------------------------------------------------------------------

public:
auto Pop(int i) {
	m_result = false;
	if (!m_length)
		return DATA_VALUE (m_none);

	ListNodePtr nodePtr = NodePtr(i, m_headPtr + 1, m_tailPtr - 1);
	if (!nodePtr)
		return DATA_VALUE (m_none);
	ListNode* p = nodePtr.m_ptr;
	DATA_T data = DATA_VALUE (p->m_data);
	p->m_prev->m_next = p->m_next;
	p->m_next->m_prev = p->m_prev;
	delete p;
	m_length--;
	m_result = true;
	return data;
}

//-----------------------------------------------------------------------------

#if 0
public:
auto Pop(DATA_T data) {
	for (auto [i, d] : *this)
		if (d == data)
			return Pop(i);
	return DATA_VALUE (m_none);
}
#endif

//-----------------------------------------------------------------------------
// move the other list to the end of this list
// will leave other list empty

public:
List< DATA_T >& operator+= (List< DATA_T > other) {
	if (other.Empty())
		return *this;
#if 1
	for (auto [i, d] : other)
		Insert (-1, d);
#else
	ListNode* s = other.m_head.m_next.m_ptr;
	ListNode* e;
	if (Empty()) {
		m_headPtr.m_ptr->m_next = s;
		s->m_prev = m_headPtr;
	}
	else {
		e = m_tailPtr.m_ptr->m_prev.m_ptr;
		e->m_next = s;
		s->m_prev = e;
	}
	e = other.m_tail.m_prev.m_ptr;
	e->m_next = &m_tail;
	m_tail.m_prev = e;
	m_tailPtr = &m_tail;
	other.Init ();
#endif
	return *this;
}

//-----------------------------------------------------------------------------
// move the other list to the end of this list
// will leave other list empty

public:
List< DATA_T >& Grab (List< DATA_T >& other) {
	Destroy ();
	if (other.Empty ())
		return *this;
	ListNode* s = other.m_head.m_next.m_ptr;
	ListNode* e;
	if (Empty ()) {
		m_headPtr.m_ptr->m_next = s;
		s->m_prev = m_headPtr;
	}
	else {
		e = m_tailPtr.m_ptr->m_prev.m_ptr;
		e->m_next = s;
		s->m_prev = e;
	}
	e = other.m_tail.m_prev.m_ptr;
	e->m_next = m_tailPtr;
	m_tailPtr->m_prev = e;
	m_length = other.m_length;
	other.Init ();
	return *this;
}

//-----------------------------------------------------------------------------

public:
List< DATA_T > operator+ (const List< DATA_T >& other) {
	List< DATA_T > l;
	if (Empty())
		return l = other;
	if (other.Empty())
		return l = *this;
	l = *this;
	l += other;
	return l;
}

//-----------------------------------------------------------------------------

public:
int Find(DATA_T data) {
#if 1
	for (const auto [i, p] : *this)
		if (p == data)
			return int (i);
#else
	int i = 0;
	for (ListNodePtr p = m_headPtr->m_next; p != m_tailPtr; p++, i++)
		if (*p.m_ptr->m_data == data)
			return int (i);
#endif
	return -1;
}

//-----------------------------------------------------------------------------

public:
List< DATA_T > Splice(size_t from, size_t to) {
	ListNode* start = NodePtr(int (from));
	ListNode* end = NodePtr(int (to ? to : m_length - 1));
	List< DATA_T > l;
	if ((start != nullptr) && (end != nullptr)) {
		for (ListNode* i = start; i != end; i = i->next) {
			ListNode& n = l.Append(new ListNode);
			*n.m_data = *i.m_data;
		}
	}
	return l;
}

//-----------------------------------------------------------------------------

public:
inline bool Append(DATA_T data) {
	return Insert(-1, data);
}

//-----------------------------------------------------------------------------

public:
inline bool Remove(DATA_T data) {
	int i = Find(data);
	if (i < 0)
		return (m_result = false);
	Pop(size_t(i));
	return (m_result = true);
}

//-----------------------------------------------------------------------------

public:
inline bool Result(void) {
	return m_result;
}

//-----------------------------------------------------------------------------

};

#endif