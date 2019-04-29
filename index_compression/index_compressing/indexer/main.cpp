#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <inttypes.h>
#include <unordered_map>
#include <algorithm>
#include <utility>
#include <exception>
#include <stdexcept>
#include <ctime>


template<typename T>
std::vector<char> encode(const T* data, size_t count) {
    std::vector<char> result = std::vector<char>();

    for (auto i = 0; i < count; ++i) {
        T number = data[i];
        while (number) {
            char c = 0;
            if (number >> 7) {
                c |= 1 << 7;
            }
            c |= number & ((1 << 7) - 1);
            result.push_back(c);

            number = number >> 7;
        }
    }
    return result;
}


int main(void) {
    std::unordered_map<std::string, std::vector<uint32_t>> index;
    std::unordered_map<std::string, std::vector<char>> delta;
    std::unordered_map<uint32_t, std::string> docid_to_url;

    uint32_t docid = 0;
    std::string url;
    auto start_time = std::time(nullptr);
    std::cout << std::asctime(std::localtime(&start_time)) << start_time << " seconds since the Epoch" << std::endl;
    std::cout << "Start reading input " << std::time(nullptr) - start_time << std::endl;
    while (std::cin >> url) {
        size_t word_count = 0;
        std::cin >> word_count;

        ++docid;
        docid_to_url[docid] = url;

        for (size_t i = 0; i < word_count; ++i) {
            std::string word;
            std::cin >> word;

            index[word].push_back(docid);
        }
    }
    std::cout << "End reading input " << std::time(nullptr) - start_time << std::endl;
    std::cout << "Start processing postings " << std::time(nullptr) - start_time  << std::endl;
    {
        std::ofstream output("compression_2");
        for (auto& p : index) {
            std::vector<uint32_t>& posting = p.second;

            std::sort(posting.begin(), posting.end());
            posting.erase(std::unique(posting.begin(), posting.end()), posting.end());
            std::vector<uint32_t> tmp(posting.size());
            tmp[0] = posting[0];
            for (auto i = 1; i < posting.size(); ++i) {
                tmp[i] = posting[i] - posting[i-1];
            }
            delta[p.first] = encode<uint32_t>(tmp.data(), posting.size());
            double compression = (double)(sizeof(uint32_t) * posting.size()) / (double)(sizeof(char) * delta[p.first].size());
            output << p.first << '\t' << compression << '\t' << posting.size() << std::endl;
        }
    }
    std::cout << "End processing postings " << std::time(nullptr) - start_time << std::endl;

    std::unordered_map<std::string, std::pair<uint32_t, uint32_t>> word_to_offset_and_length;

    std::cout << "Start saving doc to url " << std::time(nullptr) - start_time << std::endl;

    {
        std::ofstream output("docid_to_url_2");
        for (const auto& p : docid_to_url) {
            const uint32_t docid = p.first;
            const std::string& url = p.second;

            output << docid << '\t' << url << '\n';
        }
    }
    std::cout << "End saving doc to url " << std::time(nullptr) - start_time << std::endl;
    std::cout << "Start saving index " << std::time(nullptr) - start_time << std::endl;
    {
        std::ofstream output("index_2", std::ios::binary);
        for (const auto& p : delta) {
            const std::string& word = p.first;
            const std::vector<char>& posting = p.second;

            const uint32_t current_position = output.tellp();

            output.write(reinterpret_cast<const char*>(posting.data()),
                         posting.size() * sizeof(char));

            const uint32_t next_position = output.tellp();

            const uint32_t length = next_position - current_position;

            if (length != posting.size() * sizeof(char)) {
                throw std::logic_error("error while writing index");
            }

            word_to_offset_and_length[word] =
                    std::make_pair(current_position, length);
        }
    }
    std::cout << "End saving index " << std::time(nullptr) - start_time << std::endl;
    std::cout << "Start saving word ro offset " << std::time(nullptr) - start_time << std::endl;
    {
        std::ofstream output("word_to_offset_and_length_2");
        for (const auto& p : word_to_offset_and_length) {
            const std::string& word = p.first;
            const std::pair<uint32_t, uint32_t>& offset_and_length = p.second;

            output << word << '\t' <<
                      offset_and_length.first << '\t' <<
                      offset_and_length.second << '\n';
        }
    }
    std::cout << "End saving word ro offset " << std::time(nullptr) - start_time << std::endl;

    return 0;
}
