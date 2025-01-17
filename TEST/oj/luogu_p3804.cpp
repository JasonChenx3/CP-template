#include "IO/FastIO.h"
#include "STR/SAM.h"
#include "STR/SuffixArray.h"
#include "STR/SuffixTree.h"

/*
[P3804 【模板】后缀自动机（SAM）](https://www.luogu.com.cn/problem/P3804)
*/
/**
 * 本题为子串问题，可以有多种做法
 * 可以使用后缀数组解决
 * 也可以在后缀自动机的 fail 树上解决
 * 也可以通过建立后缀树解决
 */

void solve_sa() {
    std::string s;
    cin >> s;
    OY::SA::SuffixArray<true, true, 1000000> SA(s);

    // 求出 height 数组后，枚举每个位置作为长度，单调栈拓展左右两侧边界
    uint64_t ans = 0;
    std::vector<uint32_t> left(s.size());
    std::vector<uint32_t> stack;
    stack.reserve(s.size());
    for (uint32_t i = 0; i != s.size(); i++) {
        while (stack.size() && SA.query_height(stack.back()) >= SA.query_height(i)) stack.pop_back();
        left[i] = stack.size() ? stack.back() : -1;
        stack.push_back(i);
    }
    stack.clear();
    for (uint32_t i = s.size() - 1; ~i; i--) {
        while (stack.size() && SA.query_height(stack.back()) >= SA.query_height(i)) stack.pop_back();
        uint64_t right = stack.size() ? stack.back() : s.size();
        ans = std::max(ans, SA.query_height(i) * (right - left[i]));
        stack.push_back(i);
    }
    cout << ans << endl;
}

struct Node {
    uint32_t m_cnt;
};
void solve_SAM() {
    std::string s;
    cin >> s;
    using SAM = OY::StaticSAM_string<Node, 26>;
    SAM sam;
    sam.reserve(s.size());
    for (char c : s) sam.push_back(c - 'a');
    for (uint32_t i = 0; i < s.size(); i++) sam.get_node(sam.query_node_index(i))->m_cnt = 1;
    sam.prepare();

    uint64_t ans = 0;
    sam.do_for_failing_nodes([&](uint32_t a) {
        auto p = sam.get_node(a);
        if (p->m_cnt > 1) ans = std::max(ans, uint64_t(p->m_cnt) * p->m_length);
        if (~sam.query_fail(a)) sam.get_fail_node(a)->m_cnt += p->m_cnt;
    });
    cout << ans << endl;
}

void solve_suftree() {
    std::string s;
    cin >> s;

    using STree = OY::StaticSufTree_string<OY::SUFTREE::BaseNode, 27>;
    STree S(s.size() + 1, [&](uint32_t i) { return i < s.size() ? s[i] - 'a' : 26; });

    uint64_t ans = 0;
    auto dfs = [&](auto self, uint32_t cur, uint32_t dep) -> uint64_t {
        auto p = S.get_node(cur);
        if (p->m_pos + p->m_length > s.size()) return 1;
        dep += p->m_length;
        uint64_t tot = 0;
        for (uint32_t i = 0; i < 26; i++)
            if (S.get_node(cur)->has_child(i)) tot += self(self, S.get_node(cur)->get_child(i), dep);
        if (S.get_node(cur)->has_child(26)) tot++;
        if (tot > 1) ans = std::max(ans, tot * dep);
        return tot;
    };
    dfs(dfs, 0, 0);
    cout << ans << endl;
}

int main() {
    solve_sa();
    // solve_SAM();
    // solve_suftree();
}