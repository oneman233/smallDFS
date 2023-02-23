//
// Created by oneman233 on 2023/2/21.
//

#ifndef SMALLDFS_DATASERVER_H
#define SMALLDFS_DATASERVER_H

#include <string>
#include <mutex>
#include <thread>
#include <sys/stat.h>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <condition_variable>
#include <utility>

extern int chunkSize;

class DataServer {
private:
    std::string name;
    double size{};

    void put() const;

    void read();

    void locate();

    void fetch();

public:
    std::mutex mtx;
    std::condition_variable cv;
    std::string cmd;
    int fid{}, bufSize{}, offset{};
    char *buf;
    bool finish;

    [[maybe_unused]] explicit DataServer(std::string _name);

    [[noreturn]] void operator()();

    [[maybe_unused]] [[maybe_unused]] [[nodiscard]] double getSize() const;

    [[nodiscard]] std::string getName() const;
};

#endif //SMALLDFS_DATASERVER_H
