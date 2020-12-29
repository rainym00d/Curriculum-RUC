/*
 * @Description: 
 * @Author: rainym00d
 * @Github: https://github.com/rainym00d
 * @Date: 2020-11-27 12:20:03
 * @LastEditors: rainym00d
 * @LastEditTime: 2020-11-27 13:52:59
 */


#include "Fibonacci.h"

using namespace std;


int main(int argc, char const *argv[])
{
    Fibonacci_Heap <int> A;
    Fibonacci_Heap <int> B;
    Fibonacci_Heap <int> H;
    for (int i = 0; i < 5; i ++) {
        A.Insert(2 * i);
        B.Insert(2 * i + 1);
    }
    H.HeapUnion(A, B);
    for (int i = 0; i < 9; i ++) {
        cout << H.ExtractMin() << endl;
    }
    return 0;
}

