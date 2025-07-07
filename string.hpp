// Copyright (c) 2025 Dietfrid Mali
// This software is licensed under the MIT License.
// See the LICENSE file for more details.

#pragma once

#include <utility>

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

#define DEBUG_STRINGS 0
#if DEBUG_STRINGS
#	define LOG(fmt, s)	Log(fmt,s)
#else
#	define LOG(fmt, s)	
#endif

//-----------------------------------------------------------------------------

class String : public SharedArray<char> {
	private:
		int32_t	m_length;
		char	m_empty;
		char	m_char[2];

		void Log(const char* fmt, const char* s) {
			if (++m_count == m_dbgCount)
				m_dbgCount = m_dbgCount;
			char sig[85];
			if (not s)
				strncpy_s(sig, "(null)", 24);
			else {
				strncpy_s(sig, s, 24);
				if (strlen(s) > sizeof(sig) - 5) {
					memcpy(sig, s, 20);
					memcpy(sig + 20, " ...", 5);
				}
			}
			fprintf(stderr, "%6d: ", m_count);
			fprintf(stderr, fmt, sig);
		}

	public:
		static int32_t	m_count;
		static int32_t	m_dbgCount;
	
		//----------------------------------------

		String() 
			: SharedArray<char>(), m_length(0), m_empty('\0'), m_char("")
		{
			Init();
			SetBuffer(&m_empty, 1);
		}

		//----------------------------------------

		String& Copy(const String& other, bool allowStatic = true) {
			if ((this != &other) /* && (*this != other) */) {
				m_length = other.m_length;
				if (not m_length)
					SetBuffer(&m_empty, 1);
				else if (m_length == 1) {
					*m_char = *other.m_char;
					SetBuffer(m_char, 2);
				}
				else
					SharedArray<char>::CopyData(other, allowStatic, 0);
			}
			return *this;
		}

		//----------------------------------------

		String& Move(String& other) {
			if ((this != &other) && (*this != other)) {
				m_length = other.m_length;
				if (not m_length)
					SetBuffer(&m_empty, 1);
				else if (m_length == 1) {
					*m_char = *other.m_char;
					SetBuffer(m_char, 2);
				}
				else {
					LOG("Grabbing string '%s'\n", other.Data());
					Init();
					SharedArray<char>::Move(other);
					other.SetLength(0);
					return *this;
				}
				other.Destroy();
			}
			return *this;
		}

		//----------------------------------------
		// allocate string with buffer size capacity
		// min capacity is length of passed initialization string + 1 (for zero byte)
		// capacity can be larger than that to create a string buffer large enough
		// for subsequent concatenation or pattern replacement operations with that string
		// Will allocate a static buffer unless capacity > length, which means buffer will
		// be allocated bigger in advance for subsequent string operations (e.g. concatenation)

		int ComputeCapacity(const char* source, int capacity) {
			m_length = source ? static_cast<int32_t>(strlen(source)) : 0;
			if (capacity < 0)
				capacity = int(m_length);
			else if (m_length >= capacity)
				m_length = capacity;
			return capacity + 1;
		}


		String (const char* source, int capacity = -1) 
			: SharedArray<char>(), m_length(0), m_empty('\0'), m_char("")
		{
			capacity = ComputeCapacity(source, capacity);
#if 1
			// make a static buffer out of this string constant - much faster and more 
			// memory efficient for handling true string constants in a (variable size) String
			if (m_length == capacity - 1)
				SetBuffer(const_cast<char*>(source), capacity);
			else
#endif
				Copy(const_cast<char*>(source), capacity);
		}


		String(const std::string source, int capacity = -1)
			: SharedArray<char>(), m_length(0), m_empty('\0'), m_char("")
		{
			const char* s = source.c_str();
			Copy(const_cast<char*>(s), ComputeCapacity(s, capacity));
		}

			
		String(char* source, int capacity = -1)
			: SharedArray<char>(), m_length(0), m_empty('\0'), m_char("")
		{
			Copy(source, ComputeCapacity(source, capacity));
		}

		
		String Copy(char* source, int capacity) {
			if (not m_length)
				SetBuffer(&m_empty, 1);
			else if (m_length == 1) {
				m_char[0] = *source;
				SetBuffer(m_char, 2);
			}
			else { // capacity always > 2 here
				Reserve(int32_t(capacity));
				memcpy(Data(), source, m_length);
				*Data(m_length) = '\0';
				LOG("Creating string '%source'\n", Data());
			}
			return *this;
		}

		//----------------------------------------

		String (const String& other)
			: SharedArray<char>(), m_length(0), m_empty('\0'), m_char("")
		{
			LOG("Copy-Creating string '%s'\n", other.Data());
			Copy(other);
		}

		//----------------------------------------

		String (String&& other) noexcept 
			: SharedArray<char>(), m_length(0), m_empty('\0'), m_char("")
		{
			LOG("Move-Creating string '%s'\n", other.Data());
			Move(other);
		}

		//----------------------------------------

		explicit String(const char c)
			: SharedArray<char>(), m_length(0), m_empty('\0'), m_char("")
		{
			Init();
			m_length = 1;
			m_char[0] = c;
			SetBuffer(m_char, 2);
		}

		//----------------------------------------

		explicit String(const int n) 
			: SharedArray<char>(), m_length(0), m_empty('\0'), m_char("")
		{
			Init();
			char s [20];
			snprintf (s, sizeof (s), "%d", n);
			m_length = static_cast<int32_t>(strlen (s));
			Reserve (m_length + 1);
			memcpy (Data (), s, m_length + 1);
		}

		//----------------------------------------

		explicit String (const size_t n) 
			: SharedArray<char>(), m_length(0), m_empty('\0'), m_char("")
		{
			Init ();
			char s [20];
#ifdef _WIN32
			snprintf (s, sizeof (s), "%lld", n);
#else
			snprintf(s, sizeof(s), "%ld", n);
#endif
			m_length = static_cast<int32_t>(strlen (s));
			Reserve (m_length + 1);
			memcpy (Data (), s, m_length + 1);
		}

		//----------------------------------------

		~String() {
			if (Data()) {
				char msg[45];
				strncpy_s(msg, Data(), 40);
				if (Length() > 40)
					strcat(msg, " ...");
				if ((Length() > Capacity() - 1) or (strlen(Data()) > Capacity() - 1))
					LOG("String buffer overrun detected ('%s')!\n", Data());
				else
					LOG("Deleting string '%s'\n", Data());
			}
			m_length = 0;
		}

		//----------------------------------------

		explicit String(const float n) {
			Init();
			*this = n;
		}

		//----------------------------------------

		void Create(int32_t size) {
			Reserve(size);
			*Data() = '\0';
		}

		//----------------------------------------

		String& operator=(const String& other) {
			return Copy(other);
		}

		//----------------------------------------

		String& operator=(String&& other) noexcept {
			return Move(other);
		}

		//----------------------------------------

		inline int32_t Length (void) const { 
			return m_length; 
		}

		//----------------------------------------

		inline void SetLength(const int32_t length) {
			m_length = length;
		}

		//----------------------------------------

		inline bool IsEmpty(void) const {
			return m_length == 0;
		}

		//----------------------------------------

		String& operator= (const char s[]) {
			int32_t l = (int32_t) strlen (s);
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

		String& operator= (const size_t n) {
			char s [100];
			snprintf (s, sizeof (s), "%zd", n);
			return *this = s;
		}

		//----------------------------------------

		String& operator= (const float n) {
			char s[100];
			snprintf(s, sizeof(s), "%f", n);
			return *this = s;
		}

		//----------------------------------------

		operator char*() {
			return Data() ? Data() : &m_empty;
		}

		//----------------------------------------

		operator const char* () const {
			return (const char *) (Data() ? Data() : &m_empty);
		}

		//----------------------------------------

		operator unsigned char* () {
			return (unsigned char*)(Data() ? Data() : &m_empty);
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

		String& operator+= (int32_t n) {
			int32_t s = Length() + n + 1;
			if (s > Capacity())
				Resize(s);
			m_length += n;
			return *this;
		}

		//----------------------------------------

		String& Append(const char* s, int32_t l) {
			if (not l)
				return *this;
			int32_t ol = Length();
			int32_t nl = ol + l;
			*this += l;
			if (nl + 1 > Capacity())
				return *this;	// resizing failed
			memcpy(Data(ol), s, l);
			*Data(nl) = '\0';
			return *this;
		}

		//----------------------------------------

		String& operator+= (const char* s) {
			return Append(s, static_cast<int32_t>(strlen(s)));
		}

		//----------------------------------------

		inline String& operator+= (String const & other) { 
			*this += other.Data (); 
			return *this;
		}

		//----------------------------------------

		String operator+ (const String& other) {
			int32_t l = Length () + other.Length ();
			String s (Data (), int (l));
			memcpy(s.Data() + Length(), other.Data(), other.Length() + 1);
			return s;
			}

		//----------------------------------------

		String operator+ (const char* other) {
			int32_t ol = static_cast<int32_t>(strlen(other));
			int32_t l = Length() + ol;
			String s(Data(), int(l) + 1);
			memcpy(s.Data() + Length(), other, ol + 1);
			s.SetLength(l);
			return s;
		}

		//----------------------------------------

		static inline const char* ToCharPtr(const char* s) {
			return s;
		}

		static inline const char* ToCharPtr(const String& s) {
			return static_cast<const char*>(s.Data());
		}


		template<typename... Args>
		static String Concat(Args&&... args) {
			struct Fragment {
				const char* ptr;
				int32_t len;
			};

			Fragment fragments[] = {
				{ ToCharPtr(args), static_cast<int32_t>(std::strlen(ToCharPtr(args))) }...
			};

			int32_t totalLength = 0;
			for (const auto& f : fragments)
				totalLength += f.len;

			String result;
			result.Resize(totalLength + 1); // +1 für Nullterminator

			char* dest = result.Data();
			for (const auto& f : fragments) {
				std::memcpy(dest, f.ptr, f.len);
				dest += f.len;
			}

			*dest = '\0'; // Nullterminator setzen
			result.SetLength(totalLength); // logische Länge (ohne Terminator)

			return result;
		}

		//----------------------------------------

		static String Concat(std::initializer_list<String> values) {
			int32_t l = 0;
			for (auto const& v : values)
				l += v.Length();
			String result;
			result.Reserve(l + 1);
			char* p = result.Data();
			for (auto const& v : values) {
				l = v.Length();
				std::memcpy(p, v.Data(), l);
				p += l;
			}
			*p = '\0';
			return result;
		}

		//----------------------------------------

		String SubStr (int32_t nOffset, int32_t nLength) const {
			if (nOffset > Length ())
				nLength = 0;
			else if (nLength > Length () + 1 - nOffset)
				nLength = Length () + 1 - nOffset;
			return String (Data () + nOffset, int (nLength));
			}

		//----------------------------------------

		String& Delete (int32_t nOffset, int32_t nLength) {
			if (nOffset > Length ())
				return *this;
			if (nLength > Length () + 1 - nOffset)
				nLength = Length () + 1 - nOffset;
			memcpy (Data () + nOffset, Data () + nOffset + nLength, Length () + 2 - nOffset + nLength);
			m_length -= nLength;
			return *this;
			}

		//----------------------------------------

		inline int Find(const char* pattern) const {
			const char* p = strstr(Data(), pattern);
			return p ? int (p - Data()) : -1;
		}

		//----------------------------------------

		String Replace (const char* oldPattern, const char* newPattern = "", int repetitions = 0) {
			if (not oldPattern)
				return *this;
			int		ol = int (strlen (oldPattern)), 
					nl = int (strlen (newPattern)),
					dl = 0, // delta length
					dpl = nl - ol; // delta pattern lengths
			int		r = repetitions;
			char*	s, * p;
			// find every occurrence of old pattern in string and add up pattern length difference each time 
			// to compute complete size difference between current and resulting string
			for (s = Data(); (p = strstr(s, oldPattern)); s = p + ol) {
				dl += dpl;
				if (r and not --r)
					break;
			}
			if (not dl) // no old patterns?
				return *this;
			// create result string with sufficiently big buffer
			String result = String("", int (Length()) - dl);

			// successively find each old pattern, copy string part between string start or end of previous old pattern
			// to result string, then add new pattern and skip to end of old pattern from current position until the 
			// entire input string has been processed
			r = repetitions;
			for (s = Data(); (p = strstr(s, oldPattern)); ) {
				int l = int (p - s);
				if (l)
					result.Append(s, l);
				result.Append (newPattern, nl);
				s = p + ol;
				if (r and not --r)
					break;
			}
			if (*s)
				result.Append (s, Length() - static_cast<int32_t>((s - Data())));
			*result.Data(result.Length ()) = '\0';
			return result;
		}

		//----------------------------------------

		List<String> Split(char delim) const {
			List<String> subStrings;
			for (char* ps = Data(); *ps; ps++) {
				char* pe{ ps };
				for (; *pe && (*pe != delim); pe++)
					;
				subStrings.Append (String(ps, int (pe - ps)));
				if (not *pe)
					break;
				ps = pe;
			}
			return subStrings;
		}

		//----------------------------------------

		bool IsLowercase(void) {
			for (char* ps = Data(); *ps; ps++)
				if (*ps != char(tolower(*ps)))
					return false;
			return true;
		}

		//----------------------------------------

		bool IsUppercase(void) {
			for (char* ps = Data(); *ps; ps++)
				if (*ps != char(toupper(*ps)))
					return false;
			return true;
		}

		//----------------------------------------

		String ToLowercase(void) {
			String s;
			s.Copy(*this, false);
			for (char* ps = s.Data(); *ps; ps++)
				*ps = char(tolower(*ps));
			return s;
		}

		//----------------------------------------

		String ToUppercase(void) {
			String s;
			s.Copy(*this, false);
			for (char* ps = s.Data(); *ps; ps++)
				*ps = char(toupper(*ps));
			return s;
		}

		//----------------------------------------

		static int Compare(void* context, const String& s1, const String& s2) {
			return (s1.Data() && s2.Data()) ? strcmp(s1.Data(), s2.Data()) : 0;
		}

		//----------------------------------------

	};

//-----------------------------------------------------------------------------
