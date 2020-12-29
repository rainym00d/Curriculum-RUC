/*
** Title: 求二维空间中的欧式距离最小的点对
** Author: 邵宁录
** Date: 2020-09-23
** Last modify: 2020-09-26 
*/

/*
** 使用说明: 要先用python代码处理Excel数据，然后再跑本代码
*/

/*
** 时间复杂度分析
** 假设: 数据都是有序的
** 分析: 
** 首先每次递归减小的规模为n/2，因此至少有 T(n) = 2T(n/2) + b，其中b为每次执行的代价
** 再讨论计算两个区域之间的最小距离，记两个区域分别为S1和S2
** 找到S1和S2中，横坐标与分界线的差小于等于d的那些点，记这些点分别为S1'和S2'
** 由于假设中所有数据都是有序的，因此只需遍历S1'中的点，并且在S2'中寻找最多6个点并记录位置，就能实现O(n)的复杂度
** 因此，最终递归式为 T(n) = 2T(n/2) + O(n)
** 根据主方法得，时间复杂度为O(nlogn)
*/


#include <iostream>
#include <cstdio>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <cstdlib>
using namespace std;


const double INF = 23333333333;
const int N = 2333333;
int num = 0;

// Point的结构体
struct Point
{
    // 横纵坐标
    double x;
    double y;
} point[N];

// 重载sort的cmp函数，排序Point，优先级为 x >> y，升序 
bool cmp_point(const Point &A, const Point &B) {
    if(A.x != B.x) {
        return A.x < B.x;
    }
    return A.y < B.y;
}

// 重载sort的cmp函数，排序Point的index，升序
bool cmp_y(const int &index_1, const int &index_2) {
    return point[index_1].y < point[index_2].y;
}

// 计算距离
double distance(const Point &A, const Point &B) {
    return sqrt(pow((A.x - B.x), 2) + pow((A.y - B.y), 2));
}

// 递归求解二维空间中的最近点对
double closest_pair(int left, int right) {
    double d = INF;                 // 最小距离
    // boundary condition
    if(left == right) {
        return d;
    }
    if(left + 1 == right) {
        return distance(point[left], point[right]);
    }
    int mid = (left + right) >> 1;  // 中位数下标
    
    // 递归求解
    double d_1 = closest_pair(left, mid);
    double d_2 = closest_pair(mid + 1, right);
    d = min(d_1, d_2);
    // 合并
    int i, j, m, n;
    int s1_index[mid - left + 1];   // 左半部分的点空间
    int s2_index[right - mid];      // 右半部分的点空间
    m = 0;
    n = 0;
    // 寻找s1与s2中的点
    for(i = left; i <= mid; i ++) {
        if(fabs(point[i].x - point[mid].x) <= d) {
            s1_index[m ++] = i;
        }
    }
    for(j = mid+1; j <= right; j ++) {
        if(fabs(point[j].x - point[mid].x) <= d) {
            s2_index[n ++] = j;
        }
    }
    // 根据y的大小对index进行排序
    sort(s2_index, s2_index+n, cmp_y);

    // 计算s1与s2中的点的最小距离
    double d_cross = INF;           // s1与s2之间的最小距离
    for(i = 0; i < m; i ++) {
        // 在s2中寻找d*2d的矩形内是否有符合条件的点
        int index = -1;
        for(j = 0; j < n; j ++) {
            if(fabs(point[s2_index[j]].y - point[s1_index[i]].y) <= d) {
                index = j;
                break;
            }
        }
        // 如果能找到符合条件的第一个点
        if(index != -1) {
            // 最多只需比较6个点
            for(j = index; j < n && j < index+6; j ++) {
                double d_tmp = distance(point[s1_index[i]], point[s2_index[j]]);
                d_cross = min(d_cross, d_tmp);
            }
        }
    }
    return min(d, d_cross);
}

// 从csv中读入
void read_csv(char* path){
    cout << "[INFO] Read file start!" << endl;
    ifstream readfile;
    readfile.open(path);
    char x[10], y[10];
    char s[15];
    while(readfile.getline(s, 15)) {
        sscanf(s, "%[^,],%s", x, y);
        point[num].x = atof(x);
        point[num].y = atof(y);
        // 记录点的数量
        num ++;
    }
    cout << "[INFO] Finish!" << endl;
}

int main(int argc, char const *argv[])
{
    read_csv("去重后数据.csv");
    // 对读入的点进行排序
    sort(point, point+num, cmp_point);
    cout << "min distance: " << closest_pair(0, num-1) << endl;
    return 0;
}

