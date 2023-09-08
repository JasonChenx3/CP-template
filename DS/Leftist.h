/*
最后修改:
10230907
测试环境:
gcc11.2,c++11
clang12.0,C++11
msvc14.2,C++14
*/
#ifndef __OY_LEFTIST__
#define __OY_LEFTIST__

#include <algorithm>
#include <cstdint>
#include <functional>
#include <numeric>

namespace OY {
    namespace Leftist {
        using size_type = uint32_t;
        struct NoInit {};
        template <typename Tp, typename Compare = std::less<Tp>>
        struct BaseNodeWrapper {
            template <typename Node>
            struct type {
                using value_type = Tp;
                value_type m_val;
                static bool comp(const value_type &x, const value_type &y) { return Compare()(x, y); }
                void set(const value_type &val) { m_val = val; }
                const value_type &get() const { return m_val; }
            };
        };
        template <typename Tp, typename Operation, typename Compare = std::less<Tp>>
        struct CustomNodeWrapper {
            template <typename Node>
            struct type {
                using value_type = Tp;
                static Operation s_op;
                static bool comp(const value_type &x, const value_type &y) { return Compare()(x, y); }
                value_type m_val, m_info;
                void set(const value_type &val) { m_val = val; }
                const value_type &get() const { return m_val; }
                void pushup(Node *lchild, Node *rchild) { m_info = lchild->is_null() ? (rchild->is_null() ? get() : s_op(get(), rchild->m_info)) : (rchild->is_null() ? s_op(lchild->m_info, get()) : s_op(s_op(lchild->m_info, get()), rchild->m_info)); }
            };
        };
        template <typename Tp, typename Operation, typename Compare>
        template <typename Node>
        Operation CustomNodeWrapper<Tp, Operation, Compare>::type<Node>::s_op;
        template <typename Tp, typename ModifyType, typename Operation, typename Mapping, typename Composition, bool InitClearLazy>
        struct CustomLazyNodeWrapper {
            template <typename Node>
            struct type {
                using value_type = Tp;
                static Operation s_op;
                static Mapping s_map;
                static Composition s_com;
                static ModifyType s_default_modify;
                value_type m_val, m_info;
                ModifyType m_modify;
                void set(const value_type &val) {
                    m_val = val;
                    if constexpr (InitClearLazy) m_modify = s_default_modify;
                }
                const value_type &get() const { return m_val; }
                void modify(const ModifyType &modify) {
                    m_val = s_map(modify, m_val, 1);
                    m_info = s_map(modify, m_info, ((Node *)this)->m_size);
                    m_modify = s_com(modify, m_modify);
                }
                void pushup(Node *lchild, Node *rchild) { m_info = lchild->is_null() ? (rchild->is_null() ? get() : s_op(get(), rchild->m_info)) : (rchild->is_null() ? s_op(lchild->m_info, get()) : s_op(s_op(lchild->m_info, get()), rchild->m_info)); }
                void pushdown(Node *lchild, Node *rchild) {
                    if (!lchild->is_null()) lchild->modify(m_modify);
                    if (!rchild->is_null()) rchild->modify(m_modify);
                    m_modify = s_default_modify;
                };
            };
        };
        template <typename Tp, typename ModifyType, typename Operation, typename Mapping, typename Composition, bool InitClearLazy>
        template <typename Node>
        Operation CustomLazyNodeWrapper<Tp, ModifyType, Operation, Mapping, Composition, InitClearLazy>::type<Node>::s_op;
        template <typename Tp, typename ModifyType, typename Operation, typename Mapping, typename Composition, bool InitClearLazy>
        template <typename Node>
        Mapping CustomLazyNodeWrapper<Tp, ModifyType, Operation, Mapping, Composition, InitClearLazy>::type<Node>::s_map;
        template <typename Tp, typename ModifyType, typename Operation, typename Mapping, typename Composition, bool InitClearLazy>
        template <typename Node>
        Composition CustomLazyNodeWrapper<Tp, ModifyType, Operation, Mapping, Composition, InitClearLazy>::type<Node>::s_com;
        template <typename Tp, typename ModifyType, typename Operation, typename Mapping, typename Composition, bool InitClearLazy>
        template <typename Node>
        ModifyType CustomLazyNodeWrapper<Tp, ModifyType, Operation, Mapping, Composition, InitClearLazy>::type<Node>::s_default_modify;
#ifdef __cpp_lib_void_t
        template <typename... Tp>
        using void_t = std::void_t<Tp...>;
#else
        template <typename... Tp>
        struct make_void {
            using type = void;
        };
        template <typename... Tp>
        using void_t = typename make_void<Tp...>::type;
#endif
        template <typename Tp, typename NodePtr, typename = void>
        struct Has_pushup : std::false_type {};
        template <typename Tp, typename NodePtr>
        struct Has_pushup<Tp, NodePtr, void_t<decltype(std::declval<Tp>().pushup(std::declval<NodePtr>(), std::declval<NodePtr>()))>> : std::true_type {};
        template <typename Tp, typename NodePtr, typename = void>
        struct Has_pushdown : std::false_type {};
        template <typename Tp, typename NodePtr>
        struct Has_pushdown<Tp, NodePtr, void_t<decltype(std::declval<Tp>().pushdown(std::declval<NodePtr>(), std::declval<NodePtr>()))>> : std::true_type {};
        template <typename Tp, typename Fp, typename = void>
        struct Has_comp : std::false_type {};
        template <typename Tp, typename Fp>
        struct Has_comp<Tp, Fp, void_t<decltype(std::declval<Tp>().comp(std::declval<Fp>(), std::declval<Fp>()))>> : std::true_type {};
        template <template <typename> typename NodeWrapper, size_type MAX_NODE = 1 << 20>
        struct Heap {
            struct node : NodeWrapper<node> {
                size_type m_dist, m_lchild, m_rchild;
                bool is_null() const { return this == s_buffer; }
                node *lchild() { return s_buffer + m_lchild; }
                node *rchild() { return s_buffer + m_rchild; }
            };
            using value_type = typename node::value_type;
            static node s_buffer[MAX_NODE];
            static size_type s_use_count;
            size_type m_root{};
            template <typename Modify = NoInit>
            static size_type _newnode(const value_type &val, Modify &&modify = Modify()) {
                s_buffer[s_use_count].set(val), s_buffer[s_use_count].m_dist = 1;
                if constexpr (!std::is_same<typename std::decay<Modify>::type, NoInit>::value) modify(s_buffer + s_use_count);
                return s_use_count++;
            }
            static bool _comp(const value_type &x, const value_type &y) {
                if constexpr (Has_comp<node, value_type>::value)
                    return node::comp(x, y);
                else
                    return x < y;
            }
            static void _pushup(size_type x) {
                if constexpr (Has_pushup<node, node *>::value) s_buffer[x].pushup(s_buffer[x].lchild(), s_buffer[x].rchild());
            }
            static void _pushdown(size_type x) {
                if constexpr (Has_pushdown<node, node *>::value) s_buffer[x].pushdown(s_buffer[x].lchild(), s_buffer[x].rchild());
            }
            static size_type _merge(size_type x, size_type y) {
                if (!x || !y) return x | y;
                if (_comp(s_buffer[x].get(), s_buffer[y].get())) std::swap(x, y);
                _pushdown(x);
                s_buffer[x].m_rchild = _merge(s_buffer[x].m_rchild, y);
                if (s_buffer[x].rchild()->m_dist > s_buffer[x].lchild()->m_dist) std::swap(s_buffer[x].m_lchild, s_buffer[x].m_rchild);
                s_buffer[x].m_dist = s_buffer[x].rchild()->m_dist + 1, _pushup(x);
                return x;
            }
            node *root() const { return s_buffer + m_root; }
            void clear() { m_root = 0; }
            bool empty() const { return !m_root; }
            template <typename Modify = NoInit>
            void push(const value_type &val, Modify modify = Modify()) { m_root = _merge(m_root, _newnode(val, modify)); }
            value_type top() const { return s_buffer[m_root].get(); }
            void pop() { _pushdown(m_root), m_root = _merge(s_buffer[m_root].m_lchild, s_buffer[m_root].m_rchild); }
            void join(Heap<NodeWrapper, MAX_NODE> other) { m_root = _merge(m_root, other.m_root); }
        };
        template <template <typename> typename NodeWrapper, size_type MAX_NODE>
        typename Heap<NodeWrapper, MAX_NODE>::node Heap<NodeWrapper, MAX_NODE>::s_buffer[MAX_NODE];
        template <template <typename> typename NodeWrapper, size_type MAX_NODE>
        size_type Heap<NodeWrapper, MAX_NODE>::s_use_count = 1;
    }
    template <typename Tp, typename Compare = std::less<Tp>, Leftist::size_type MAX_NODE = 1 << 20, typename Operation, typename TreeType = Leftist::Heap<Leftist::CustomNodeWrapper<Tp, Operation, Compare>::template type, MAX_NODE>>
    auto make_LeftistHeap(Operation op) -> TreeType { return TreeType(); }
    template <typename Tp, typename Compare = std::less<Tp>, Leftist::size_type MAX_NODE = 1 << 20, typename TreeType = Leftist::Heap<Leftist::CustomNodeWrapper<Tp, const Tp &(*)(const Tp &, const Tp &), Compare>::template type, MAX_NODE>>
    auto make_LeftistHeap(const Tp &(*op)(const Tp &, const Tp &)) -> TreeType { return TreeType::node::s_op = op, TreeType(); }
    template <typename Tp, typename Compare = std::less<Tp>, Leftist::size_type MAX_NODE = 1 << 20, typename TreeType = Leftist::Heap<Leftist::CustomNodeWrapper<Tp, Tp (*)(Tp, Tp), Compare>::template type, MAX_NODE>>
    auto make_LeftistHeap(Tp (*op)(Tp, Tp)) -> TreeType { return TreeType::node::s_op = op, TreeType(); }
    template <typename Tp, typename ModifyType, bool InitClearLazy, typename Compare = std::less<Tp>, Leftist::size_type MAX_NODE = 1 << 20, typename Operation, typename Mapping, typename Composition, typename TreeType = Leftist::Heap<Leftist::CustomLazyNodeWrapper<Tp, ModifyType, Operation, Mapping, Composition, InitClearLazy>::template type, MAX_NODE>>
    auto make_lazy_LeftistHeap(Operation op, Mapping map, Composition com, const ModifyType &default_modify = ModifyType()) -> TreeType { return TreeType::node::s_default_modify = default_modify, TreeType(); }
    template <typename Tp, typename Compare = std::less<Tp>, Leftist::size_type MAX_NODE = 1 << 20>
    using LeftistHeap = Leftist::Heap<Leftist::BaseNodeWrapper<Tp, Compare>::template type, MAX_NODE>;
}

#endif