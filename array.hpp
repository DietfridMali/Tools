// Copyright (c) 2025 Dietfrid Mali
// This software is licensed under the MIT License.
// See the LICENSE file for more details.

#pragma once

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>

#if 0	// use std::vector

#include <vector>

template < typename DATA_T, typename KEY_T >
class Array : public std::vector < DATA_T >
{
public:
	inline DATA_T* Create (size_t length) {
		this->reserve (length);
		this->resize (length);
		return this->data ();
	}

	inline void Destroy(void) {
		this->clear ();
	}

	inline DATA_T* Data(void) {
		return this->data ();
	}
};


#else

#include "quicksort.hpp"

#define sizeofa(_a)	((sizeof(_a) / sizeof(*(_a))))

//-----------------------------------------------------------------------------

template < typename DATA_T > 
class Array : public QuickSort < DATA_T > {

	// ----------------------------------------

	class ArrayInfo {
		public:
			DATA_T*	data;
			DATA_T	none;
			size_t	length;
			size_t	pos;
			size_t	mode;
			bool	wrap;

		public:

			ArrayInfo() : data(nullptr), length(0), pos(0), mode(0), wrap(false) {
				memset(&none, 0, sizeof(none));
			}

			inline size_t Capacity (void) { return length; }
		};

	protected:
		ArrayInfo		m_info;

		// ----------------------------------------

	public:
		class Iterator {
			private:
				DATA_T*		m_start;
				DATA_T*		m_end;
				DATA_T*		m_current;

			public:
				explicit Iterator () : m_start (nullptr), m_end (nullptr), m_current (nullptr) {}

				Iterator (Array& a) : m_start (a.Start ()), m_end (a.End () + 1), m_current (nullptr) {}

				operator bool() const { 
					return m_current != nullptr; 
				}

				DATA_T* operator*() const { 
					return m_current; 
				}

				Iterator& operator++() { 
					++m_current;
					return *this;
				}

				Iterator& operator--() { 
					--m_current;
					return *this;
				}

				Iterator& First (void) {
					m_current = m_start;
					return *this;
				}

				Iterator& Last (void) {
					m_current = m_end;
					return *this;
				}

				bool operator== (Iterator& other) {
					return m_current == other.m_current;
				}

				bool operator!= (Iterator& other) {
					return m_current != other.m_current;
				}

		};

		// ----------------------------------------

		Array() { 
			Init (); 
			}
		
		explicit Array(const size_t nLength) { 
			Init (); 
			Create (nLength);
			}
		
		Array(Array const& other) {
			Init (); 
			Copy (other);
			}
		
		Array (Array&& other) {
			Grab (other);
		}

		explicit Array(DATA_T const * data, size_t dataLength) {
			Init();
			Create(dataLength);
			memcpy(m_info.data, data, sizeof(DATA_T) * dataLength);
		}

		Array(std::initializer_list<DATA_T> data) {
			Init();
			Create(data.size ());
			size_t i = 0;
			for (auto it = data.begin(); it != data.end(); it++)
				m_info.data[i++] = *it;
			//memcpy(m_info.data, data.begin(), sizeof(DATA_T) * data.size());
		}

		~Array() {
			Destroy();
		}

		Iterator begin() {
			return Iterator(*this).First();
		}

		Iterator end() {
			return Iterator(*this).Last();
		}

		// ----------------------------------------

		void Init (void) { 
			m_info.data = nullptr; // reinterpret_cast<DATA_T*> (nullptr);
			m_info.length = 0;
			m_info.pos = 0;
			m_info.mode = 0;
			m_info.wrap = false;
			memset (&m_info.none, 0, sizeof (m_info.none));
			}

		// ----------------------------------------

		void Clear (uint8_t filler = 0, size_t count = 0xffffffff) { 
			if (m_info.data) 
				memset (m_info.data, filler, sizeof (DATA_T) * ((count < m_info.length) ? count : m_info.length)); 
			}

		// ----------------------------------------

		void Fill (DATA_T filler, size_t count = 0xffffffff) {
			if (m_info.data) {
				if (count > m_info.length)
					count = m_info.length;
				for (DATA_T* bufP = Data(); count; count--, bufP++)
					*bufP = filler;
			}
		}

		// ----------------------------------------

		inline bool IsIndex (size_t i) { 
			return (m_info.data != nullptr) && (i < m_info.length); 
		}
		
		// ----------------------------------------

		inline bool IsElement (DATA_T* elem, bool bDiligent = false) {
			if (!m_info.data || (elem < m_info.data) || (elem >= m_info.data + m_info.length))
				return false;	// no data or element out of data
			if (bDiligent) {
				size_t i = static_cast<size_t> (reinterpret_cast<uint8_t*> (elem) - reinterpret_cast<uint8_t*> (m_info.data));
				if (i % sizeof (DATA_T))	
					return false;	// elem in the data, but not properly aligned
			}
			return true;
		}

		// ----------------------------------------

		inline size_t Index (DATA_T* elem) { 
			return size_t (elem - m_info.data); 
		}

		// ----------------------------------------

		inline DATA_T* Pointer (size_t i) { 
			return m_info.data + i; 
		}

		// ----------------------------------------

		void Destroy (void) { 
			if (m_info.data) {
				if (!m_info.mode)
					delete[] m_info.data;
				Init ();
			}
		}
			
		// ----------------------------------------

		DATA_T *Create (size_t length) {
			if (m_info.length != length) {
				Destroy ();
				if ((m_info.data = new DATA_T [length]))
					m_info.length = length;
			}
			return m_info.data;
		}
			
		// ----------------------------------------

		inline DATA_T* Data (size_t i = 0) const { 
			return m_info.data + i; 
		}
		
		// ----------------------------------------

		void SetBuffer (DATA_T *data, size_t mode = 0, size_t length = 0xffffffff) {
			if (m_info.data != data) {
				if (!(m_info.data = data))
					Init ();
				else {
					m_info.length = length;
					m_info.mode = mode;
				}
			}
		}
			
		// ----------------------------------------

		DATA_T* Resize (size_t length, bool bCopy = true) {
			if (m_info.mode == 2)
				return m_info.data;
			if (!m_info.data)
				return Create (length);
			DATA_T* p;
			try {
				p = new DATA_T [length];
			}
			catch(...) {
				return m_info.data;
			}
			if (bCopy) {
				memcpy (p, m_info.data, ((length > m_info.length) ? m_info.length : length) * sizeof (DATA_T)); 
				Clear (); // hack to avoid d'tors
			}
			m_info.length = length;
			m_info.pos %= length;
			delete[] m_info.data;
			return m_info.data = p;
		}

		// ----------------------------------------

		inline const size_t Capacity (void) const { 
			return m_info.length; 
		}

		// ----------------------------------------

		inline DATA_T* Current (void) { 
			return m_info.data ? m_info.data + m_info.pos : nullptr; 
		}

		// ----------------------------------------

		inline size_t Size (void) { 
			return m_info.length * sizeof (DATA_T); 
		}

		// ----------------------------------------

		inline DATA_T& operator[] (const size_t i) {
			return m_info.data [i]; 
		}

		// ----------------------------------------
		/*
		inline DATA_T* operator[] (const DATA_T v) {
			size_t i = BinSearch(v);
			return (i < 0) ? nullptr : &m_info.data[i];
		}
		*/
		// ----------------------------------------

		inline DATA_T* operator* () const { 
			return m_info.data; 
		}

		// ----------------------------------------

		inline Array<DATA_T>& operator= (Array<DATA_T> const & source) {
			return _Copy (source.Data (), source.Capacity ()); 
		}

		// ----------------------------------------

		inline Array<DATA_T>& operator= (Array<DATA_T>&& source) {
			return Grab (source);
		}

		// ----------------------------------------

		inline Array<DATA_T>& operator= (std::initializer_list<DATA_T> data) {
			Init();
			Create(data.size());
			memcpy(m_info.data, data.begin(), sizeof(DATA_T) * data.size());
			return *this;
		}

		// ----------------------------------------

		inline DATA_T& operator= (DATA_T* source) { 
			memcpy (m_info.data, source, m_info.length * sizeof (DATA_T)); 
			return m_info.data [0];
		}

		// ----------------------------------------

		inline Array<DATA_T>& Copy (Array<DATA_T> const & source, size_t offset = 0) { 
			return _Copy(source.Data(), source.Capacity(), offset);
		}

		// ----------------------------------------

		Array<DATA_T>& _Copy(DATA_T const* source, size_t length, size_t offset = 0) {
			if ((m_info.data && (m_info.length >= length + offset)) || Resize(length + offset, false))
				memcpy(m_info.data + offset, source, ((m_info.length - offset < length) ? m_info.length - offset : length) * sizeof(DATA_T));
			return *this;
		}

		// ----------------------------------------

		Array<DATA_T>& Grab (Array<DATA_T>& source) {
			Destroy ();
			m_info.data = source.m_info.data;
			m_info.length = source.m_info.length;
			m_info.pos = source.m_info.pos;
			m_info.mode = source.m_info.mode;
			m_info.wrap = source.m_info.wrap;
			source.m_info.data = nullptr;
			source.Destroy ();
			return *this;
		}

		// ----------------------------------------

		inline DATA_T operator+ (Array<DATA_T>& source) { 
			Array<DATA_T> a (*this);
			a += source;
			return a;
		}

		// ----------------------------------------

		inline DATA_T& operator+= (Array<DATA_T>& source) { 
			size_t offset = m_info.length;
			if (m_info.data) 
				Resize (m_info.length + source.m_info.length);
			return Copy (source, offset);
		}

		// ----------------------------------------

		inline bool operator== (Array<DATA_T>& other) { 
			return (m_info.length == other.m_info.length) && !(m_info.length && memcmp (m_info.data, other.m_info.data)); 
		}

		// ----------------------------------------

		inline bool operator!= (Array<DATA_T>& other) { 
			return (m_info.length != other.m_info.length) || (m_info.length && memcmp (m_info.data, other.m_info.data)); 
		}

		// ----------------------------------------

		inline DATA_T* Start (void) { return m_info.data; }

		// ----------------------------------------

		inline DATA_T* End (void) { return (m_info.data && m_info.length) ? m_info.data + m_info.length - 1 : nullptr; }

		// ----------------------------------------

		inline DATA_T* operator++ (void) { 
			if (!m_info.data)
				return nullptr;
			if (m_info.pos < m_info.length - 1)
				m_info.pos++;
			else if (m_info.wrap) 
				m_info.pos = 0;
			else
				return nullptr;
			return m_info.data + m_info.pos;
		}

		// ----------------------------------------

		inline DATA_T* operator-- (void) { 
			if (!m_info.data)
				return nullptr;
			if (m_info.pos > 0)
				m_info.pos--;
			else if (m_info.wrap)
				m_info.pos = m_info.length - 1;
			else
				return nullptr;
			return m_info.data + m_info.pos;
		}

		// ----------------------------------------

		inline DATA_T* operator+ (size_t i) { 
			return m_info.data ? m_info.data + i : nullptr; 
		}

		// ----------------------------------------

		inline DATA_T* operator- (size_t i) { return m_info.data ? m_info.data - i : nullptr; }

		// ----------------------------------------

		Array<DATA_T>& ShareBuffer (Array<DATA_T>& child) {
			memcpy (&child.m_info, &m_info, sizeof (m_info));
			if (!child.m_info.mode)
				child.m_info.mode = 1;
			return child;
			}

		// ----------------------------------------

		inline bool operator! () { return m_info.data == nullptr; }

		// ----------------------------------------

		inline size_t Pos (void) { return m_info.pos; }

		// ----------------------------------------

		inline void Pos (size_t pos) { m_info.pos = pos % m_info.length; }

		// ----------------------------------------
#if 0
		size_t Read (CFile& cf, size_t nCount = 0, size_t nOffset = 0, bool bCompressed = 0) { 
			if (!m_info.data)
				return -1;
			if (nOffset >= m_info.length)
				return -1;
			if (!nCount)
				nCount = m_info.length - nOffset;
			else if (nCount > m_info.length - nOffset)
				nCount = m_info.length - nOffset;
			return cf.Read (m_info.data + nOffset, sizeof (DATA_T), nCount, bCompressed);
			}

		// ----------------------------------------

		size_t Write (CFile& cf, size_t nCount = 0, size_t nOffset = 0, size_t bCompressed = 0) { 
			if (!m_info.data)
				return -1;
			if (nOffset >= m_info.length)
				return -1;
			if (!nCount)
				nCount = m_info.length - nOffset;
			else if (nCount > m_info.length - nOffset)
				nCount = m_info.length - nOffset;
			return cf.Write (m_info.data + nOffset, sizeof (DATA_T), nCount, bCompressed);
			}
#endif

		// ----------------------------------------

		inline void SetWrap (bool wrap) { m_info.wrap = wrap; }

		// ----------------------------------------

		inline void SortAscending (size_t left = 0, size_t right = 0) { 
			if (m_info.data) 
				QuickSort<DATA_T>::SortAscending (m_info.data, left, (right = 0) ? right : m_info.length - 1); 
				}

		// ----------------------------------------

		inline void SortDescending (size_t left = 0, size_t right = 0) {
			if (m_info.data) 
				QuickSort<DATA_T>::SortDescending (m_info.data, left, (right > 0) ? right : m_info.length - 1);
			}

		// ----------------------------------------

		inline void SortAscending (QuickSort<DATA_T>::tComparator compare, size_t left = 0, size_t right = 0) {
			if (m_info.data) 
				QuickSort<DATA_T>::SortAscending (m_info.data, left, (right > 0) ? right : m_info.length - 1, compare);
			}

		// ----------------------------------------

		inline void SortDescending (QuickSort<DATA_T>::tComparator compare, size_t left = 0, size_t right = 0) {
			if (m_info.data) 
				QuickSort<DATA_T>::SortDescending (m_info.data, left, (right > 0) ? right : m_info.length - 1, compare);
			}

		// ----------------------------------------

		template<typename KEY_T>
		inline size_t Find(KEY_T const& key, int (__cdecl* compare) (DATA_T const&, KEY_T const&), size_t left = 0, size_t right = 0) {
			return m_info.data ? this->BinSearch(m_info.data, key, compare, left, (right > 0) ? right : m_info.length - 1) : -1;
			}
	};

//-----------------------------------------------------------------------------

inline size_t operator- (char* v, Array<char>& a) { return a.Index (v); }
inline size_t operator- (uint8_t* v, Array<uint8_t>& a) { return a.Index (v); }
inline size_t operator- (int16_t* v, Array<int16_t>& a) { return a.Index (v); }
inline size_t operator- (uint16_t* v, Array<uint16_t>& a) { return a.Index (v); }
inline size_t operator- (uint32_t* v, Array<uint32_t>& a) { return a.Index (v); }
inline size_t operator- (size_t* v, Array<size_t>& a) { return a.Index (v); }

//-----------------------------------------------------------------------------

class CharArray : public Array<char> {
	public:
		inline char* operator= (const char* source) { 
			size_t l = size_t (strlen (source) + 1);
			if ((l > this->m_info.length) && !this->Resize (this->m_info.length + l))
				return nullptr;
			memcpy (this->m_info.data, source, l);
			return this->m_info.data;
		}
};

//-----------------------------------------------------------------------------

class ByteArray : public Array<uint8_t> {
public:
	ByteArray(const size_t nLength) {
		Init();
		Create(nLength);
	}
};

class ShortArray : public Array<int16_t> {
public:
	ShortArray(const size_t nLength) {
		Init();
		Create(nLength);
	}
};

class UShortArray : public Array<uint16_t> {
public:
	UShortArray(const size_t nLength) {
		Init();
		Create(nLength);
	}
};

class IntArray : public Array<int32_t> {
public:
	IntArray(const size_t nLength) {
		Init();
		Create(nLength);
	}
};

class UIntArray : public Array<uint32_t> {
public:
	UIntArray(const size_t nLength) {
		Init();
		Create(nLength);
	}
};

class SizeArray : public Array<size_t> {
public:
	SizeArray(const size_t nLength) {
		Init();
		Create(nLength);
	}
};

class FloatArray : public Array<float> {
public:
	FloatArray(const size_t nLength) {
		Init();
		Create(nLength);
	}
};


//-----------------------------------------------------------------------------

template < class DATA_T, size_t length > 
class StaticArray : public Array < DATA_T > {

	class StaticArrayData {
		public:
			DATA_T		data [length];
			};

	protected:
		StaticArrayData	m_info;

	public:
		StaticArray () { Create (length); }

		DATA_T *Create (size_t _length) {
			this->SetBuffer (m_info.data, 2, _length); 
			return m_info.data;
			}
		void Destroy (void) { }
	};

//-----------------------------------------------------------------------------

#endif // ! use std::vector
