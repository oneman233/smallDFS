//
// Created by oneman233 on 2023/2/21.
//

#include "dataserver.h"

int chunkSize = 2 * 1024 * 1024;

[[maybe_unused]] DataServer::DataServer(std::string _name)
        : name(std::move(_name)), buf(nullptr), finish(true) {
    //    Use on Linux
    //    std::string tmp = "mkdir -p " + name;
    std::string tmp = "mkdir " + name;
    system(tmp.c_str());
}

[[noreturn]] void DataServer::operator()() {
    while (true) {
        std::unique_lock<std::mutex> ul(mtx);
        cv.wait(ul, [&]() { return !this->finish; });
        if (cmd == "put") {
            size += bufSize / 1024.0 / 1024.0;
            put();
        } else if (cmd == "read") {
            read();
        } else if (cmd == "locate") {
            locate();
        } else if (cmd == "fetch") {
            fetch();
        }
        this->finish = true;
        ul.unlock();
        cv.notify_all();
    }
}

void DataServer::put() const {
    int start = 0;
    std::ofstream os;
    while (start < bufSize) {
        int tmpOffset = start / chunkSize;
        std::string filePath = name + "/" + std::to_string(fid) + " " + std::to_string(tmpOffset);
        os.open(filePath);
        if (!os)
            std::cerr << "create file error in dataserver: (file name) " << filePath << std::endl;
        os.write(&buf[start], std::min(chunkSize, bufSize - start));
        start += chunkSize;
        os.close();
    }
}

void DataServer::read() {
    int start = 0;
    buf = new char[bufSize];
    while (start < bufSize) {
        int tmpOffset = start / chunkSize;
        std::string filePath = name + "/" + std::to_string(fid) + " " + std::to_string(tmpOffset);
        std::ifstream is(filePath);
        // file found not in this server.
        if (!is) {
            delete[]buf;
            bufSize = 0;
            break;
        }
        is.read(&buf[start], std::min(chunkSize, bufSize - start));
        start += chunkSize;
    }
}

void DataServer::fetch() {
    buf = new char[chunkSize];
    std::string filePath = name + "/" + std::to_string(fid) + " " + std::to_string(offset);
    std::ifstream is(filePath);
    // 服务器上未找到文件
    if (!is) {
        delete[]buf;
        bufSize = 0;
    } else {
        is.read(buf, std::min(chunkSize, bufSize - chunkSize * offset));
        bufSize = (int) is.tellg();
    }
}


void DataServer::locate() {
    std::string filePath = name + "/" + std::to_string(fid) + " " + std::to_string(offset);
    std::ifstream is(filePath);
    if (is)
        bufSize = 1;
    else
        bufSize = 0;
}

std::string DataServer::getName() const { return name; }

[[maybe_unused]] double DataServer::getSize() const { return size; }