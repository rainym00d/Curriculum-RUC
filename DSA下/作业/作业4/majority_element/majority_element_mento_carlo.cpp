/*
 * @Description: mento carlo算法解决主元素问题
 * @Author: rainym00d
 * @Github: https://github.com/rainym00d
 * @Date: 2020-10-05 15:24:44
 * @LastEditors: rainym00d
 * @LastEditTime: 2020-10-10 18:08:42
 */

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>

using namespace std;


bool Majority(int *t, int n) {
    // initialize random seed by time
    srand((unsigned)time(NULL));
    // choose i randomly
    int i = rand() % n;
    int x = t[i];
    int k = 0;
    // traverse array t to find x
    for(int j = 0; j < n; j ++) {
        if(t[j] == x) {
            k ++;
        }
    }
    // k > n/2 denote x is majority element
    return k > n/2;
}

// multiple call Majority
bool MajorityMC(int *t, int n, double e) {
    // depending epsilon to calculate k
    int k = (int)ceil(log(1/e)/log(2));
    for(int i = 0; i < k; i ++) {
        if(Majority(t, n)) {
            return true;
        }
    }
    return false;
}

int main(int argc, char const *argv[])
{
    int t[] = {1, 1, 1, 1, 1, 2, 3, 4, 5, 6, 1, 1, 7, 8, 9, 1, 1, 1};
    // set epsilon = 0.0001
    cout << (MajorityMC(t, sizeof(t)/sizeof(t[0]), 0.0001) ? "Find majority element!" : "Not find majority element!") << endl;
    
    return 0;
}
