#include <iostream>
#include <vector>
#include <fstream>
#include <unordered_map>

using ui64 = unsigned long long;

std::vector<ui64> hashes;
std::unordered_map<ui64, std::vector<size_t>> parts[4];
std::vector<bool> used;

long long getPart(ui64 hash, int left, int right) {
    ui64 res = hash >> left;
    ui64 tmp = ((long long)1 << (right - left + 1)) - 1;
    res = tmp & res;
    return (long long)res;
}

std::vector<long long> getParts(ui64 hash) {
    return {getPart(hash, 0, 15), getPart(hash, 16, 31), getPart(hash, 32, 47), getPart(hash, 48, 64)};
}

bool isSimilar(ui64 h1, ui64 h2) {
    return __builtin_popcountll(h1 ^ h2) <= 3;
}

size_t findGroup(size_t pos) {
    ui64 hash = hashes[pos];
    auto tmp = getParts(hash);

    size_t cnt = 0;
    for (size_t i = 0; i < 4; ++i) {
        auto& vec = parts[i][tmp[i]];
        for (auto ind : vec) {
            if(!used[ind] && isSimilar(hash, hashes[ind])) {
                cnt += 1;
                used[ind] = true;
            }
        }
    }
    return cnt;
}


int main() {
    std::ifstream in("simhash_sorted.txt");
    std::ofstream out("group_sizes.txt");

    ui64 hash;
    while(in >> hash) {
        hashes.push_back(hash);
        used.push_back(false);
        
        auto tmp = getParts(hash);
        for (int j = 0; j < 4; ++j) {
            if(parts[j].count(tmp[j]) == 0) {
                parts[j][tmp[j]] = std::vector<size_t>();
            }
            parts[j][tmp[j]].push_back(hashes.size() - 1);
        }
    }

    for (auto i = 0; i < used.size(); ++i) {
        if (!used[i]) {
            out << findGroup(i) << std::endl;
        }
        if( i % 10000 == 0){
            std::cerr << i << std::endl;
        }
    }
    return 0;
}
