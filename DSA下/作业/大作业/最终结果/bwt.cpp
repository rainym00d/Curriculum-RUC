/*
 * @Description: 基于FM-Index与倍增算法的精确字符串匹配，能够在短串的时间复杂度中得出结果
 * @Author: rainym00d
 * @Github: https://github.com/rainym00d
 * @Date: 2020-12-05 19:56:00
 * @LastEditors: rainym00d
 * @LastEditTime: 2020-12-15 22:05:33
 */
#include <cstdio>
#include <iostream>
#include <cstring>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <set>
#include <algorithm>
#include <chrono> 


using namespace std;
using namespace chrono;


// 限制上限的全局变量
const int MAX_LEN = 1000005;        // 存储字符串的最大长度
const int MAX_N = 1500;              // 存储字符串的最大数量
const int MAX_TRUE_LEN = 1000000;   // 存储字符串的实际最大使用长度
const int MAX_MATCH_LEN = 1000;     // 最大匹配字符串长度

// 作为计数器而存在的全局变量
int STR_NUM = 0;                    // 分段后字符串数量计数器

// 函数间共享的全局变量
char **S, **GLOBAL_F, **GLOBAL_L;   // 分别为: 存储读入字符串的二维数组，存储第一列字符串的二维数组，存储最后一列字符串的二维数组
int **SA;                           // 后缀排序结果二维数组，表示: 排在index的是value
map <char, int> gene2rank = {       // gene碱基字母映射到id
    {'A', 0}, {'C', 1}, 
    {'G', 2}, {'T', 3}
};
map <int, char> rank2gene  = {      // id映射到基因碱基字母
    {0, 'A'}, {1, 'C'}, 
    {2, 'G'}, {3, 'T'}
};
map <int, map <char, int>* > GLOBAL_C;                  // 全局C字典，记录第一次出现某字符的位置
map <int, map <int, vector <int> >* > GLOBAL_Occ_buf;   // 全局Occ_buff字典，记录在index之前各个字母一共出现多少次

// 存放搜索结果的全局变量
vector <pair<int, int> > pos_list;  // 存储匹配结果位置的数组，格式为 (分割后的小串编号，在小串中的位置)

// 在计算后缀时用到的超大数组
int *wa, *wb, *wv, *Ws;             // 后缀排序时用到的，不是很清楚


// 输出函数
/**
 * @description: 正常打印字符串，白色的
 * @param {string} msg: 想要输出的字符串
 * @param {bool} newline: 是否换行
 * @return {void}
 */
void PRINT_MSG(const string &msg, bool newline = true) {
    if (newline) cout << msg << endl;
    else cout << msg;
}


/**
 * @description: 打印[INFO]字符串，绿色的
 * @param {string} msg: 想要输出的字符串
 * @param {bool} newline: 是否换行
 * @return {void}
 */
void PRINT_INFO(const string &msg, bool newline = true) {
    if (newline) cout << "\033[32m[INFO] " << msg << "\033[0m" << endl;
    else cout << "\033[32m[INFO] " << msg << "\033[0m";
}

/**
 * @description: 打印[WARNING]字符串，黄色的
 * @param {string} msg: 想要输出的字符串
 * @param {bool} newline: 是否换行
 * @return {void}
 */
void PRINT_WARNING(const string &msg, bool newline = true) {
    if (newline) cout << "\033[33m[WARNING] " << msg << "\033[0m" << endl;
    else cout << "\033[33m[WARNING] " << msg << "\033[0m";
}

/**
 * @description: 打印[ERROR]字符串，红色的
 * @param {string} msg: 想要输出的字符串
 * @param {bool} newline: 是否换行
 * @return {void}
 */
void PRINT_ERROR(const string &msg, bool newline = true) {
    if (newline) cout << "\033[31m[ERROR] " << msg << "\033[0m" << endl;
    else cout << "\033[31m[ERROR] " << msg << "\033[0m";
}


// sa用到的比较函数函数
int cmp(int *r,int a,int b,int l)
{return r[a]==r[b]&&r[a+l]==r[b+l];}


// 排序位置用到的比较函数，从小到大
bool pos_cmp(pair <int, int> i, pair <int, int> j) {
    if (i.first != j.first) {
        return i.first < j.first;
    }
    return i.second < j.second;
}


/**
 * @description: 初始化所有的全局变量，因为很大，所以只能new，用堆空间
 * @param {void}
 * @return {void}
 */
void init() {
    // 初始化 S F L
    S = new char*[MAX_N];
    for (int i = 0; i < MAX_N; i ++) {
        S[i] = new char[MAX_LEN];
        memset(S[i], 0, MAX_LEN * sizeof(char));
    }
    GLOBAL_F = new char*[MAX_N];
    for (int i = 0; i < MAX_N; i ++) {
        GLOBAL_F[i] = new char[MAX_LEN];
        memset(GLOBAL_F[i], 0, MAX_LEN * sizeof(char));
    }
    GLOBAL_L = new char*[MAX_N];
    for (int i = 0; i < MAX_N; i ++) {
        GLOBAL_L[i] = new char[MAX_LEN];
        memset(GLOBAL_L[i], 0, MAX_LEN * sizeof(char));
    }
    // 初始化 sa
    SA = new int*[MAX_N];
    for (int i = 0; i < MAX_N; i ++) {
        SA[i] = new int[MAX_LEN];
        memset(SA[i], 0, MAX_LEN * sizeof(int));
    }
    // 初始化 C
    for (int i = 0; i < MAX_N; i ++) {
        map <char, int>* tmp = new map <char, int>;
        GLOBAL_C[i] = tmp;
    }
    // 初始化 Occ_buf
    for (int i = 0; i < MAX_N; i ++) {
        map <int, vector <int> >* tmp = new map <int, vector <int> >;
        GLOBAL_Occ_buf[i] = tmp;
    }
    // 初始化杂七杂八
    wa = new int[MAX_LEN];
    memset(wa, 0, MAX_LEN * sizeof(int));

    wb = new int[MAX_LEN];
    memset(wb, 0, MAX_LEN * sizeof(int));

    wv = new int[MAX_LEN];
    memset(wv, 0, MAX_LEN * sizeof(int));

    Ws = new int[MAX_LEN];
    memset(Ws, 0, MAX_LEN * sizeof(int));
}


/**
 * @description: 安全销毁所有全局变量
 * @param {void}
 * @return {void}
 */
void destory() {
    // 谨慎起见倒着释放内存
    // 释放乱七八糟
    delete [] wa;
    wa = NULL;

    delete [] wb;
    wb = NULL;

    delete [] wv;
    wv = NULL;

    delete [] Ws;
    Ws = NULL;
    // 释放 Occ_buf
    map <int, map <int, vector <int> >* > ::iterator multitr_1;
    map <int, vector <int> >* tmp_1 = NULL;
    for (multitr_1 = GLOBAL_Occ_buf.begin(); multitr_1 != GLOBAL_Occ_buf.end(); multitr_1 ++) {
        tmp_1 = multitr_1->second;
        delete tmp_1;
        tmp_1 = NULL;
    }
    // 释放 C
    map <int, map <char, int>* > ::iterator multitr_2;
    map <char, int>* tmp_2 = NULL;
    for (multitr_2 = GLOBAL_C.begin(); multitr_2 != GLOBAL_C.end(); multitr_2 ++) {
        tmp_2 = multitr_2->second;
        delete tmp_2;
        tmp_2 = NULL;
    }
    // 释放 SA
    for (int i = 0; i < MAX_N; i ++) {
        delete [] SA[i];
        SA[i] = NULL;
    }
    delete [] SA;
    SA = NULL;
    // 释放 L F S
    for (int i = 0; i < MAX_N; i ++) {
        delete [] GLOBAL_L[i];
        GLOBAL_L[i] = NULL;
    }
    delete [] GLOBAL_L;
    GLOBAL_L = NULL;

    for (int i = 0; i < MAX_N; i ++) {
        delete [] GLOBAL_F[i];
        GLOBAL_F[i] = NULL;
    }
    delete [] GLOBAL_F;
    GLOBAL_F = NULL;

    for (int i = 0; i < MAX_N; i ++) {
        delete [] S[i];
        S[i] = NULL;
    }
    delete [] S;
    S = NULL;
}


/**
 * @description: 逐个字符读入文件，并根据大小分割成较小的小串
 * @param {string} file: 读入文件的文件名
 * @return {void}
 */
void ReadFile(string file) {
    int count = 0;
    ifstream infile;
    infile.open(file);
    if(infile.is_open()) {
        char c;
        while((c = infile.get()) && c != EOF) { 
            S[STR_NUM][count] = c;
            count ++;
            if (count == MAX_TRUE_LEN) {
                count = 0;
                STR_NUM ++;
            }
        }
        if (count) {
            STR_NUM ++;
        }
    }
    // 也许会加上打开失败
    // else {
        
    // }
    infile.close();
}


/**
 * @description: 将结果文件写入文件file
 * @param {string} file: 默认写入 result.txt
 * @return {void}
 */
void WriteResultToFile(string file = "result.txt") {
    ofstream outfile;
    outfile.open("result.txt");
    if (outfile.is_open()) {
        // 先排序
        sort(pos_list.begin(), pos_list.end(), pos_cmp);
        // 写入文件
        for (auto pos : pos_list) {
            outfile << pos.first * MAX_TRUE_LEN + pos.second << endl;
        }
    }
    // 也许会加上打开失败
    // else {
        
    // }
    outfile.close();
}


/**
 * @description: 计算后缀排序
 * @param {int} idx: 选择后缀排序的小串编号
 * @return {void}
 */
void CalSA(int idx)
{
    // 变量初始化
    char *r = S[idx];
    int *sa = SA[idx];
    int m = 512, n = strlen(r);
    r[n] = '$';
    n ++;
    memset(wa, 0, MAX_LEN * sizeof(int));
    memset(wb, 0, MAX_LEN * sizeof(int));
    memset(wv, 0, MAX_LEN * sizeof(int));
    memset(Ws, 0, MAX_LEN * sizeof(int));

    int i,j,p,*x=wa,*y=wb,*t; 
    for(i=0; i<m; i++) Ws[i]=0;
    for(i=0; i<n; i++) Ws[x[i]=r[i]]++;//以字符的ascii码为下标 
    for(i=1; i<m; i++) Ws[i]+=Ws[i-1];
    for(i=n-1; i>=0; i--) sa[--Ws[x[i]]]=i;
    for(j=1,p=1; p<n; j*=2,m=p)
    {
        for(p=0,i=n-j; i<n; i++) y[p++]=i; 
        for(i=0; i<n; i++) if(sa[i]>=j) y[p++]=sa[i]-j;
        for(i=0; i<n; i++) wv[i]=x[y[i]];
        for(i=0; i<m; i++) Ws[i]=0;
        for(i=0; i<n; i++) Ws[wv[i]]++;
        for(i=1; i<m; i++) Ws[i]+=Ws[i-1];
        for(i=n-1; i>=0; i--) sa[--Ws[wv[i]]]=y[i];
        for(t=x,x=y,y=t,p=1,x[sa[0]]=0,i=1; i<n; i++)
                x[sa[i]]=cmp(y,sa[i-1],sa[i],j)?p-1:p++;
    }
    return;
}


/**
 * @description: BWT编码
 * @param {int} idx: 选择编码的小串编号
 * @return {void}
 */
void BWTEncode(int idx) {
    // 后缀排序
    CalSA(idx);
    
    // 初始化一些变量
    char *F = GLOBAL_F[idx];
    char *L = GLOBAL_L[idx];
    map <char, int>* C = GLOBAL_C[idx];
    map <int, vector <int> >* Occ_buf = GLOBAL_Occ_buf[idx];
    int n = strlen(S[idx]);
    char *r = S[idx];
    int *sa = SA[idx];

    // 生成F和L字符串
    for (int i = 0; i < n; ++ i) {
        F[i] = r[sa[i]];
        if (sa[i] == 0) {
            L[i] = '$';
        }
        else {
            L[i] = r[sa[i]-1];
        }
    }
    
    // 生成C数组，基因组特化
    int count = 0;
    int j = 0;
    char gene[5] = "ACGT";
    for (int i = 1; i < n; ++ i) {
        if (F[i] == gene[j]) {
            C->insert(pair<char, int>(gene[j], count));
            ++ j;
            ++ count;
            continue;
        }
        ++ count;
    }
    // 生成Occ_buffer，按照论文，生成间隔为448行
    vector <int> count_list(4);
    int step = 448;
    for (int i = 0; i < n; ++ i) {
        if (L[i] == '$') {
            continue;
        }
        ++ count_list[gene2rank[L[i]]];
        if (i % step == 0) {
            Occ_buf->insert(pair<int, vector<int> >(i, count_list));
        }
    }
    // 生成SA，按照论文，生成的间隔为36行
    // 很尴尬第一步直接全部生成了
}


/**
 * @description: 得到小串idx编码后的L字符串中，第row行的c字符出现的次数
 * @param {int} idx: 选择小串的编号
 * @param {char} c: 想要找的字符c
 * @param {int} row: 行号
 * @return {int}
 */
int Occ(int idx, char c, int row) {
    char *L = GLOBAL_L[idx];
    map <int, vector <int> >* Occ_buf = GLOBAL_Occ_buf[idx];
    int step = 448;
    int check_point = int(row / step) * step;
    int count = Occ_buf->at(check_point)[gene2rank[c]];
    for (int i = check_point + 1; i <= row; ++ i) {
        if (L[i] == c) {
            ++ count;
        }
    }
    return count;
}


/**
 * @description: 在小串idx里边精确匹配P字符串
 * @param {int} idx: 选择小串的编号
 * @param {string} P: 要匹配的字符串
 * @return {void}
 */
void ExactMatch(int idx, string P) {
    // 变量初始化
    int *sa = SA[idx];
    char *F = GLOBAL_F[idx];
    int n = strlen(S[idx]);
    map <char, int>* C = GLOBAL_C[idx];
    
    int i = P.length() - 1;
    char c = P[i];
    int sp = C->at(c) + 1;
    int ep;
    if (c == F[n - 1]) {
        ep =  n - 1;
    }
    else {
        ep = C->at(rank2gene[gene2rank[c] + 1]);
    }

    while (sp <= ep && i >= 1) {
        c = P[i - 1];
        sp = C->at(c) + Occ(idx, c, sp - 1) + 1;
        ep = C->at(c) + Occ(idx, c, ep);
        -- i;
    }

    for (int j = ep; j > sp - 1; -- j) {
        // cout << j << sa[j] << endl;
        pos_list.push_back(pair<int, int>(idx, sa[j]));
    }
    return;
}


/**
 * @description: 在小串idx_l和idx_r的连接出匹配字符串P
 * @param {int} idx_l: 左边的小串编号
 * @param {int} idx_r: 右边的小串编号
 * @param {string} P: 要匹配的字符串
 * @return {void}
 */
void JointMatch(int idx_l, int idx_r, string P) {
    // 构造连接后的字符串
    int len_P = P.size(), len_l = strlen(S[idx_l]) - 1, len_r = strlen(S[idx_r]) - 1;   //  不计算最后一个 $
    int offset = len_l - len_P + 1;
    string s_l(S[idx_l] + offset, len_P - 1);
    string s_r;
    if (len_r < len_P) {
        s_r = string(S[idx_r], len_r);
    }
    else {
        s_r = string(S[idx_r], len_P - 1);
    }
    string s_joint = s_l + s_r;
    // 用 STL 的内置函数循环找子串，因为连接后的字符串的长度为 2 * len_P，因此寻找的复杂度不会很高
    size_t pos = s_joint.find(P, 0);
    while (pos != s_joint.npos) {
        pos_list.push_back(pair<int, int>(idx_l, pos + offset));
        pos = s_joint.find(P, pos + 1);
    }
    return;
}


int main(int argc, char const *argv[])
{
    // 初始化变量
    init();

    // 读入文件名
    string file;
    while(file.empty()) {
        PRINT_INFO("Please Input File Name: ", false);
        cin >> file;
    }
    
    // 读入文件
    PRINT_INFO("Load Start!");
    auto start = system_clock::now();
    ReadFile(file);     // 读入
    auto end = system_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    cout <<  "花费了" 
        << double(duration.count()) * microseconds::period::num / microseconds::period::den 
        << "秒" << endl;
    PRINT_INFO("Load Finished!");

    // BWTEncode
    PRINT_INFO("BWTEncode Start!");
    start = system_clock::now();
    for (int i = 0; i < STR_NUM; i ++) {
        BWTEncode(i);   // 给各个小串编码
    }
    end = system_clock::now();
    duration = duration_cast<microseconds>(end - start);
    cout <<  "花费了" 
        << double(duration.count()) * microseconds::period::num / microseconds::period::den 
        << "秒" << endl;
    PRINT_INFO("BWTEncode Finished!");

    // 精确匹配字符串
    string P;
    while (true) {
        // 输入字符串模式
        PRINT_INFO("Please Input String (Max 1k): ", false);
        cin >> P;

        // 输入文件名模式
        // string file;
        // PRINT_INFO("Please Input File: ", false);
        // cin >> file;
        // ifstream infile;
        // infile.open(file);
        // if(infile.is_open()) {
        //     infile >> P;
        //     infile.close();
        // }
        // else {
        //     PRINT_ERROR("Open File Error!");
        //     continue;
        // }

        // 检查输入的内容长度
        if (P.size() > MAX_MATCH_LEN) {
            PRINT_ERROR("Content is too long!");
        }
        // 将输入的内容变为大写
        transform(P.begin(), P.end(), P.begin(), ::toupper);
        // 判断是否退出
        if (P == "EXIT") {
            cout << "[INFO]Exit System!" << endl;
            break;
        }
        // 判断输入的字符是否不合法
        set <int> skip_set;                         // skip_set，里边记录 P 绝对不会出现的小串编号
        set <char> char_s(P.begin(), P.end());      // 字符串集合，可能这样子不是很快，可以再改
        for (int i = 0; i < STR_NUM; i ++) {
            for (auto c : char_s) {
                if (GLOBAL_C[i]->find(c) == GLOBAL_C[i]->end()) {
                    // 如果 C 中找不到 P 中的某个字符，这说明该小串中绝对不会出现 P，于是加入skip_list。但反之不成立。
                    skip_set.insert(i);
                    break;
                }
            }
        }
        // 清空上一轮的 pos_list 结果并释放内存
        vector <pair<int, int> >().swap(pos_list);
        // Matching
        PRINT_INFO("Start Matching...");
        start = system_clock::now();
        // ExcatMatch
        for (int i = 0; i < STR_NUM; i ++) {
            // 判断该小串是否绝对匹配不到 P
            if (skip_set.find(i) == skip_set.end()) {
                ExactMatch(i, P);   // 在各个小串里搜索
            }
        }
        // JointMatch
        for (int i = 1; i < STR_NUM; i ++) {
            JointMatch(i - 1, i, P);
        }
        end = system_clock::now();
        duration = duration_cast<microseconds>(end - start);
        PRINT_INFO(string("Match Finished! Total Result: ") + to_string(pos_list.size()));
        cout <<  "花费了" 
            << double(duration.count()) * microseconds::period::num / microseconds::period::den 
            << "秒" << endl;
        // 将结果写入文件
        PRINT_WARNING("Writing result to file, please not turn off the power...");
        WriteResultToFile();
        PRINT_INFO("Write Finished!");
    }
    PRINT_WARNING("Exiting, please not turn off the power...");
    // 销毁变量
    destory();
    PRINT_INFO("Exit Successfully!");
    return 0;
}