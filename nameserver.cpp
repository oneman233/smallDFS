//
// Created by oneman233 on 2023/2/22.
//

#include "nameserver.h"

[[maybe_unused]] NameServer::NameServer(int _numReplicated, int _strategy)
        : numReplicate(_numReplicated), idCounter(0), strategy(_strategy) {}

std::vector<std::string> NameServer::parseCmd() {
    std::cout << "smallDFS> ";
    std::string cmd, tmp;
    std::getline(std::cin, cmd);
    std::vector<std::string> params;
    std::stringstream ss(cmd);
    while (ss >> tmp)
        params.push_back(tmp);
    return params;
}

void NameServer::add(DataServer *server) { dataServers.push_back(server); }

void NameServer::operator()() {
    while (true) {
        std::vector<std::string> params = parseCmd();
        std::vector<int> idx;
        std::ifstream ifs;
        char *buf = nullptr;
        MD5 md5;

        // 命令为空
        if (params.empty()) {
            std::cerr << "Blank line" << std::endl;
            continue;
        }

        if (params[0] == "quit") {
            exit(0);
        } else if (params[0] == "list" || params[0] == "ls") {
            // 命令错误
            if (params.size() != 1) {
                std::cerr << "Usage: list/ls" << std::endl;
            } else {
                std::cout << "file/fileID/chunkNumber" << std::endl;
                fileTree.list(meta);
            }
        } else if (params[0] == "put") {
            // 命令错误
            if (params.size() != 3) {
                std::cerr << "Usage: put source_file_path des_file_path" << std::endl;
                continue;
            }
            ifs.open(params[1], std::ifstream::ate | std::ifstream::binary);

            if (!ifs) {
                // 打开文件失败
                std::cerr << "Open file error: local file " << params[1] << std::endl;
                continue;
            } else if (!fileTree.insertNode(params[2], true)) {
                // 远程文件已存在
                std::cerr << "Create file error (maybe remote file " << params[2] << " exists)" << std::endl;
                continue;
            } else {
                int totalSize = (int) ifs.tellg();
                buf = new char[totalSize];
                ifs.seekg(0, std::ifstream::beg);
                ifs.read(buf, totalSize);

                std::vector<double> serverSize;
                for (const auto &server: dataServers)
                    serverSize.push_back(server->getSize());

                // 三种负载均衡策略
                if (strategy == 1)
                    idx = ArgSort<double>(serverSize);
                else if (strategy == 2)
                    idx = HashSort<double>(serverSize);
                else if (strategy == 3)
                    idx = PlainSort<double>(serverSize);

                // 创建文件拷贝
                ++idCounter;
                for (int i = 0; i < numReplicate; ++i) {
                    std::unique_lock<std::mutex> ul(dataServers[idx[i]]->mtx);
                    meta[params[2]] = std::make_pair(idCounter, totalSize);
                    dataServers[idx[i]]->cmd = "put";
                    dataServers[idx[i]]->fid = idCounter;
                    dataServers[idx[i]]->bufSize = totalSize;
                    dataServers[idx[i]]->buf = buf;
                    dataServers[idx[i]]->finish = false;
                    ul.unlock();
                    dataServers[idx[i]]->cv.notify_all();
                }
            }
        } else if (params[0] == "read" || params[0] == "fetch") {
            // 命令错误
            if (params.size() != 3 && params.size() != 4) {
                if (params[0] == "read")
                    std::cerr << "Usage: read source_file_path dest_file_path" << std::endl;
                else if (params[0] == "fetch")
                    std::cerr << "Usage: fetch FileID Offset dest_file_path" << std::endl;
                continue;
            }

            // 找不到文件
            if (params[0] == "read" && meta.find(params[1]) == meta.end()) {
                std::cerr << "error: no such file in DFS" << std::endl;
                continue;
            }

            for (auto &server: dataServers) {
                std::unique_lock<std::mutex> ul(server->mtx);
                server->cmd = params[0];
                if (params[0] == "read") {
                    auto metaPair = meta[params[1]];
                    server->fid = metaPair.first;
                    server->bufSize = metaPair.second;
                } else if (params[0] == "fetch") {
                    server->fid = std::stoi(params[1]);
                    server->offset = std::stoi(params[2]);
                }
                server->finish = false;
                ul.unlock();
                server->cv.notify_all();
            }
        } else if (params[0] == "locate") {
            // 命令错误
            if (params.size() != 3) {
                std::cerr << "Usage: locate fileID Offset" << std::endl;
                continue;
            }

            for (auto &server: dataServers) {
                std::unique_lock<std::mutex> ul(server->mtx);
                server->cmd = "locate";
                server->fid = std::stoi(params[1]);
                server->offset = std::stoi(params[2]);
                server->finish = false;
                ul.unlock();
                server->cv.notify_all();
            }
        } else if (params[0] == "help") {
            std::cout <<
                      "list/ls: show all files in DFS\n" <<
                      "put:     upload file to DFS\n" <<
                      "read:    download file from DFS with name\n" <<
                      "fetch:   download file from DFS with ID and offset\n" <<
                      "locate:  locate file with ID and offset\n" <<
                      "quit:    close server\n";
        } else {
            std::cerr << "wrong command" << std::endl;
        }

        // 等待数据服务器完成处理
        for (const auto &server: dataServers) {
            std::unique_lock<std::mutex> ul(server->mtx);
            (server->cv).wait(ul, [&]() { return server->finish; });
            ul.unlock();
            (server->cv).notify_all();
        }

        // 数据服务器处理后的工作
        if (params[0] == "read" || params[0] == "fetch") {
            std::string checksum, preChecksum;
            for (auto &server: dataServers) {
                if (server->bufSize > 0) {
                    // 打开文件
                    std::ofstream ofs;
                    if (params[0] == "read") ofs.open(params[2]);
                    else if (params[0] == "fetch") ofs.open(params[3]);

                    // 下载文件并检查MD5
                    if (!ofs)
                        std::cerr << "Create file error" << std::endl;
                    else {
                        ofs.write(server->buf, server->bufSize);
                        ofs.close();
                        md5.update(server->buf, server->bufSize);
                        md5.finalize();
                        checksum = md5.toString();
                        if (!preChecksum.empty() && checksum != preChecksum)
                            std::cerr << "Unequal checksum for files from different dataServers" << std::endl;
                        preChecksum = checksum;
                    }

                    // 清空缓存
                    delete[] server->buf;
                }
            }
        } else if (params[0] == "put") {
            std::cout << "Successfully upload file ID " << idCounter << std::endl;
        } else if (params[0] == "locate") {
            bool notFound = true;
            for (auto &server: dataServers) {
                if (server->bufSize > 0) {
                    notFound = false;
                    std::cout << "found FileID " << params[1]
                              << " offset " << params[2]
                              << " at " << server->getName() << std::endl;
                }
            }
            if (notFound)
                std::cout << "not found FileID " << params[1]
                          << " offset " << params[2] << std::endl;
        }

        // 清理资源
        delete[] buf;
        ifs.close();
    }
}