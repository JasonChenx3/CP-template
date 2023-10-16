#include "DS/AdjDiff2D.h"
#include "IO/FastIO.h"

/*
[U223961 差分矩阵](https://www.luogu.com.cn/problem/U223961)
*/
/**
 * 本题为二维差分模板题
*/
int main() {
    uint32_t n, m, q;
    cin >> n >> m >> q;
    OY::AdjDiff2D::Table<int32_t, false, 1 << 20> S(n, m, [](uint32_t i, uint32_t j) {
        int32_t x;
        cin >> x;
        return x;
    });

    S.switch_to_difference();
    for (uint32_t i = 0; i < q; i++) {
        uint32_t x1, y1, x2, y2;
        int32_t c;
        cin >> x1 >> y1 >> x2 >> y2 >> c;
        S.add(x1 - 1, x2 - 1, y1 - 1, y2 - 1, c);
    }
    
    S.switch_to_value();
    for (uint32_t i = 0; i < n; i++) {
        for (uint32_t j = 0; j < m; j++) cout << S.query(i, j) << ' ';
        cout << '\n';
    }
}