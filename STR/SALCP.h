/*
最后修改:
20231212
测试环境:
gcc11.2,c++11
clang12.0,C++11
msvc14.2,C++14
*/
#ifndef __OY_SALCP__
#define __OY_SALCP__

#include "../DS/MaskRMQ.h"
#include "SuffixArray.h"

namespace OY {
    namespace SALCP {
        using size_type = uint32_t;
        template <typename Sequence, size_type MAX_LEN, size_type MAX_NODE>
        struct LCP {
            size_type m_length;
            SA::SuffixArray<Sequence, MAX_LEN> m_table;
            MaskRMQMinValueTable<size_type, uint64_t, MAX_NODE> m_inner_table;
            template <typename Iterator>
            LCP(Iterator first, Iterator last) : m_length(last - first), m_table(first, last) {
                m_table.get_rank(), m_table.get_height();
                m_inner_table.resize(m_length, [&](size_type i) { return m_table.query_height(i); });
            }
            LCP(const std::vector<int> &seq) : LCP(seq.begin(), seq.end()) {}
            LCP(const std::string &seq) : LCP(seq.begin(), seq.end()) {}
            size_type lcp(size_type a, size_type b, size_type limit) const {
                if (a == b) return std::min(limit, m_length - a);
                size_type ra = m_table.query_rank(a), rb = m_table.query_rank(b);
                if (ra > rb) std::swap(ra, rb);
                return std::min<size_type>(limit, m_inner_table.query(ra + 1, rb));
            }
            size_type lcp(size_type a, size_type b) const {
                if (a == b) return m_length - a;
                return lcp(a, b, m_length - std::max(a, b));
            }
            int compare(size_type l1, size_type r1, size_type l2, size_type r2) const {
                if (l1 == l2) return r1 < r2 ? -1 : (r1 == r2 ? 0 : 1);
                size_type len1 = r1 - l1 + 1, len2 = r2 - l2 + 1, ra = m_table.query_rank(l1), rb = m_table.query_rank(l2);
                if (ra < rb)
                    if (len1 < len2 || m_inner_table.query(ra + 1, rb) < len2)
                        return -1;
                    else
                        return len1 > len2;
                else if (len2 < len1 || m_inner_table.query(rb + 1, ra) < len1)
                    return 1;
                else
                    return len1 < len2 ? -1 : 0;
            }
        };
    }
    template <SALCP::size_type MAX_LEN, SALCP::size_type MAX_NODE, typename TableType = SALCP::LCP<std::vector<int>, MAX_LEN, MAX_NODE>>
    auto make_SA_LCP(const std::vector<int> &seq) -> TableType { return TableType(seq.begin(), seq.end()); }
    template <SALCP::size_type MAX_LEN, SALCP::size_type MAX_NODE, typename TableType = SALCP::LCP<std::string, MAX_LEN, MAX_NODE>>
    auto make_SA_LCP(const std::string &seq) -> TableType { return TableType(seq.begin(), seq.end()); }
    template <SALCP::size_type MAX_LEN, SALCP::size_type MAX_NODE, typename ValueType, typename TableType = SALCP::LCP<std::vector<ValueType>, MAX_LEN, MAX_NODE>>
    auto make_SA_LCP(ValueType *first, ValueType *last) -> TableType { return TableType(first, last); }
}

#endif