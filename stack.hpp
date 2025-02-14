// Copyright (c) 2025 Dietfrid Mali
// This software is licensed under the MIT License.
// See the LICENSE file for more details.

#pragma once

#include "array.hpp"

//-----------------------------------------------------------------------------

template < class DATA_T > 
class Stack : public Array< DATA_T > {
	protected:
		size_t	m_tos;
		size_t	m_growth;

	public:
		Stack () { Init (); }

		Stack (size_t nLength) { 
			Init (); 
			Create (nLength);
			}

		~Stack() { Destroy (); }

		inline void Reset (void) { m_tos = 0; }

		inline void Init (void) { 
			m_growth = 0;
			Reset ();
			Array<DATA_T>::Init ();
			}

		inline bool Grow (const size_t i = 1) {
			if ((m_tos + i > this->m_info.length) && (!(m_growth && this->Resize (this->m_info.length + m_growth)))) {
				return false;
				}
//#pragma omp critical
			m_tos += i;
			return true;
			}

		inline bool Push (const DATA_T elem) { 
			if (!Grow ())
				return false;
//#pragma omp critical
			this->m_info.data [m_tos - 1] = elem;
			return true;
			}
	
		inline void Shrink (size_t i = 1) {
//#pragma omp critical
			if (i >= m_tos)
				m_tos = 0;
			else
				m_tos -= i;
			}

		inline DATA_T& Pop (void) {
//#pragma omp critical
			Shrink ();
			return this->m_info.data [m_tos];
			}

		inline void Truncate (size_t i = 1) {
			if (i < m_tos)
				m_tos = i;
			}

		inline size_t Find (DATA_T& elem) {
			for (size_t i = 0; i < m_tos; i++)
				if (this->m_info.data [i] == elem)
					return i;
			return m_tos;
			}

		inline size_t ToS (void) { return m_tos; }

		inline DATA_T* Top (void) { return (this->m_info.data && m_tos) ? this->m_info.data + m_tos - 1 : NULL; }

		inline bool Delete (size_t i) {
			if (i >= m_tos) {
				return false;
				}
//#pragma omp critical
			if (i < --m_tos)
				memcpy (this->m_info.data + i, this->m_info.data + i + 1, sizeof (DATA_T) * (m_tos - i));
			return true;
			}

		inline bool DeleteElement (DATA_T& elem) { return Delete (Find (elem));	}

		inline DATA_T& Pull (DATA_T& elem, size_t i) {
//#pragma omp critical
			if (i < m_tos) {
				elem = this->m_info.data [i];
				Delete (i);
				}
			return elem;
			}

		inline DATA_T Pull (size_t i) {
			DATA_T	v;
			return Pull (v, i);
			}

		inline void Destroy (void) { 
			Array<DATA_T>::Destroy ();
			m_tos = 0;
			}

		inline DATA_T *Create (size_t length, size_t growth = 0) {
			Destroy ();
			m_growth = (growth > 0) ? growth : length;
			return Array<DATA_T>::Create (length);
			}

		inline size_t Growth (void) { return m_growth; }

		inline void SetGrowth (size_t growth) { m_growth = growth; }

		inline void SortAscending (size_t left = 0, size_t right = 0) { 
			if (this->m_info.data)
				QuickSort<DATA_T>::SortAscending (this->m_info.data, left, (right >= 0) ? right : m_tos - 1); 
				}

		inline void SortDescending (size_t left = 0, size_t right = 0) {
			if (this->m_info.data)
				QuickSort<DATA_T>::SortDescending (this->m_info.data, left, (right >= 0) ? right : m_tos - 1);
			}

		inline void SortAscending (QuickSort<DATA_T>::tComparator compare, size_t left = 0, size_t right = 0) {
			if (this->m_info.data)
				QuickSort<DATA_T>::SortAscending (this->m_info.data, left, (right >= 0) ? right : m_tos - 1, compare);
			}

		inline void SortDescending (QuickSort<DATA_T>::tComparator compare, size_t left = 0, size_t right = 0) {
			if (this->m_info.data)
				QuickSort<DATA_T>::SortDescending (this->m_info.data, left, (right >= 0) ? right : m_tos - 1, compare);
			}

		inline size_t BinSearch (DATA_T key, size_t left = 0, size_t right = 0) {
			return this->m_info.data ? QuickSort<DATA_T>::BinSearch (this->m_info.data, left, (right >= 0) ? right : m_tos - 1, key) : -1;
			}

	};

//-----------------------------------------------------------------------------
