#ifndef __OY_TARJAN_SCC__
#define __OY_TARJAN_SCC__

#include <algorithm>
#include <cstdint>
#include <numeric>
#include "Graph.h"

namespace OY {
    struct Tarjan_scc {
        struct _RawEdge {
            uint32_t from, to;
        };
        std::vector<_RawEdge> m_rawEdges;
        std::vector<uint32_t> m_adjs;
        std::vector<uint32_t> m_starts;
        std::vector<uint32_t> m_dfn;
        std::vector<uint32_t> m_low;
        std::vector<uint32_t> m_id;
        std::vector<uint32_t> m_stack;
        std::vector<uint32_t> m_topo;
        uint32_t m_vertexNum;
        uint32_t m_dfnCount;
        uint32_t m_idCount;
        Tarjan_scc(uint32_t __vertexNum, uint32_t __edgeNum) : m_starts(__vertexNum + 1, 0), m_vertexNum(__vertexNum), m_dfn(__vertexNum, -1), m_low(__vertexNum, -1), m_id(__vertexNum, -1), m_dfnCount(0), m_idCount(0) { m_rawEdges.reserve(__edgeNum); }
        void addEdge(uint32_t __a, uint32_t __b) { m_rawEdges.push_back({__a, __b}); }
        void build() {
            for (auto &[from, to] : m_rawEdges) m_starts[from + 1]++;
            std::partial_sum(m_starts.begin(), m_starts.end(), m_starts.begin());
            m_adjs.resize(m_starts.back());
            uint32_t cursor[m_vertexNum];
            std::copy(m_starts.begin(), m_starts.begin() + m_vertexNum, cursor);
            for (uint32_t index = 0; index < m_rawEdges.size(); index++) {
                auto &[from, to] = m_rawEdges[index];
                m_adjs[cursor[from]++] = to;
            }
        }
        void calc() {
            build();
            m_stack.reserve(m_vertexNum);
            m_topo.reserve(m_vertexNum);
            for (uint32_t i = 0; i < m_vertexNum; i++)
                if (!~m_dfn[i]) dfs(i);
            for (uint32_t i = 0; i < m_vertexNum; i++) m_id[i] = m_idCount - 1 - m_id[i];
            std::reverse(m_topo.begin(), m_topo.end());
        }
        uint32_t dfs(uint32_t __i) {
            m_dfn[__i] = m_low[__i] = m_dfnCount++;
            m_stack.push_back(__i);
            for (uint32_t cur = m_starts[__i], end = m_starts[__i + 1]; cur < end; cur++)
                if (uint32_t to = m_adjs[cur]; !~m_dfn[to])
                    chmin(m_low[__i], dfs(to));
                else if (!~m_id[to])
                    chmin(m_low[__i], m_dfn[to]);
            if (m_dfn[__i] == m_low[__i]) {
                while (true) {
                    uint32_t back = m_stack.back();
                    m_stack.pop_back();
                    m_id[back] = m_idCount;
                    m_topo.push_back(back);
                    if (back == __i) break;
                }
                m_idCount++;
            }
            return m_low[__i];
        }
        std::vector<std::vector<uint32_t>> groups() const {
            uint32_t count[m_idCount];
            std::fill(count, count + m_idCount, 0);
            std::vector<std::vector<uint32_t>> res(m_idCount);
            for (uint32_t i = 0; i < m_idCount; i++) res[i].reserve(count[i]);
            for (uint32_t i = 0; i < m_id.size(); i++) res[m_id[i]].push_back(i);
            return res;
        }
    };
}

#endif