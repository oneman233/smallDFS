//
// Created by oneman233 on 2023/2/21.
//

#ifndef SMALLDFS_NAMESERVER_H
#define SMALLDFS_NAMESERVER_H

#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include "dataserver.h"
#include "filetree.h"

class NameServer {
private:
    std::vector<DataServer *> dataServers;
    FileTree fileTree;
    int numReplicate;
    int idCounter;
    int strategy{};

    static std::vector<std::string> parseCmd();

public:
    std::map<std::string, std::pair<int, int>> meta;

    [[maybe_unused]] explicit NameServer(int _numReplicated, int _strategy = 1);

    void add(DataServer *server);

    void operator()();
};

#endif //SMALLDFS_NAMESERVER_H
