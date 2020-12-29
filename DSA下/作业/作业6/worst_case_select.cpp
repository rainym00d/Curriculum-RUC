/*
 * @Description: 算法导论第9章选择算法（worst case select）实现
 * @Author: rainym00d
 * @Github: https://github.com/rainym00d
 * @Date: 2020-10-21 11:17:45
 * @LastEditors: rainym00d
 * @LastEditTime: 2020-10-23 20:04:33
 */
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>

using namespace std;

// 从外部读入的接口
void ReadData(const char *filename, vector<int> &array)
{
    string data;

    ifstream infile;
    infile.open(filename, ios::in);

    if (!infile.is_open())
    {
        cout << "[ERROR] Open file failed!" << endl;
        exit(1);
    }

    while (getline(infile, data))
    {
        array.push_back(stoi(data));
    }
    infile.close();
}

bool cmp(int a, int b)
{
    return a < b;
}

int Partition(vector<int> &A, int left, int right)
{

    int x = A[right];
    int i = left;
    for (int j = left; j < right; j ++)
    {
        if (A[j] <= x)
        {
            swap(A[i ++], A[j]);
        }
    }
    swap(A[i], A[right]);
    
    return i;
}

int WorstCasePartition(vector<int> &A, int left, int right, int x)
{
    int index;
    for (int i = left; i <= right; i ++)
    {
        if (A[i] == x)
        {
            index = i;
            break;
        }
    }
    swap(A[index], A[right]);
    return Partition(A, left, right);
}


int WorstCaseSelect(vector<int> &A, int left, int right, int k)
{
    // 边界条件
    if (right - left + 1 < 140)
    {
        // 插入排序，稳定
        __insertion_sort(A.begin() + left, A.begin() + right + 1, cmp);
        return A[left + k - 1];
    }
    
    // 5个为一组进行插入排序，选出中位数，如果有俩中位数，取较小的
    int t = left;   // 从left开始计数的下标，将每组的中位数交换到这个下标的位置
    for (int i = left; i <= right; i += 5)
    {
        vector<int>::iterator start = next(A.begin(), i);
        // 不足5个就取剩下全部的
        vector<int>::iterator end = i + 5 <= right ? next(A.begin(), i + 5) : next(A.begin(), right);
        __insertion_sort(start, end, cmp);
        swap(A[t], *(start + (end - start) / 2));
        t ++;
    }
    // 中位数的中位数
    int mid_of_mid = WorstCaseSelect(A, left, t - 1, floor((t - left - 1) / 2));
    // 找出当前选的中间位置
    int mid = WorstCasePartition(A, left, right, mid_of_mid);
    // 当前下标所排的位置
    int cur = mid - left + 1;
    // 三种情况分类讨论
    if (cur == k)
    {
        return A[mid];
    }
    else if (cur > k)
    {
        return WorstCaseSelect(A, left, mid - 1, k);
    }
    else
    {
        return WorstCaseSelect(A, mid + 1, right, k - cur);
    }   
}



int main(int argc, char const *argv[])
{
    vector<int> A;
    ReadData("无重复5千整数集范围是1-50000.txt", A);
    // ReadData("test.txt", A);
    cout << "最坏情况为线性时间的选择算法得出的第1000大的数为：" << WorstCaseSelect(A, 0, A.size() - 1, 4001) << endl;
    
    sort(A.begin(), A.end(), cmp);
    cout << "快速排序得出的第1000大个数为：" << A[4000] << endl;

    return 0;
}
