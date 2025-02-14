// Copyright (c) 2025 Dietfrid Mali
// This software is licensed under the MIT License.
// See the LICENSE file for more details.

#pragma once

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "quicksort.hpp"
#include "array.hpp"
#include "list.hpp"

#ifndef _WIN32
#	define sscanf_s		sscanf
#endif

//-----------------------------------------------------------------------------

class String : public Array < char > {
	private:
		size_t	m_length;

	public:
		explicit String() : m_length (0) {
			Init();
		}

		String (const char* s, int l = -1) {
			m_length = (size_t) ((s == nullptr) ? 0 : (l < 0) ? strlen (s) : l);
			if (l < 0)
				l = -l;
			if (l < int (m_length))
				l = int(m_length);
			Init (); 
			Create (size_t (l) + 1);
			if (l > 0)
				memcpy (Data (), s, l);
			Data() [l] = '\0';
			}

		//----------------------------------------

		String (String const& other) : Array<char> (other) {
			m_length = other.Length ();
		}

		//----------------------------------------

		String (String&& other) noexcept 
			: Array<char> (other) 
		{
			m_length = other.Length ();
			Grab (other);
		}

		//----------------------------------------

		explicit String(const char c) {
			Init();
			m_length = 1;
			Create(2);
			Data()[0] = c;
			Data()[1] = '\0';
		}

		//----------------------------------------

		explicit String(const int n) {
			Init();
			char s [100];
			snprintf (s, sizeof (s), "%d", n);
			m_length = strlen (s);
			Create (m_length + 1);
			memcpy (Data (), s, m_length + 1);
		}

		//----------------------------------------

		explicit String (const size_t n) {
			Init ();
			char s [100];
#ifdef _WIN32
			snprintf (s, sizeof (s), "%lld", n);
#else
			snprintf(s, sizeof(s), "%ld", n);
#endif
			m_length = strlen (s);
			Create (m_length + 1);
			memcpy (Data (), s, m_length + 1);
		}

		//----------------------------------------

		explicit String(const float n) {
			Init();
			*this = n;
		}

		//----------------------------------------

		void Reserve(size_t size) {
			Create(size);
			*Data() = '\0';
		}

		//----------------------------------------

		String& operator=(String const& other) {
			Array<char>::operator=(other);
			m_length = other.Length();
			return *this;
		}

		//----------------------------------------

		String& operator=(String&& other) noexcept {
			m_length = other.Length ();
			Grab (other);
			return *this;
		}

		//----------------------------------------

		inline size_t Length (void) const { 
			return m_length; 
		}

		//----------------------------------------

		inline void SetLength(const size_t length) {
			m_length = length;
		}

		//----------------------------------------

		inline bool Empty(void) const {
			return m_length == 0;
		}

		//----------------------------------------

		String operator= (const char s[]) {
			size_t l = (size_t) strlen (s);
			if (l + 1 > Capacity ()) {
				Resize (l + 1);
				if (l + 1 > Capacity ())
					return *this;
				}
			memcpy (Data (), s, l + 1);
			m_length = l;
			return *this;
		}


		//----------------------------------------

		String operator= (const size_t n) {
			char s [100];
			snprintf (s, sizeof (s), "%zd", n);
			size_t l = strlen (s);
			if (l + 1 > Capacity()) {
				Resize(l + 1);
				if (l + 1 > Capacity())
					return *this;
			}
			memcpy (Data (), s, l + 1);
			return *this;
		}

		//----------------------------------------

		String operator= (const float n) {
			char s[100];
			snprintf(s, sizeof(s), "%f", n);
			return *this = s;
		}

		//----------------------------------------

		explicit operator char*() {
			return Data();
		}

		//----------------------------------------

		explicit operator const char* () {
			return (const char *) Data();
		}

		//----------------------------------------

		explicit operator unsigned char* () {
			return (unsigned char*) Data();
		}

		//----------------------------------------

		explicit operator uint16_t() {
			uint16_t i;
			sscanf_s(Data(), "%hu", &i);
			return i;
		}

		//----------------------------------------

		explicit operator int() {
			int i;
			sscanf_s(Data(), "%d", &i);
			return i;
		}

		//----------------------------------------

		explicit operator size_t() {
			size_t i;
#ifdef _WIN32
			sscanf_s(Data(), "%lld", &i);
#else
			sscanf(Data(), "%ld", &i);
#endif
			return i;
		}

		//----------------------------------------

		explicit operator float() {
			float i;
			sscanf_s(Data(), "%f", &i);
			return i;
		}

		//----------------------------------------

		explicit operator bool() {
			int i;
			sscanf_s(Data(), "%d", &i);
			return bool(i);
		}

		//----------------------------------------

		inline const bool operator== (const String& other) const {
			return (Data () && other.Data ()) ? strcmp (Data (), other.Data ()) == 0 : false;
		}

		//----------------------------------------

		bool operator!= (const String& other) const { 
			return (Data () && other.Data ()) ? strcmp (Data (), other.Data ()) != 0 : true;
		}

		//----------------------------------------

		bool operator< (const String& other) const {
			return (Data() && other.Data()) ? strcmp(Data(), other.Data()) < 0 : false;
		}

		//----------------------------------------

		bool operator> (const String& other) const {
			return (Data() && other.Data()) ? strcmp(Data(), other.Data()) > 0 : false;
		}

		//----------------------------------------

		bool operator<= (const String& other) const {
			return (Data() && other.Data()) ? strcmp(Data(), other.Data()) <= 0 : false;
		}

		//----------------------------------------

		bool operator>= (const String& other) const {
			return (Data() && other.Data()) ? strcmp(Data(), other.Data()) >= 0 : false;
		}

		//----------------------------------------

		inline const bool operator== (const char* other) const {
			return (Data () && other) ? strcmp(Data(), other) == 0 : false;
		}

		//----------------------------------------

		bool operator!= (const char* other) const {
			return (Data () && other) ? strcmp(Data(), other) != 0 : true;
		}

		//----------------------------------------

		String& operator+= (size_t n) {
			size_t s = Length() + n + 1;
			if (s > Capacity())
				Resize(s);
			m_length += n;
			return *this;
		}

		//----------------------------------------

		String& Append(const char* s, size_t l) {
			if (!l)
				return *this;
			size_t ol = Length();
			size_t nl = ol + l;
			*this += l;
			if (nl + 1 > Capacity())
				return *this;	// resizing failed
			memcpy(Data(ol), s, l);
			*Data(nl) = '\0';
			return *this;
		}

		//----------------------------------------

		String& operator+= (const char* s) {
			return Append(s, strlen(s));
		}

		//----------------------------------------

		inline String& operator+= (String const & other) { 
			*this += other.Data (); 
			return *this;
		}

		//----------------------------------------

		String operator+ (const String& other) {
			size_t l = Length () + other.Length ();
			String s (Data (), int (l));
			memcpy(s.Data() + Length(), other.Data(), other.Length() + 1);
			return s;
			}

		//----------------------------------------

		String operator+ (const char* other) {
			size_t ol = strlen(other);
			size_t l = Length() + ol;
			String s(Data(), int(l));
			memcpy(s.Data() + Length(), other, ol + 1);
			return s;
		}

		//----------------------------------------

		String SubStr (size_t nOffset, size_t nLength) {
			if (nOffset > Length ())
				nLength = 0;
			else if (nLength > Length () + 1 - nOffset)
				nLength = Length () + 1 - nOffset;
			return String (Data () + nOffset, int (nLength));
			}

		//----------------------------------------

		String& Delete (size_t nOffset, size_t nLength) {
			if (nOffset > Length ())
				return *this;
			if (nLength > Length () + 1 - nOffset)
				nLength = Length () + 1 - nOffset;
			memcpy (Data () + nOffset, Data () + nOffset + nLength, Length () + 2 - nOffset + nLength);
			m_length -= nLength;
			return *this;
			}

		//----------------------------------------

		inline int Find(const char* pattern) {
			const char* p = strstr(Data(), pattern);
			return p ? int (p - Data()) : -1;
		}

		//----------------------------------------

		String Replace (const char* oldPattern, const char* newPattern = "", int repetitions = 0) {
			if (!oldPattern)
				return *this;
			int		ol = int (strlen (oldPattern)), 
					nl = int (strlen (newPattern)),
					dl = 0, 
					dpl = nl - ol;
			int		r = repetitions;
			char*	s, * p;
			for (s = Data(); (p = strstr(s, oldPattern)); s = p + ol) {
				dl += dpl;
				if (r && !--r)
					break;
			}
			if (!dl)
				return *this;
			String result = String("", -int (Length()) - dl);
			r = repetitions;
			for (s = Data(); (p = strstr(s, oldPattern)); ) {
				int l = int (p - s);
				if (l)
					result.Append(s, l);
				result.Append (newPattern, nl);
				s = p + ol;
				if (r && !--r)
					break;
			}
			if (*s)
				result.Append (s, Length() - (s - Data()));
			*result.Data(result.Length ()) = '\0';
			return result;
		}

		//----------------------------------------

		List<String> Split(char delim) {
			List<String> subStrings;
			for (const char* ps = Data(); *ps; ps++) {
				const char* pe{ ps };
				for (; *pe && (*pe != delim); pe++)
					;
				subStrings.Append (String(ps, int (pe - ps)));
				if (!*pe)
					break;
				ps = pe;
			}
			return subStrings;
		}

		//----------------------------------------

		String Lower(void) {
			String s = *this;
			for (char* ps = s.Data(); *ps; ps++)
				*ps = char (tolower(*ps));
			return s;
		}

		//----------------------------------------

		String Upper(void) {
			String s = *this;
			for (char* ps = s.Data(); *ps; ps++)
				*ps = char (toupper(*ps));
			return s;
		}

		//----------------------------------------

		static int Compare(const String& s1, const String& s2) {
			return strcmp(s1.Data(), s2.Data());
		}

		//----------------------------------------

	};

//-----------------------------------------------------------------------------
