#include "IO/FastIO.h"
#include "STR/SAM.h"

#include <map>

struct Node {
    uint32_t m_cnt;
};
void test_substr_cnt() {
    using SAM = OY::StaticSAM_string<Node, 26>;
    std::string s = "abbababcababbcca";
    SAM sam;
    for (char c : s) {
        sam.push_back(c - 'a');
        sam.get_node(sam.query_node_index(sam.size() - 1))->m_cnt = 1;
    }
    sam.prepare();

    // 在 fail 树上求树上前缀和
    sam.do_for_failing_nodes([&](uint32_t a) {
        uint32_t p = sam.query_fail(a);
        if (~p) sam.get_node(p)->m_cnt += sam.get_node(a)->m_cnt;
    });

    // 查询某个子串出现次数
    auto query_cnt = [&](std::string &&s) {
        uint32_t a = 0;
        for (char c : s) a = sam.get_node(a)->get_child(c - 'a');
        return sam.get_node(a)->m_cnt;
    };

    cout << "ab appeared " << query_cnt("ab") << " times\n";
    cout << "abb appeared " << query_cnt("abb") << " times\n";
    cout << "ca appeared " << query_cnt("ca") << " times\n";
    cout << endl;
}

struct MapNode {
    std::map<uint32_t, uint32_t> m_child;
    uint32_t m_cnt;
    bool has_child(uint32_t index) const { return m_child.find(index) != m_child.end(); }
    void set_child(uint32_t index, uint32_t child) { m_child[index] = child; }
    uint32_t get_child(uint32_t index) const { return m_child.find(index)->second; }
    void copy_children(const MapNode &rhs) { m_child = rhs.m_child; }
};
void test_map_node() {
    using SAM = OY::SAM::Automaton<MapNode>;
    std::vector<int> s{9999, 1437, 9999, 1437, 1437, 9999, 1437, 9999};
    SAM sam;
    for (int c : s) {
        sam.push_back(c);
        sam.get_node(sam.query_node_index(sam.size() - 1))->m_cnt = 1;
    }
    sam.prepare();

    // 在 fail 树上求树上前缀和
    sam.do_for_failing_nodes([&](uint32_t a) {
        uint32_t p = sam.query_fail(a);
        if (~p) sam.get_node(p)->m_cnt += sam.get_node(a)->m_cnt;
    });

    // 查询某个子串出现次数
    auto query_cnt = [&](std::vector<int> &&s) {
        uint32_t a = 0;
        for (int c : s) a = sam.get_node(a)->get_child(c);
        return sam.get_node(a)->m_cnt;
    };

    cout << "[1437, 9999] appeared " << query_cnt({1437, 9999}) << " times\n";
    cout << "[9999] appeared " << query_cnt({9999}) << " times\n";
    cout << endl;
}

int main() {
    test_substr_cnt();
    test_map_node();
}
/*
#输出如下
ab appeared 5 times
abb appeared 2 times
ca appeared 2 times

[1437, 9999] appeared 3 times
[9999] appeared 4 times

*/