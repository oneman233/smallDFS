//
// Created by oneman233 on 2023/2/21.
//

#ifndef SMALLDFS_FILETREE_H
#define SMALLDFS_FILETREE_H

#include <string>
#include <map>
#include <iostream>
#include <cmath>
#include <utility>
#include "utils.h"

struct TreeNode {
    std::string value;
    bool isFile;
    TreeNode *parent, *firstSon, *nextSibling;

    TreeNode(std::string _value, const bool _isFile)
            : value(std::move(_value)),
              isFile(_isFile),
              parent(nullptr),
              firstSon(nullptr),
              nextSibling(nullptr) {}
};

class FileTree {
private:
    TreeNode *root;
public:
    FileTree();

    [[nodiscard]] bool insertNode(const std::string &path, bool isFile) const;

    bool findNode(const std::string &path, TreeNode **parent) const;

    void list(TreeNode *node, std::map<std::string, std::pair<int, int>> &meta);

    void list(std::map<std::string, std::pair<int, int>> &meta);
};

#endif //SMALLDFS_FILETREE_H
