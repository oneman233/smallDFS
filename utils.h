//
// Created by oneman233 on 2023/2/21.
//

#ifndef SMALLDFS_UTILS_H
#define SMALLDFS_UTILS_H

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <array>
#include <iterator>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <random>

std::vector<std::string> Split(const std::string &str, const char &delim);

// 最小容量优先
template<typename T>
std::vector<int> ArgSort(const std::vector<T> &vec) {
    std::vector<int> idx;
    for (int i = 0; i < vec.size(); ++i)
        idx.push_back(i);
    sort(idx.begin(), idx.end(),
         [&vec](int x, int y) {
             return vec[x] < vec[y];
         });
    return idx;
}

// 随机值均分
template<typename T>
std::vector<int> HashSort(const std::vector<T> &vec) {
    std::vector<int> idx;
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<int> dis;
    for (int i = 0; i < vec.size(); ++i)
        idx.push_back(dis(gen) % vec.size());
    return idx;
}

// 顺序选取
template<typename T>
std::vector<int> PlainSort(const std::vector<T> &vec) {
    std::vector<int> idx;
    for (int i = 0; i < vec.size(); ++i)
        idx.push_back(i);
    return idx;
}

#define BLOCK_SIZE 64

// Reference: https://github.com/joyeecheung/md5
class MD5 {
public:
    MD5();

    MD5 &update(const unsigned char *in, size_t inputLen);

    MD5 &update(const char *in, size_t inputLen);

    MD5 &finalize();

    [[nodiscard]] std::string toString() const;

private:
    void init();

    void transform(const uint8_t block[BLOCK_SIZE]);

    uint8_t buffer[BLOCK_SIZE]{};  // buffer of the raw data
    uint8_t digest[16]{};  // result hash, little endian

    uint32_t state[4]{};  // state (ABCD)
    uint32_t lo{}, hi{};    // number of bits, modulo 2^64 (lsb first)
    bool finalized{};  // if the context has been finalized
};

std::string md5(const std::string &str);

#endif //SMALLDFS_UTILS_H
