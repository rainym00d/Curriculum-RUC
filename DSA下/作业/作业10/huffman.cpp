#include <iostream>
#include <string>
using namespace std;

#define INFINITE 2333333

//树结点定义
typedef struct 
{
    int weight;
    int parent;
    int lchild;
    int rchild;
} HTNode, *HuffmanTree;

//封装最小权节点和次小权节点
typedef struct 
{
    int node_1;
    int node_2;
} MinCode;

typedef string *HuffmanCode;

string Str;
//输出错误信息的函数
void Error(string message) {
    cerr << message << endl;
    exit(1);
}

MinCode FindTwoMin(HuffmanTree HT, int n) {
    //遍历找到最小与次小
    int min_1, min_2;
    int index_1, index_2;
    MinCode min;
    min_1 = INFINITE, min_2 = INFINITE;
    for(int i = 1; i <= n; i ++) {
        if(HT[i].parent == 0 && HT[i].weight < min_1) {
            min_1 = HT[i].weight;
            index_1 = i;
        }
    } 
    for(int i = 1; i <= n; i ++) {
        if(HT[i].parent == 0 && i != index_1 && HT[i].weight < min_2) {
            min_2 = HT[i].weight;
            index_2 = i;
        }
    }
    min.node_1 = index_1;
    min.node_2 = index_2;
    //cout << index_1 << "\t" << index_2 << endl;
    return min;
}

HuffmanCode HuffmanCoding(HuffmanTree &HT, HuffmanCode HC, int *w, int n) {
    //输入少于一个字母的时候，不构造树，报错
    if(n <= 1) {
        Error("String is too short!");
    }
    HuffmanTree p;
    int i = 0;
    int m = 2 * n - 1;
    HT = new HTNode[m + 1];
    //初始化n个叶子结点
    for(p = HT; i <= n; i ++, p ++, w ++) {
        p->weight = *w;  
        p->parent = 0; 
        p->lchild = 0; 
        p->rchild = 0;
    }
    //初始化其他n-1个非叶子节点
    for(; i <= m; i ++, p ++) {
        p->weight = 0;  
        p->parent = 0; 
        p->lchild = 0; 
        p->rchild = 0;
    }
    //构造赫夫曼树
    MinCode min;
    int index_1, index_2;
    for(i = n + 1; i <= m; i ++) {
        min = FindTwoMin(HT, i - 1);
        index_1 = min.node_1, index_2 = min.node_2;
        HT[index_1].parent = i; 
        HT[index_2].parent = i;
        HT[i].lchild = index_1;
        HT[i].rchild = index_2;
        //给新的节点赋权，权值为左右两孩子权值之和
        HT[i].weight = HT[index_1].weight + HT[index_2].weight;
    }
/* 
    //用来输出赫夫曼树，debug时候用的
    for(int j = 0; j <= m; j ++) {
        cout << j << "\t";
        cout << HT[j].parent << "\t";
        cout << HT[j].lchild << "\t";
        cout << HT[j].rchild << "\t";
        cout << endl;
    }
*/
    //求每个字符的赫夫曼编码
    HC = new string[n + 1];
    for(i = 1; i <= n; i ++) {
        //j储存当前节点，k储存父节点
        int j, k;
        for(j = i, k = HT[i].parent; k != 0; j = k, k = HT[k].parent) {
            if(HT[k].lchild == j) {
                HC[i] = string("0") + HC[i];
            }
            else {
                HC[i] = string("1") + HC[i];
            }
        }
    }
    return HC;
}

string HuffmanEnCoding(HuffmanTree HT, string s, int n) {
    int m = 2 * n - 1;
    HuffmanCode p;
    //树的跟节点在最后一个数组
    int j = m;
    string result = "";
    for(int i = 0; i < s.length(); i ++) {
        //若为0，则往左搜索
        if(s[i] == '0') {
            j = HT[j].lchild;
            //无子节点，说明为叶子结点，输出该节点代表的字母
            if(HT[j].lchild == 0 && HT[j].rchild == 0) {
                result += Str[4 * (j - 1)];
                //回到跟节点，重置当前搜索的位置
                j = m;
            }
        }
        //若为1，则往右搜索
        else if(s[i] == '1') {
            j = HT[j].rchild;
            //无子节点，说明为叶子结点，输出该节点代表的字母
            if(HT[j].lchild == 0 && HT[j].rchild == 0) {
                result += Str[4 * (j - 1)];
                //回到跟节点，重置当前搜索的位置
                j = m;
            }
        }
    }
    return result;
}

int main() {   
    //输入字符串与权值
    cout << "Example Input: A,2, ,4,E,1,G,1,F,1,I,3,H,1,M,2,O,2,P,1,S,2,R,3,T,2,V,1,Y,1" << endl;
    cout << "Input:" << endl;
    getline(cin, Str);
    //cin >> Str;  
    int n = (Str.length() + 1) / 4;
    //cout << Str.length() << endl;
    //cout << n << endl;
    HuffmanTree HT;
    int *w;
    w = new int[n + 1];
    w[0] = 0;
    //cout << "input weight:" << endl;
    for(int i = 1; i <= n; i ++) {
        //cout << "weight[" << i << "]: ";
        //cin >> w[i];
        //cout << Str[4 * i - 2];
        w[i] = Str[4 * i - 2] - '0';
    }
    //构造树并编码
    HuffmanCode HC;
    HC = HuffmanCoding(HT, HC, w, n);
    //输出编码的信息
    cout << "Char\t" << "Code\t" << "Weight" << endl;
    for(int i = 1; i <= n; i ++) {
        cout << Str[4 * (i - 1)] << "\t" << HC[i] << "\t" << w[i] << endl;
    }
    //解码
    string s;
    cout << "input encoding string:" << endl;
    cin >> s;
    cout << "encodeing: " << HuffmanEnCoding(HT, s, n) << endl;

    return 0;
}