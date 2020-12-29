/*
 * @Description: 算法导论第15章矩阵链乘法实现
 * @Author: rainym00d
 * @Github: https://github.com/rainym00d
 * @Date: 2020-10-27 14:03:31
 * @LastEditors: rainym00d
 * @LastEditTime: 2020-11-06 21:42:05
 */

// 输入：n个矩阵的大小，共有n+1个数，如[1, 2, 3, 4, 5]
// 输出：最小的乘法次数

#include <iostream>
#include <cstdlib>
#include <vector>

using namespace std;

const int SUP = 23333333;
const int INF = -23333333;
const int N = 1024;



// 带备忘录的自顶向下
int m[N][N];

int LookupChain(vector<int> &p, int i, int j)
{
    if (m[i][j] < SUP)
    {
        return m[i][j];
    }
    if (i == j)
    {
        m[i][j] = 0;
    }
    else
    {
        for (int k = i; k < j; k ++)
        {
            // 状态转移方程
            int q = LookupChain(p, i, k) + LookupChain(p, k + 1, j) + p[i] * p[k + 1] * p[j + 1];
            if (q < m[i][j])
            {
                m[i][j] = q;
            }
        }
    }
    return m[i][j];
}

int MemoizedMatrixChain(vector<int> &p)
{
    int n = p.size() - 1;
    for (int i = 0; i < N; i ++)
    {
        for (int j = 0; j < N; j ++)
        {
            m[i][j] = SUP;
        } 
    }
    return LookupChain(p, 0, n - 1);
}


int main(int argc, char const *argv[])
{
    // 在这里输入矩阵的大小
    int a[] = {30, 35, 15, 5, 10, 20, 25};
    vector<int> p;
    for (int ai : a)
    {
        p.push_back(ai);
    }
    // 矩阵数量n
    int n = p.size() - 1;
    cout << MemoizedMatrixChain(p) << endl;
    
    return 0;
}
