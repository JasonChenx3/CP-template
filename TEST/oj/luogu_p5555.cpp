#include "IO/FastIO.h"
#include "STR/PAM.h"
#include "STR/RollbackPAM.h"

/*
[P5555 秩序魔咒](https://www.luogu.com.cn/problem/P5555)
*/
/**
 * 本题为回文串问题
 * 可以使用回文自动机模板解决
 */

struct Node {
    uint32_t m_mask;
};
void solve_PAM() {
    uint32_t n1, n2;
    std::string s1, s2;
    cin >> n1 >> n2 >> s1 >> s2;
    using PAM = OY::StaticPAM_string<Node, 28>;
    // using PAM = OY::StaticRollbackPAM_string<NodeWrap, 28, 19>;
    using node = PAM::node;
    // 将 s1 和 s2 用两个特殊字符连接，然后插入 PAM 中
    // 只有两个串都遍历到的结点才算数
    PAM pam;
    uint32_t ans = 0, ans_cnt = 0;
    pam.reserve(s1.size() + s2.size() + 2);
    for (char c : s1) {
        pam.push_back(c - 'a');
        pam.get_node(pam.query_node_index(pam.size() - 1))->m_mask |= 1;
    }
    pam.push_back(26);
    pam.push_back(27);
    for (char c : s2) {
        pam.push_back(c - 'a');
        node *p = pam.get_node(pam.query_node_index(pam.size() - 1));
        if (p->m_mask == 1) {
            uint32_t len = p->m_length;
            if (len > ans)
                ans = len, ans_cnt = 1;
            else if (len == ans)
                ans_cnt++;
            p->m_mask |= 2;
        }
    }

    cout << ans << ' ' << ans_cnt << endl;
}

int main() {
    solve_PAM();
}