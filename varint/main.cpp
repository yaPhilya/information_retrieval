#include <iostream>
#include <vector>
#include <fstream>
#include <set>
#include <map>
#include <random>
#include <chrono>
#include <thread>
using namespace std;

template<typename T>
std::vector<char> encode(const T* data, size_t count) {
    std::vector<char> res = std::vector<char>();

    for (size_t i = 0; i < count; ++i) {
        T num = data[i];

        while (num) {
            char c = 0;
            if (num >> 7) {
                c |= 1 << 7;
            }
            c |= num & ((1 << 7) - 1);
            res.push_back(c);

            num = num >> 7;
        }
    }
    return res;
}

template<typename T>
std::vector<T> decode(const char* data, size_t length){
    std::vector<T> res = std::vector<T>();

    size_t pos = 0;
    while (pos < length) {
        T num = 0;
        int pow = 0;
        do{
            num |= (T)(data[pos] & ((1 << 7) - 1)) << pow;
            pow += 7;
            pos++;
        }while(data[pos - 1] >> 7);

        res.push_back(num);
    }

    return res;
}

// testing with random "count" elements
template<typename T>
bool test(size_t count) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<T> dis;

    //генерируем данные
    T *data = (T *)malloc(sizeof(T) * count);
    for (size_t i = 0; i < count; ++i) {
        data[i] = dis(gen);
    }

    // кодируем их
    std::vector<char> encoded = encode<T>(data, count);

    char *tmp = (char *)malloc(sizeof(char) * encoded.size());
    for (size_t j = 0; j < encoded.size(); ++j) {
        tmp[j] = encoded[j];
    }

    // декодируем их
    std::vector<T> decoded = decode<T>(tmp, encoded.size());

    // теперь проверяем на корректность
    if (decoded.size() != count) {
        return false;
    }

    for (int k = 0; k < count; ++k) {
        if (decoded[k] != data[k]) {
            return false;
        }
    }
    return true;
}

int main() {
   if (test<int>(100000)) {
       printf("OK\n");
   }else {
       printf("WRONG\n");
   }

   if (test<long long>(100000)) {
       printf("OK\n");
   }else {
       printf("WRONG\n");
   }

    return 0;
}