/*
最后修改:
20231221
测试环境:
gcc11.2,c++11
clang12.0,C++11
msvc14.2,C++14
*/
#ifndef __OY_SUFFIXTREE__
#define __OY_SUFFIXTREE__

#include <algorithm>
#include <cstdint>
#include <limits>
#include <numeric>
#include <vector>

namespace OY {
    namespace SUFTREE {
        using size_type = uint32_t;
        struct BaseNode {};
        template <typename Node, size_type ChildCount>
        struct StaticNode : Node {
            size_type m_child[ChildCount];
            bool has_child(size_type index) const { return m_child[index]; }
            void set_child(size_type index, size_type child) { m_child[index] = child; }
            size_type get_child(size_type index) const { return m_child[index]; }
            void copy_children(const StaticNode<Node, ChildCount> &rhs) { std::copy_n(rhs.m_child, ChildCount, m_child); }
        };
        template <typename Node, typename Sequence>
        struct Tree {
            static constexpr size_type inf = std::numeric_limits<size_type>::max() / 2;
            struct node : Node {
                size_type m_length, m_pos, m_fail;
            };
            Sequence m_seq;
            std::vector<node> m_data;
            size_type m_now, m_remain;
            size_type _newnode(size_type pos, size_type len) {
                m_data.push_back({});
                m_data.back().m_pos = pos;
                m_data.back().m_length = len;
                return m_data.size() - 1;
            }
            void _init() {
                m_data.resize(1);
                m_now = m_remain = 0;
            }
            Tree() { _init(); }
            template <typename InitMapping>
            Tree(size_type length, InitMapping &&mapping) { resize(length, mapping); }
            template <typename Iterator>
            Tree(Iterator first, Iterator last) { reset(first, last); }
            template <typename InitMapping>
            void resize(size_type length, InitMapping &&mapping) {
                reserve(length);
                for (size_type i = 0; i != length; i++) push_back(mapping(i));
            }
            template <typename Iterator>
            void reset(Iterator first, Iterator last) {
                resize(last - first, [&](size_type i) { return *(first + i); });
            }
            void reserve(size_type length) { m_data.clear(), m_data.reserve(length * 2 + 1), m_seq.clear(), m_seq.reserve(length + 1), _init(); }
            void clear() { m_data.clear(), m_seq.clear(), _init(); }
            size_type size() const { return m_seq.size(); }
            bool empty() const { return m_seq.empty(); }
            template <typename ValueType>
            void push_back(const ValueType &elem) {
                m_seq.push_back(elem);
                m_remain++;
                size_type n = m_seq.size(), lst = 0;
                do {
                    while (true) {
                        if (!m_data[m_now].has_child(m_seq[n - m_remain])) break;
                        size_type child = m_data[m_now].get_child(m_seq[n - m_remain]);
                        if (m_remain <= m_data[child].m_length) break;
                        m_remain -= m_data[child].m_length, m_now = child;
                    }
                    auto &&c = m_seq[n - m_remain];
                    if (!m_data[m_now].has_child(c)) {
                        m_data[lst].m_fail = m_now;
                        size_type child = _newnode(n - 1, inf);
                        m_data[lst = m_now].set_child(c, child);
                    } else {
                        size_type v = m_data[m_now].get_child(c);
                        auto &&e = m_seq[m_data[v].m_pos + m_remain - 1];
                        if (elem == e) {
                            m_data[lst].m_fail = m_now;
                            break;
                        } else {
                            size_type u = _newnode(m_data[v].m_pos, m_remain - 1), u2 = _newnode(n - 1, inf);
                            m_data[u].set_child(e, v);
                            m_data[u].set_child(elem, u2);
                            m_data[v].m_pos += m_remain - 1;
                            m_data[v].m_length -= m_remain - 1;
                            m_data[m_now].set_child(c, u);
                            m_data[lst].m_fail = u;
                            lst = u;
                        }
                    }
                    if (!m_now)
                        m_remain--;
                    else
                        m_now = m_data[m_now].m_fail;
                } while (m_remain);
            }
            const node *get_node(size_type node_index) const { return &m_data[node_index]; }
            node *get_node(size_type node_index) { return &m_data[node_index]; }
        };
    }
    template <typename Node = SUFTREE::BaseNode, SUFTREE::size_type ChildCount = 27>
    using StaticSufTree_string = SUFTREE::Tree<SUFTREE::StaticNode<Node, ChildCount>, std::string>;
}

#endif