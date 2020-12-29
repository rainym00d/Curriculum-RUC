/*
 * @Description: 算法导论第16章贪心算法16.2-4实现
 * @Author: rainym00d
 * @Github: https://github.com/rainym00d
 * @Date: 2020-11-11 20:38:54
 * @LastEditors: rainym00d
 * @LastEditTime: 2020-11-13 15:25:11
 */
#include <iostream>
#include <cstdlib>
#include <vector>

using namespace std;

vector<int> GetWater(int &m, vector<int> &stops)
{
    vector<int> ans;
    int n = stops.size();
    int already_go = 0;
    
    // 从1开始，0是出发点
    for (int i = 1; i < n; i ++)
    {
        if (stops[i] > m + already_go)
        {
            // 加入站点
            ans.push_back(i - 1);
            // 增加已经走过的路程
            already_go = stops[i - 1];
        }
    }
    return ans;
}


int main(int argc, char const *argv[])
{
    // 初始化
    int a[] = {0, 30, 40, 60, 100, 130, 170};

    vector<int> stops(a, a + sizeof a / sizeof a[0]);
    int m = 40;

    vector<int> ans = GetWater(m, stops);

    cout << "一共经过" << ans.size() << "个站点" << endl;
    cout << "站点为：";
    for (auto i : ans)
    {
        cout << i << " ";
    }
    cout << endl;
    
    
    return 0;
}
