#pragma once

#include "stack.hpp"
#include "list.hpp"

#define USE_SEGMENTED_LISTS 0

#if !USE_SEGMENTED_LISTS

#	define SegmentedList	List

#else

template <typename DATA_T>
class SegmentedList {
private:
	typedef Stack<DATA_T>		tDataBuffer;
	typedef List<tDataBuffer*>	tSegmentList;

	tSegmentList	m_data;

	//----------------------------------------

	class ListDataPtr {
	public:
		tSegmentList*				m_data;
		typename tSegmentList::ListNode*		m_nodePtr;
		size_t						m_segmentSize;
		size_t						m_index;

		explicit ListDataPtr() : m_data(nullptr), m_nodePtr(nullptr), m_index(0) {}
#if 0
		explicit ListDataPtr(tSegmentList* data, size_t segmentSize, size_t index = 0) 
			: m_data(data), m_segmentSize(segmentSize), m_index(index)
		{
			if (data && (segmentSize > 0)) {
				m_nodePtr = data->NodePtrAt(index / segmentSize);
			}
		}
#endif
		explicit ListDataPtr(const tSegmentList* data, const size_t segmentSize, size_t index = 0)
			: m_data(const_cast<tSegmentList*>(data)), m_segmentSize(segmentSize), m_index(index), m_nodePtr(nullptr)
		{
			if (data && (segmentSize > 0)) {
				m_nodePtr = m_data->NodePtrAt(int (index / segmentSize));
			}
		}

		ListDataPtr& operator= (ListDataPtr other) {
			m_nodePtr = m_nodePtr;
			return *this;
		}

		ListDataPtr& operator= (typename tSegmentList::ListNode* p) {
			m_nodePtr = p;
			return *this;
		}
#if 0
		bool operator== (const ListDataPtr* other) {
			return (m_nodePtr == other->m_nodePtr) && (m_index == other->m_index);
		}
#endif
		bool operator!= (const ListDataPtr* other) {
			return (m_nodePtr != other->m_nodePtr) or (m_index != other->m_index);
		}

		bool operator== (const ListDataPtr& other) {
			return (m_nodePtr == other.m_nodePtr) && (m_index == other.m_index);
		}

		bool operator!= (const ListDataPtr& other) {
			return (m_nodePtr != other.m_nodePtr) or (m_index != other.m_index);
		}

		void operator++ (int) {
			if (m_index < m_nodePtr->GetDataValue()->ToS())
				m_index++;
			else {
				m_nodePtr = m_nodePtr->m_succ;
				m_index = 0;
			}
		}

		void operator-- (int) {
			if (m_index == 0) {
				m_nodePtr = m_nodePtr->pred;
				m_index = m_nodePtr->GetDataValue().ToS();
			}
			m_index--;
		}


		ListDataPtr ListDataPtrAt(int i) {
			i += m_index;
			return ListDataPtr(&m_data, m_segmentSize, i);
		}


		DATA_T* GetDataRef(void) {
			return m_nodePtr->GetDataValue()->GetRef(m_index);
		}


		ListDataPtr operator+ (int n) {
			return ListDataPtrAt(n);
		}

		ListDataPtr operator- (int n) {
			return ListDataPtrAt(-n);
		}

		typename tSegmentList::ListNode* operator->() const {
			return m_nodePtr;
		}
#if 1
		bool operator== (const typename tSegmentList::ListNode* nodePtr) {
			return m_nodePtr == nodePtr;
		}

		bool operator!= (const typename tSegmentList::ListNode* nodePtr) {
			return m_nodePtr != nodePtr;
		}
#endif
		operator bool() {
			return (m_nodePtr != nullptr);
		}

		operator typename tSegmentList::ListNode* const() {
			return m_nodePtr;
		}
	};

	//----------------------------------------

	class Iterator {
		using ListDataPtr = typename SegmentedList<DATA_T>::ListDataPtr;

	private:
		ListDataPtr 	m_first;
		ListDataPtr 	m_last;
		ListDataPtr 	m_current;
		size_t			m_index;

	public:
		explicit Iterator() {}

		Iterator(SegmentedList<DATA_T>& l)
			//: m_first(l.First()), m_last(l.Last()), m_index(0)
		{
			m_first = l.First();
			m_last = l.Last();
			m_index = 0;
			m_current = m_first;
		}

		Iterator(const SegmentedList<DATA_T>& l)
			: m_first(l.First()), m_last(l.Last()), m_index(0)
		{
			m_current = m_first;
		}

		operator bool() const { return m_current != nullptr; }
#if 0
		DATA_T* operator*() {
			return m_current;
		}

		const DATA_T* operator*() const {
			return m_current;
		}
#endif
		//DATA_T* operator*() const { return &m_current->m_data; }
		// return the current value and the index of the value in the list
		std::pair<size_t, DATA_T&> operator*() {
			return std::pair<size_t, DATA_T&> {m_index, *m_current.GetDataRef() };
		}

		constexpr std::pair<size_t, DATA_T&> operator*() const {
			return std::pair<size_t, DATA_T&> {m_index, *m_current.GetDataRef() };
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

		constexpr Iterator& Last(void) {
			m_current = m_last;
			return *this;
		}
	};

	// ----------------------------------------
	// This list implementation uses two dummy entries as head and tail elements,
	// as this makes many operations on the list much easier.
	// Since the list can always store a DATA_T*, head and tail will be initialized 
	// with a DATA_T*, which is quite memory efficient.

private:
	size_t		m_segmentSize;
	size_t		m_length;
	bool		m_result;
	DATA_T		m_none;

public:
	void Destroy(void) {
		m_data.Destroy();
		Init();
	}

	SegmentedList& Copy(const SegmentedList& other) {
		m_data = other.m_data;
		return *this;
	}

	inline void Init(void) {
		m_data.Init();
		m_length = 0;
		memset(&m_none, 0, sizeof(m_none));
	}

	ListDataPtr ListDataPtrAt(size_t i) {
		return ListDataPtr(&m_data, m_segmentSize, i);
	}

	typename tSegmentList::ListNode* NodePtrAt(size_t i) {
		return ListDataPtrAt(i).m_nodePtr.GetDataValue().GetRef();
	}

	DATA_T* NodeDataAt(size_t i) {
		return *(m_data->NodePtrAt(i / m_segmentSize)) [i % m_segmentSize];
	}

	Iterator begin() const {
		return Iterator(*this).First();
	}

	Iterator end() const {
		return Iterator(*this).Last();
	}

	inline ListDataPtr First(void) const {
		return ListDataPtr (&m_data, m_segmentSize, 0 );
		//return p;
	}

	inline ListDataPtr Last(void) const {
		return ListDataPtr (&m_data, m_segmentSize, size_t(m_length - 1) );
		//return p;
		//return ListDataPtr(&m_data, m_segmentSize, 0); // size_t(m_length - 1));
	}

	inline size_t Length(void) {
		return m_length;
	}

	inline const bool IsEmpty(void) const {
		return m_length == 0;
	}

	inline DATA_T& operator[] (int i) {
		DATA_T* data = ListDataPtr(&m_data, m_segmentSize, i).GetDataRef();
		if (data) {
			m_result = true;
			return *data;
		}
		m_result = false;
		return m_none;
	}

	inline SegmentedList<DATA_T>& operator= (SegmentedList<DATA_T> const& other) {
		Copy(other);
		return *this;
	}

	inline SegmentedList<DATA_T>& operator= (SegmentedList&& other) noexcept {
		return Move(other);
	}

	inline SegmentedList<DATA_T>& operator= (std::initializer_list<DATA_T> data) {
		Destroy();
		Init();
		for (auto& d : data)
			Append(d);
		return *this;
	}

	SegmentedList<DATA_T>(size_t segmentSize = 1) 
		: m_segmentSize(segmentSize), m_length(0), m_result(true)
	{
		Init();
	}

	SegmentedList<DATA_T>(SegmentedList<DATA_T> const& other) :
		m_length(0), m_result(true) 
	{
		Copy(other);
	}

	SegmentedList<DATA_T>(SegmentedList<DATA_T>&& other) noexcept :
		m_length(0), m_result(true) 
	{
		Init();
		Move(other);
	}

	explicit SegmentedList<DATA_T>(DATA_T& data) :
		m_length(0), m_result(true) {
		Append(data);
	}

	explicit SegmentedList<DATA_T>(Array<DATA_T>& data) :
		m_length(0), m_result(true) {
		Init();
		for (auto const& v : data)
			Append(*v);
	}

	SegmentedList<DATA_T>(std::initializer_list<DATA_T> data), size_t segmentSize = 1) :
		m_length(0), m_result(true) {
		Init();
		for (auto const& d : data)
			Append(d);
	}

	~SegmentedList() {
		Destroy();
	}

	//-----------------------------------------------------------------------------

public:
	DATA_T* Add(int i, const DATA_T* data = nullptr) {
		if (i < 0)
			i = int (m_length - i + 1);
		if (i < m_length)
			Add(int (m_length), &(*this)[i]); // move element at index i to end of list to free up its position
		else {
			int first = int (m_length / m_segmentSize);
			int last = int ((m_length + i) / m_segmentSize);
			for (int i = first; i < last; i++)
				m_data.Insert(-1);
		}
		DATA_T* dataPtr = ListDataPtr(&m_data, m_segmentSize, i).GetDataRef();
		if (data)
			*dataPtr = *data;
		else {
			DATA_T d{};
			*dataPtr = DATA_T{};
		}
		m_length++;
		return dataPtr;
	}

	//-----------------------------------------------------------------------------

public:
	bool Insert(int i, DATA_T* data) {
		return Add(i, data) != nullptr;
	}


	bool Insert(int i, DATA_T& data) {
		return Add(i, &data) != nullptr;
	}


	bool Insert(int i, const DATA_T& data) {
		return Add(i, &data) != nullptr;
	}


	bool Insert(int i, DATA_T&& data) {
		return Add(i, new DATA_T(std::move(data), true)) != nullptr;
	}

	//-----------------------------------------------------------------------------

public:
	DATA_T Extract(int i) {
		m_result = false;
		if (not m_length)
			return m_none;

		typename tSegmentList::ListNode* node = m_data.NodePtrAt(i / m_segmentSize);
		if (not node)
			return m_none;
		DATA_T* data = node->GetDataValue().GetRef(i % m_segmentSize);
		Discard(node);
		m_result = true;
		return *data;
	}

	//-----------------------------------------------------------------------------

public:
	bool Discard(int i) {
		m_result = false;
		if (not m_length)
			return false;
		return Discard(NodePtrAt(i));
	}


	bool Discard(typename tSegmentList::ListNode* node) {
		if (not node)
			return m_result = false;
		*node = *NodePtrAt(m_length - 1); // overwrite with last entry in list - fastest way to remove one list entry and keep the list without gaps
		tDataBuffer& b = m_data[m_length / m_segmentSize]; // if the last list segment is empty now, discard it
		if (not b.Shrink())
			m_data.Discard(m_length - 1);
		m_length--;
		return m_result = true;
	}

	//-----------------------------------------------------------------------------
	// copy the other list to the end of this list
	// will leave other list intact

public:
	SegmentedList<DATA_T>& operator+= (SegmentedList<DATA_T>& other) {
		if (IsEmpty())
			return *this;
		for (auto& d : other)
			Insert(-1, d);
		return *this;
	}

	//-----------------------------------------------------------------------------

	SegmentedList<DATA_T>& operator+= (List<DATA_T>& other) {
		if (IsEmpty())
			return *this;
		for (auto& d : other)
			Insert(-1, d);
		return *this;
	}

	//-----------------------------------------------------------------------------
	// move the other list to this list. current list content will be removed first.
	// will leave other list empty

public:
	SegmentedList<DATA_T>& Move(SegmentedList<DATA_T>& other) {
		Destroy();
		m_data.Move(other.m_data);
		m_length = other.m_length;
		Init();
		return *this;
	}

	//-----------------------------------------------------------------------------

public:
	SegmentedList<DATA_T> operator+ (const SegmentedList<DATA_T>& other) {
		SegmentedList l;
		if (IsEmpty())
			return l = other;
		if (IsEmpty())
			return l = *this;
		l = *this;
		l += other;
		return l;
	}

	//-----------------------------------------------------------------------------

public:
	template<typename T>
	int Find(T&& data) {
		DATA_T pattern = std::forward<T>(data);
#if 1
		for (const auto p : *this)
			if (p == pattern)
				return int(i);
#else
		int i = 0;
		for (ListDataPtr p(m_headPtr); p != m_tailPtr; p++, i++)
			if (*p->GetDataValue() == pattern)
				return int(i);
#endif
		return -1;
	}

	//-----------------------------------------------------------------------------

public:
	SegmentedList<DATA_T> Splice(size_t from, size_t to = 0) {
		ListDataPtr start = ListDataPtrAt(int(from));
		ListDataPtr end = ListDataPtrAt(int(to ? to : m_length - 1));
		SegmentedList l;
		if ((start != nullptr) && (end != nullptr)) {
			for (ListDataPtr current = start; current != end; current++) {
				l.Append(new typename tSegmentList::ListNode(*current.GetDataRef()));
			}
		}
		return l;
	}

	//-----------------------------------------------------------------------------

public:
#if 1
	template<typename T>
	inline bool Append(T&& data) {
		return Insert(-1, std::forward<T>(data));
	}
#else
	inline bool Append(DATA_T* data) {
		return Insert(-1, data);
	}

	inline bool Append(DATA_T& data) {
		return Insert(-1, data);
	}

	inline bool Append(DATA_T&& data) {
		return Insert(-1, data);
	}
#endif

	//-----------------------------------------------------------------------------

public:
	template<typename T>
	inline bool Remove(T&& data) {
		int i = Find(std::forward<T>(data));
		if (i < 0)
			return (m_result = false);
		Extract(size_t(i));
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
