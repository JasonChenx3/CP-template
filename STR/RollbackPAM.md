### 一、模板类别

​	序列：可回滚的回文自动机。

​	练习题目：

1. [P3649 [APIO2014] 回文串](https://www.luogu.com.cn/problem/P3649)
2. [P4287 [SHOI2011] 双倍回文](https://www.luogu.com.cn/problem/P4287)
3. [P4762 [CERC2014] Virus synthesis](https://www.luogu.com.cn/problem/P4762)
4. [P5496 【模板】回文自动机（PAM）](https://www.luogu.com.cn/problem/P5496)
5. [P5555 秩序魔咒](https://www.luogu.com.cn/problem/P5555)
6. [P5685 [JSOI2013] 快乐的 JYY](https://www.luogu.com.cn/problem/P5685)

### 二、模板功能

​	本模板与普通 `PAM` 模板功能一致，仅仅是保证了 `push_back` 和 `pop_back` 不会因最坏情况而超时。

#### 1.构造回文自动机

1. 数据类型

   模板参数 `size_type DepthBitCount` ，表示回文树的最大高度的二进制位长。

   其余同普通 `PAM` 。

2. 时间复杂度

   $O(n)$ 。
   
3. 备注

   同普通 `PAM` 。

#### 2.在尾部添加元素(push_back)

1. 数据类型

   输入参数 `const value_type &elem` ，表示在末尾添加的元素值。

2. 时间复杂度

   均摊 $O(1)$ ，最坏 $O(\log n)$ 。

3. 备注

   本方法是为了动态延伸区间而准备的。

#### 3.移除尾部元素(pop_back)

1. 数据类型

2. 时间复杂度

   均摊 $O(1)$ ，最坏 $O(\log n)$ 。

3. 备注

   本方法是为了动态收缩区间而准备的。

#### 4.查询结点的长度一半以下的失配结点(query_fail_to_half)

1. 数据类型

   输入参数 `size_type node_index` ，表示回文树中结点的索引。

   返回类型 `size_type` ，表示查询结果。

2. 时间复杂度

   $O(\log n)$ 。

#### 5.按照失配顺序对某回文串的每个本质不同的失配串结点调用回调函数(do_for_each_distinct_node)

1. 输入类型

   输入参数 `size_type init_node_index` ，表示初始时的回文树结点。

   输入参数 `Callback &&call` ，表示对每个结点调用的回调函数。

2. 时间复杂度

   $O(\log n)$ 。

3. 备注

   一个回文串的失配串的数量，最多可以达到和原串长度同数量级。但是，其本质不同的失配串最多只有 `log n` 种。

### 三、模板示例

```c++
#include "IO/FastIO.h"
#include "STR/RollbackPAM.h"

#include <map>

void test_find_longest() {
    cout << "test find longest:\n";
    std::string s = "abacabadabadabacabad";

    using PAM = OY::StaticRollbackPAM_string<>;
    PAM pam(s.size(), [&](int i) { return s[i] - 'a'; });
    cout << "longest palindromic string endsWith index 19 is: \n"
         << s.substr(19 - pam.get_node(pam.query_node_index(19))->m_length + 1, pam.get_node(pam.query_node_index(19))->m_length) << endl;

    cout << "longest palindromic string endsWith index 18 is: \n"
         << s.substr(18 - pam.get_node(pam.query_node_index(18))->m_length + 1, pam.get_node(pam.query_node_index(18))->m_length) << endl;

    cout << "longest palindromic string endsWith index 14 is: \n"
         << s.substr(14 - pam.get_node(pam.query_node_index(14))->m_length + 1, pam.get_node(pam.query_node_index(14))->m_length) << endl;

    cout << endl;
}

void test_find_all_occurrences() {
    cout << "test find all ocuurrences:\n";
    std::string s = "abacabadabadabacabad";

    using PAM = OY::StaticRollbackPAM_string<>;
    PAM pam(s.size(), [&](int i) { return s[i] - 'a'; });

    auto cur = pam.query_node_index(18);
    do {
        cout << s.substr(18 - pam.get_node(cur)->m_length + 1, pam.get_node(cur)->m_length) << endl;
        cur = pam.query_fail(cur);
    } while (pam.is_node(cur));
    cout << endl;
}

struct NodeWrap {
    std::map<uint32_t, uint32_t> m_child;
    bool has_child(uint32_t index) const { return m_child.find(index) != m_child.end(); }
    void add_child(uint32_t index, uint32_t child) { m_child[index] = child; }
    void remove_child(uint32_t index) { m_child.erase(index); }
    uint32_t get_child(uint32_t index) const { return m_child.find(index)->second; }
};
void test_map_node() {
    cout << "test map node:\n";
    std::string s = "abcbaabcbaabcbaabcbaabcbaabcba";

    // 找出以下标 29 作为结尾的所有的本质不同的回文串
    // 什么叫本质不同？当字符串出现周期之后，一个字符串的后缀子串中，超过自身长度一半的长度会形成一个等差数列
    // 对于此类的子串，调用本方法只会访问次长的及最短的
    using PAM = OY::RollbackPAM::Automaton<NodeWrap, std::string, 10>;
    PAM pam(s.size(), [&](int i) { return s[i] - 'a'; });

    pam.do_for_each_distinct_node(pam.query_node_index(29), [&](uint32_t a) {
        cout << "len = " << pam.get_node(a)->m_length << " : " << s.substr(s.size() - pam.get_node(a)->m_length) << endl;
    });
    cout << endl;
}

int main() {
    test_find_longest();
    test_find_all_occurrences();
    test_map_node();
}
```

```
/*
#输出如下
test find longest:
longest palindromic string endsWith index 19 is: 
dabacabad
longest palindromic string endsWith index 18 is: 
abacabadabadabacaba
longest palindromic string endsWith index 14 is: 
abadabadaba

test find all ocuurrences:
abacabadabadabacaba
abacaba
aba
a

test map node:
len = 30 : abcbaabcbaabcbaabcbaabcbaabcba
len = 25 : abcbaabcbaabcbaabcbaabcba
len = 5 : abcba
len = 1 : a

```
