#ifndef __OY_GARBOW__
#define __OY_GARBOW__

#include "Graph.h"

namespace OY {
    template <typename _DG>
    struct Garbow {
        _DG &m_graph;
        std::vector<uint32_t> m_dfn;
        uint32_t m_dfnCount;
        std::vector<uint32_t> m_id;
        uint32_t m_idCount;
        std::vector<uint32_t> m_stack1;
        std::vector<uint32_t> m_stack2;
        std::vector<uint32_t> m_topo;
        Garbow(_DG &__graph) : m_graph(__graph), m_dfn(__graph.m_vertexNum, -1), m_id(__graph.m_vertexNum, -1), m_dfnCount(0), m_idCount(0) {
            m_stack1.reserve(__graph.m_vertexNum);
            m_stack2.reserve(__graph.m_vertexNum);
            m_topo.reserve(__graph.m_vertexNum);
            for (uint32_t i = 0; i < m_graph.m_vertexNum; i++)
                if (!~m_dfn[i]) dfs(i);
            for (uint32_t i = 0; i < m_graph.m_vertexNum; i++) m_id[i] = m_idCount - 1 - m_id[i];
            std::reverse(m_topo.begin(), m_topo.end());
        }
        void dfs(uint32_t __i) {
            m_stack1.push_back(__i);
            m_stack2.push_back(__i);
            m_dfn[__i] = m_dfnCount++;
            for (auto [to, value] : m_graph.getEdgesAdjOf(__i)) {
                if (!~m_dfn[to])
                    dfs(to);
                else if (!~m_id[to])
                    while (m_dfn[m_stack2.back()] > m_dfn[to]) m_stack2.pop_back();
            }
            if (m_stack2.back() == __i) {
                m_stack2.pop_back();
                while (true) {
                    uint32_t back = m_stack1.back();
                    m_id[back] = m_idCount;
                    m_topo.push_back(back);
                    m_stack1.pop_back();
                    if (back == __i) break;
                }
                m_idCount++;
            }
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