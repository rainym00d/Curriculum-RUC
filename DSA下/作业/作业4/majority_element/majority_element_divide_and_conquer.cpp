/*
 * @Description: 分治法解决主元素问题
 * @Author: rainym00d
 * @Github: https://github.com/rainym00d
 * @Date: 2020-10-05 15:24:42
 * @LastEditors: rainym00d
 * @LastEditTime: 2020-10-10 18:06:25
 */

/* 主要策略
** 1. 如果左右都存在主元素
**    a. 如果相等，则返回其中任意一个
**    b. 若不相等，则检查它们两个是否左右合并后的主元素，返回是的那个，若不是则返回空
** 2. 如果只有一侧存在主元素，则检查它是否是左右合并后的主元素，若是则返回，若不是则返回空
** 3. 如果不存在主元素，则返回空
*/  

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>

using namespace std;


int t[] = {1, 1, 1, 2, 3, 3, 3, 3, 3};


bool Check(int left, int right, int num) {
    int count = 0;
    for(int i = left; i <= right; i ++) {
        if(t[i] == num) {
            count ++;
        }
    }
    if(count > ((right - left + 1) >>1)) {
        return true;
    }
    return false;
}

int Majority(int left, int right) {
    if(left == right) {
        return t[left];
    }
    int mid = (left + right) >> 1;
    int left_majority = Majority(left, mid);
    int right_majority = Majority(mid+1, right);
    // if left and right majority element exist
    if(left_majority != -1 && right_majority != -1) {
        // if left == right
        if(left_majority == right_majority) {
            return left_majority;
        }
        // if left != right
        if(Check(left, right, left_majority)) {
            return left_majority;
        }
        if(Check(left, right, right_majority)) {
            return right_majority;
        }
        return -1;
    }
    // if only left majority exists
    else if(left_majority != -1) {
        if(Check(left, right, left_majority)) {
            return left_majority;
        }
        return -1;
    }
    // if only right majority exists
    else if(right_majority != -1) {
        if(Check(left, right, right_majority)) {
            return right_majority;
        }
        return -1;
    }
    return -1;
}


int main(int argc, char const *argv[])
{
    cout << (Majority(0, 8) ? "Find majority element!" : "Not find majority element!") << endl;
    
    return 0;
}
