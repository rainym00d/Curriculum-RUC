/*
 * @Description: Dijkstra算法实现
 * @Author: rainym00d
 * @Github: https://github.com/rainym00d
 * @Date: 2020-11-18 21:29:31
 * @LastEditors: rainym00d
 * @LastEditTime: 2020-11-20 17:41:18
 */
#include <iostream>
#include <cstdlib>
#include <vector>
#include <stack>

using namespace std;


const int SUP = 23333333;
const int INF = -23333333;
const int N = 1024;


// int M[N][N] = {
//                 {0, 12, SUP, SUP, SUP, 16, 14}, 
//                 {12, 0, 10, SUP, SUP, 7, SUP},
//                 {SUP, 10, 0, 3, 5, 6, SUP},
//                 {SUP, SUP, 3, 0, 4, SUP, SUP},
//                 {SUP, SUP, 5, 4, 0, 2, 8},
//                 {16, 7, SUP, SUP, SUP, 2, 0},
//                 {14, SUP, SUP, SUP, 8, SUP ,0}
//             };

// 图的矩阵表示，样例来自于PPT
int M[N][N] = {
                {0, 10, SUP, 30, 100}, 
                {SUP, 0, 50, SUP, SUP},
                {SUP, SUP, 0, SUP, 10},
                {SUP, SUP, 20, 0, 60},
                {SUP, SUP, SUP, SUP, SUP}
            };

// 存放最短距离
int d[N] = {0};
// 存放路径
int path[N];

// Dijkstra算法主函数
void Dijkstra(int source, const int &n)
{
    // 记录被访问过的节点
    int vis[N] = {0};
    // 将初始节点标记为已访问
    vis[source] = 1;
    // 初始化d
    for (int i = 0; i < n; i ++)
    {
        d[i] = M[source][i];
    }
    // 开始遍历其他节点
    for (int i = 1; i < n; i ++)
    {
        // 记录最小的距离
        int min = SUP;
        // 记录最小的距离所对应的节点
        int min_num;
        // 遍历所有节点，找到最小的那个
        for (int j = 0; j < n; j ++)
        {
            if (vis[j] == 0 && d[j] < min)
            {
                min = d[j];
                min_num = j;
            }
        }
        // 将找到的那个节点标记为访问过
        vis[min_num] = 1;
        // 进行下一轮的寻找路径
        for (int j = 0; j < n; j ++)
        {
            if (d[j] > min + M[min_num][j])
            {
                d[j] = min + M[min_num][j];
                path[j] = min_num;
            }
        }
    }
}

// 打印路径的函数
void print(int source, int n)
{
    // 用于辅助打印的栈
    stack<int> s;
    for (int i = 0; i < n; i ++)
    {
        // 如果是初始节点，则跳过，不然会打印出一个初始节点=>初始节点的路径
        if (i == source)
        {
            continue;
        }
        // 将路径压入栈，因为路径是从最后面找到最前面，而栈可以让它先进后出
        int j = i;
        while (path[j] != -1)
        {
            s.push(j);
            j = path[j];
        }
        s.push(j);
        // 打印
        printf("%d=>%d, length: %d, path: %d ", source, i, d[i], source);
        while (!s.empty())
        {
            printf("%d ", s.top());
            s.pop();
        }
        printf("\n");
    }
    
}


int main(int argc, char const *argv[])
{
    // 初始化全局变量
    memset(path, -1, sizeof path);
    // 初始化图矩阵
    int n = 7;
    // 算法开始，计算第0个点到其他点的最短路径
    Dijkstra(0, 5);
    print(0, 5);

    return 0;
}
