/*
最后修改:
20231130
测试环境:
gcc11.2,c++11
clang12.0,C++11
msvc14.2,C++14
*/
#ifndef __OY_EULERSIEVE__
#define __OY_EULERSIEVE__

#include <algorithm>
#include <bitset>
#include <cstdint>
#include <numeric>
#include <vector>

namespace OY {
    namespace EulerSieve {
        using size_type = uint32_t;
        template <size_type MAX_RANGE, bool B>
        struct SieveArray {
            size_type m_val[MAX_RANGE + 1];
            void set(size_type i, size_type val) { m_val[i] = val; }
            size_type operator[](size_type i) const { return m_val[i]; }
        };
        template <size_type MAX_RANGE>
        struct SieveArray<MAX_RANGE, false> {
            void set(size_type i, size_type val) {}
            size_type operator[](size_type i) const { return 1; }
        };
        struct SievePair {
            size_type m_prime, m_count;
            bool operator<(const SievePair &rhs) const { return m_prime < rhs.m_prime; }
        };
        constexpr size_type get_estimated_ln(size_type x) {
            return x <= 7            ? 1
                   : x <= 32         ? 2
                   : x <= 119        ? 3
                   : x <= 359        ? 4
                   : x <= 1133       ? 5
                   : x <= 3093       ? 6
                   : x <= 8471       ? 7
                   : x <= 24299      ? 8
                   : x <= 64719      ? 9
                   : x <= 175196     ? 10
                   : x <= 481451     ? 11
                   : x <= 1304718    ? 12
                   : x <= 3524653    ? 13
                   : x <= 9560099    ? 14
                   : x <= 25874783   ? 15
                   : x <= 70119984   ? 16
                   : x <= 189969353  ? 17
                   : x <= 514278262  ? 18
                   : x <= 1394199299 ? 19
                                     : 20;
        }
        constexpr size_type get_estimated_Pi(size_type x) { return x / get_estimated_ln(x); }
        template <size_type MAX_RANGE, bool GetPhi = false, bool GetSmallFactor = false, bool GetBigFactor = false>
        struct Sieve {
            static constexpr size_type max_pi = get_estimated_Pi(MAX_RANGE);
            static SieveArray<MAX_RANGE, GetPhi> s_phi;
            static SieveArray<MAX_RANGE, GetSmallFactor> s_smallest_factor;
            static SieveArray<MAX_RANGE, GetBigFactor> s_biggest_factor;
            static std::bitset<MAX_RANGE + 1> s_isprime;
            static size_type s_primes[max_pi], s_prime_cnt;
            template <typename Callback>
            void _dfs(size_type index, size_type prod, const std::vector<SievePair> &pairs, Callback &&call) const {
                if (index == pairs.size())
                    call(prod);
                else {
                    auto &&pair = pairs[index];
                    size_type p = pair.m_prime, c = pair.m_count;
                    _dfs(index + 1, prod, pairs, call);
                    while (c--) _dfs(index + 1, prod *= p, pairs, call);
                }
            }
            Sieve(size_type range = MAX_RANGE) {
                s_isprime.set();
                s_isprime.reset(0);
                if (range >= 1) s_isprime.reset(1), s_smallest_factor.set(1, 1), s_biggest_factor.set(1, 1), s_phi.set(1, 1);
                for (int i = 2; i <= range; i++) {
                    if (s_isprime[i]) s_smallest_factor.set(i, i), s_biggest_factor.set(i, i), s_phi.set(i, i - 1), s_primes[s_prime_cnt++] = i;
                    for (size_type *it = s_primes, *end = s_primes + s_prime_cnt; it != end; ++it) {
                        auto p = *it, q = i * p;
                        if (q > range) break;
                        s_isprime.reset(q), s_smallest_factor.set(q, p), s_biggest_factor.set(q, s_biggest_factor[i]);
                        if (i % p)
                            s_phi.set(q, s_phi[i] * (p - 1));
                        else {
                            s_phi.set(q, s_phi[i] * p);
                            break;
                        }
                    }
                }
            }
            bool is_prime(size_type i) const { return (i & 1) || i == 2 ? s_isprime[i] : false; }
            size_type get_Euler_Phi(size_type i) const {
                static_assert(GetPhi);
                return (i & 1) ? s_phi[i] : s_phi[i >> std::countr_zero(i)] << std::countr_zero(i) - 1;
            }
            size_type query_smallest_factor(size_type i) const {
                static_assert(GetSmallFactor);
                return (i & 1) ? s_smallest_factor[i] : 2;
            }
            size_type query_biggest_factor(size_type i) const {
                static_assert(GetBigFactor);
                return s_biggest_factor[i];
            }
            size_type query_kth_prime(size_type k) const { return s_primes[k]; }
            size_type count() const { return s_prime_cnt; }
            std::vector<SievePair> decomposite(size_type n) const {
                static_assert(GetSmallFactor);
                std::vector<SievePair> res;
                if (n % 2 == 0) {
                    size_type x = std::countr_zero(n);
                    res.push_back({2, x}), n >>= x;
                }
                while (n > 1) {
                    size_type cur = query_smallest_factor(n), cnt = 0;
                    do {
                        n /= cur, cnt++;
                    } while (query_smallest_factor(n) == cur);
                    res.push_back({cur, cnt});
                }
                return res;
            }
            template <typename Callback>
            void enumerate_factors(const std::vector<SievePair> &pairs, Callback &&call) const { _dfs(0, 1, pairs, call); }
            template <bool Sorted = false>
            std::vector<size_type> get_factors(size_type n) const {
                static_assert(GetSmallFactor);
                std::vector<size_type> res;
                size_type count = 1;
                auto pairs = decomposite(n);
                for (auto &&pair : pairs) count *= pair.m_count + 1;
                res.reserve(count);
                enumerate_factors(pairs, [&](size_type f) { res.push_back(f); });
                if constexpr (Sorted) std::sort(res.begin(), res.end());
                return res;
            }
        };
        template <size_type MAX_RANGE, bool GetPhi, bool GetSmallFactor, bool GetBigFactor>
        SieveArray<MAX_RANGE, GetPhi> Sieve<MAX_RANGE, GetPhi, GetSmallFactor, GetBigFactor>::s_phi;
        template <size_type MAX_RANGE, bool GetPhi, bool GetSmallFactor, bool GetBigFactor>
        SieveArray<MAX_RANGE, GetSmallFactor> Sieve<MAX_RANGE, GetPhi, GetSmallFactor, GetBigFactor>::s_smallest_factor;
        template <size_type MAX_RANGE, bool GetPhi, bool GetSmallFactor, bool GetBigFactor>
        SieveArray<MAX_RANGE, GetBigFactor> Sieve<MAX_RANGE, GetPhi, GetSmallFactor, GetBigFactor>::s_biggest_factor;
        template <size_type MAX_RANGE, bool GetPhi, bool GetSmallFactor, bool GetBigFactor>
        size_type Sieve<MAX_RANGE, GetPhi, GetSmallFactor, GetBigFactor>::s_primes[Sieve<MAX_RANGE, GetPhi, GetSmallFactor, GetBigFactor>::max_pi];
        template <size_type MAX_RANGE, bool GetPhi, bool GetSmallFactor, bool GetBigFactor>
        size_type Sieve<MAX_RANGE, GetPhi, GetSmallFactor, GetBigFactor>::s_prime_cnt;
        template <size_type MAX_RANGE, bool GetPhi, bool GetSmallFactor, bool GetBigFactor>
        std::bitset<MAX_RANGE + 1> Sieve<MAX_RANGE, GetPhi, GetSmallFactor, GetBigFactor>::s_isprime;
    }
}

#endif