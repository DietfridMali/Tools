#pragma once

#include "list.hpp"
#include "array.hpp"

// =================================================================================================

template<typename DATA_T>
class DataContainer {
public:
    using DataList = List<DATA_T>;
    using DataTable = Array<DATA_T>;

    DataList    m_dataList;
    DataTable   m_dataTable;

    DataContainer()
        : m_dataList(), m_dataTable()
    {
    }

    inline DataList& GetList(void) {
        return m_dataList;
    }

    inline DataTable& GetTable(void) {
        return m_dataTable;
    }

    inline DATA_T Append(DATA_T data) {
        return *m_dataList.Append(data);
    }

    inline DATA_T& operator[](const int i) {
        return m_dataTable[i];
    }

    inline int Count(void) {
        return static_cast<int>(m_dataList.IsEmpty() ? m_dataTable.Capacity() : m_dataList.Length());
    }

    inline auto begin() { return m_dataTable.begin(); }

    inline auto end() { return m_dataTable.end(); }

    inline auto rbegin() { return m_dataTable.rbegin(); }

    inline auto rend() { return m_dataTable.rend(); }

    void Convert(void) {
        m_dataTable.Reserve(m_dataList.Length());
        int i = 0;
        for (typename DataList::ListNode* pn = m_dataList.First(); pn != m_dataList.GetTail(); pn = pn->Succ())
            m_dataTable[i++] = pn->DataValue();
        m_dataList.Destroy();
    }

    void Destroy(void) {
        m_dataList.Destroy();
        for (int i = 0; i < Count(); i++)
            delete m_dataTable[i];
        m_dataTable.Destroy();
    }

    ~DataContainer(void) {
        Destroy();
    }
};

// =================================================================================================


