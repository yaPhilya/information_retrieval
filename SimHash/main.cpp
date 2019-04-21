#include <iostream>
#include <vector>
#include <fstream>
#include <set>
#include <map>

using namespace std;

//вектор для всех хэшей
vector<unsigned long long> hashes;
//для каждой части храним список хэшей с такой же частью
map<long long, vector<int>> parts[4];
// вектор удалённых хэшей
vector<bool> deleted;

long long getPart(unsigned long long hash, int left, int right){
    // возвращает часть хэша в указанных границах, включая их
    unsigned long long res = hash >> left;
    unsigned long long tmp = ((long long)1 << (right - left + 1)) - 1;
    res = tmp & res;
    return (long long)res;
}

bool isSimilar(unsigned long long h1, unsigned long long h2){
    return __builtin_popcountll(h1 ^ h2) <= 3;
}

int findGroup(int pos){
    //возвращает размер группы pos-ого хэша и удаляет группу

    //разделим данный хэш на части
    unsigned long long hash = hashes[pos];
    long long tmp[4] = {
            getPart(hash, 0, 15),
            getPart(hash, 16, 31),
            getPart(hash, 32, 47),
            getPart(hash, 48, 64)
    };

    //все хэши, которые совпадают с данным хотя бы в одной части проверяем на близость, удаляя найденные
    int cnt = 0;
    for (int i = 0; i < 4; ++i) {
        vector<int> vec = parts[i][tmp[i]];
        for (int j = 0; j < vec.size(); ++j) {
            int ind = vec[j];
            if(!deleted[ind] && isSimilar(hash, hashes[ind])) {
                cnt += 1;
                deleted[ind] = true;
            }
        }
    }

    return cnt;
}

void doFast() {
    ifstream in;
    ofstream out;
    in.open("simhash_sorted.txt");
    out.open("output.txt");

    // считаем все хэши
    unsigned long long hash;
    while(in >> hash){
        hashes.push_back(hash);
        deleted.push_back(false);
    }

    // разобьём все хэши на 4 равные части
    for (int i = 0; i < hashes.size(); ++i) {
        long long tmp[4] = {
                getPart(hashes[i], 0, 15),
                getPart(hashes[i], 16, 31),
                getPart(hashes[i], 32, 47),
                getPart(hashes[i], 48, 64)
        };
        for (int j = 0; j < 4; ++j) {
            if(parts[j].count(tmp[j]) == 0) {
                parts[j][tmp[j]] = vector<int>();
            }
            parts[j][tmp[j]].push_back(i);
        }
    }

    //будем разделять на группы
    for (int i = 0; i < deleted.size(); ++i) {
        if (!deleted[i]) {
            out << findGroup(i) << endl;
        }
        if( i % 10000 == 0){
            cerr << i << endl;
        }
    }
}


int main() {
    doFast();
    return 0;
}