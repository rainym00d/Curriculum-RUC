/*
 * @Description: 算法导论第15章最长公共子序列实现
 * @Author: rainym00d
 * @Github: https://github.com/rainym00d
 * @Date: 2020-11-01 19:37:54
 * @LastEditors: rainym00d
 * @LastEditTime: 2020-11-06 11:52:25
 */
#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>

using namespace std;

const int SUP = 23333333;
const int INF = -23333333;
const int N = 1024;

int c[N][N];
int b[N][N];    // 1: 左上  2: 上   3: 左

// 带备忘的自顶向下
int MemoizedLcsLengthAux(string &s1, string &s2, int i, int j)
{
    // 边界条件
    if (c[i][j] != -1)
    {
        return c[i][j];
    }
    // 状态转移方程
    // 第一种情况
    if (i == 0 || j == 0)
    {
        c[i][j] = 0;
    }
    else
    {   
        // 第二种情况
        if (s1[i - 1] == s2[j - 1])
        {
            c[i][j] = MemoizedLcsLengthAux(s1, s2, i - 1, j - 1) + 1;
            b[i][j] = 1;
        }
        // 第三种情况
        else
        {
            int p = MemoizedLcsLengthAux(s1, s2, i - 1, j);
            int q = MemoizedLcsLengthAux(s1, s2, i, j - 1);
            // 若是上面的比较大
            if (p >= q)
            {
                c[i][j] = p;
                b[i][j] = 2;
            }
            // 若是左面的比较大
            else
            {
                c[i][j] = q;
                b[i][j] = 3;
            }
        }
    }
    return c[i][j];
}

int MemoizedLcsLength(string &s1, string &s2)
{
    memset(c, -1, sizeof c);

    return MemoizedLcsLengthAux(s1, s2, s1.length(), s2.length());
}

// 自底向上
void LcsLength(string &s1, string &s2)
{
    // 获取字符串长度
    int m = s1.length();
    int n = s2.length();
    // 从左上角开始遍历，自上而下
    for (int i = 0; i < m; i ++)
    {
        // 从左往右
        for (int j = 0; j < n; j ++)
        {
            // 若当前位置字符相同，则为左上角+1
            if (s1[i] == s2[j])
            {
                c[i + 1][j + 1] = c[i][j] + 1;
                b[i + 1][j + 1] = 1;
            }
            // 上面的比较大
            else if (c[i][j + 1] >= c[i + 1][j])
            {
                c[i + 1][j + 1] = c[i][j + 1];
                b[i + 1][j + 1] = 2;
            }
            // 左面的比较大
            else
            {
                c[i + 1][j + 1] = c[i + 1][j];
                b[i + 1][j + 1] = 3;
            }
        }
    }
}
// 打印函数，s务必得是第一个字符串
void PrintLcs(string &s, int i, int j)
{
    if (i == 0 || j == 0)
    {
        return;
    }
    if (b[i][j] == 1)
    {
        PrintLcs(s, i - 1, j - 1);
        cout << s[i - 1];
    }
    else if (b[i][j] == 2)
    {
        PrintLcs(s, i - 1, j);
    }
    else
    {
        PrintLcs(s, i, j -1);
    }   
}

int main(int argc, char const *argv[])
{
    // 二维表初始化
    memset(c, 0, sizeof c);
    memset(b, 0, sizeof b);

    string s1 = "ABCBDAB";
    string s2 = "BDCABA";
    // 自底向上
    cout << "自底向上：" << endl;
    LcsLength(s1, s2);
    cout << "长度：" << c[s1.length()][s2.length()] << endl;
    PrintLcs(s1, s1.length(), s2.length());
    cout << endl;
    // 带备忘的自顶向下
    cout << "带备忘的自顶向下：" << endl;
    cout << "长度" << MemoizedLcsLength(s1, s2) << endl;
    PrintLcs(s1, s1.length(), s2.length());
    cout << endl;
    
    return 0;
}

