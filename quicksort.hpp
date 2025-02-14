// Copyright (c) 2025 Dietfrid Mali
// This software is licensed under the MIT License.
// See the LICENSE file for more details.

#pragma once

#ifndef _WIN32
#	define __cdecl
#endif

//-----------------------------------------------------------------------------

template < typename DATA_T > 
class QuickSort {
	public:
		typedef int (__cdecl *tComparator) (const DATA_T*, const DATA_T*);

		template<typename KEY_T>
		using tSearchComparator = int(__cdecl*)(const DATA_T&, const KEY_T&);

		/*
		void SortAscending (DATA_T* data, size_t left, size_t right);
		void SortDescending (DATA_T* data, size_t left, size_t right);
		void SortAscending (DATA_T* data, size_t left, size_t right, comparator compare);
		void SortDescending (DATA_T* data, size_t left, size_t right, comparator compare);
		inline void Swap (DATA_T* left, DATA_T* right);
		size_t BinSearch (DATA_T* data, size_t l, size_t r, DATA_T key);
*/
//-----------------------------------------------------------------------------

inline void Swap(DATA_T* left, DATA_T* right)
{
	DATA_T h;
	memcpy (&h, left, sizeof (DATA_T));
	memcpy(left, right, sizeof(DATA_T));
	memcpy(right, &h, sizeof(DATA_T));
	memset(&h, 0, sizeof(DATA_T));
}

//-----------------------------------------------------------------------------

void SortAscending(DATA_T* data, size_t left, size_t right)
{
	size_t	l = left,
			r = right;
	DATA_T* m = data + (l + r) / 2;

	do {
		while (data[l] < *m)
			l++;
		while (data[r] > *m)
			r--;
		if (l <= r) {
			if (l < r)
				Swap(data + l, data + r);
			l++;
			r--;
		}
	} while (l <= r);
	if (l < right)
		SortAscending(data, l, right);
	if (left < r)
		SortAscending(data, left, r);
}

//-----------------------------------------------------------------------------

void SortDescending(DATA_T* data, size_t left, size_t right)
{
	size_t	l = left,
			r = right;
	DATA_T* m = data + (l + r) / 2;

	do {
		while (data[l] > *m)
			l++;
		while (data[r] < *m)
			r--;
		if (l <= r) {
			if (l < r)
				Swap(data + l, data + r);
			l++;
			r--;
		}
	} while (l <= r);
	if (l < right)
		SortDescending(data, l, right);
	if (left < r)
		SortDescending(data, left, r);
}

//-----------------------------------------------------------------------------

void SortAscending(DATA_T* data, size_t left, size_t right, tComparator compare)
{
	size_t	l = left,
			r = right;
	DATA_T* m = data + (l + r) / 2;

	do {
		while (compare(data + l, m) < 0)
			l++;
		while (compare(data + r, m) > 0)
			r--;
		if (l <= r) {
			if (l < r)
				Swap(data + l, data + r);
			l++;
			r--;
		}
	} while (l <= r);
	if (l < right)
		SortAscending(data, l, right, compare);
	if (left < r)
		SortAscending(data, left, r, compare);
}

//-----------------------------------------------------------------------------

void SortDescending(DATA_T* data, size_t left, size_t right, tComparator compare)
{
	size_t	l = left,
			r = right;
	DATA_T	m;
	
	memcpy (&m, data + (l + r) / 2, sizeof (DATA_T));

	do {
		while (compare(data + l, &m) > 0)
			l++;
		while (compare(data + r, &m) < 0)
			r--;
		if (l <= r) {
			if (l < r)
				Swap(data + l, data + r);
			l++;
			r--;
		}
	} while (l <= r);
	if (l < right)
		SortDescending(data, l, right, compare);
	if (left < r)
		SortDescending(data, left, r, compare);
}

// ----------------------------------------------------------------------------
/*
template<typename KEY_T>
size_t BinSearch(const DATA_T* data, size_t l, size_t r, typename KEY_T::KeyType key) 
{
	size_t	m;

	do {
		m = (l + r) / 2;
		if (data[m] > key)
			r = m - 1;
		else if (data[m] < key)
			l = m + 1;
		else {
			// find first record with equal key
			while ((m > 0) && (data[m - 1] > key))
				--m;
			return m;
		}
	} while (l <= r);
	return -1;
}
*/
// ----------------------------------------------------------------------------

template<typename KEY_T>
size_t BinSearch(DATA_T* const data, KEY_T const& key, int(__cdecl* compare)(DATA_T const&, KEY_T const&), size_t l, size_t r)
{
	size_t	m;
	int		i;

	do {
		m = (l + r) / 2;
		i = compare(data [m], key);
		if (i > 0)
			r = m - 1;
		else if (i < 0)
			l = m + 1;
		else {
			// find first record with equal key
			while ((m > 0) && (compare (data [m - 1], key) > 0))
				--m;
			return m;
		}
	} while (l <= r);
	return -1;
}

//-----------------------------------------------------------------------------

};
