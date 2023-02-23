//
// Created by oneman233 on 2023/2/21.
//

#include "filetree.h"

FileTree::FileTree() {
    root = new TreeNode("/", false);
}

bool FileTree::findNode(const std::string &path, TreeNode **parent) const {
    std::vector<std::string> pathFolders = Split(path, '/');
    TreeNode *node = root->firstSon;
    *parent = root;

    for (const auto &name: pathFolders) {
        while (node && node->value != name)
            node = node->nextSibling;
        if (!node)
            return false;
        *parent = node;
        node = node->firstSon;
    }
    return node->isFile;
}

bool FileTree::insertNode(const std::string &path, const bool isFile) const {
    TreeNode *parent = nullptr;
    bool isFound = findNode(path, &parent);
    if (isFound) return false;

    std::vector<std::string> pathFolders = Split(path, '/');
    auto *newNode = new TreeNode(path, isFile);
    newNode->parent = parent;

    TreeNode *son = parent->firstSon;
    if (son == nullptr)
        parent->firstSon = newNode;
    else {
        while (son->nextSibling != nullptr)
            son = son->nextSibling;
        son->nextSibling = newNode;
    }

    return true;
}

void FileTree::list(TreeNode *node, std::map<std::string, std::pair<int, int>> &meta) {
    static int chunkSize = 2 * 1024 * 1024;
    if (node != nullptr) {
        std::cout << node->value << "\t" << meta[node->value].first << "\t"
                  << (int) ceil(1.0 * meta[node->value].second / chunkSize) << std::endl;
        list(node->firstSon, meta);
        list(node->nextSibling, meta);
    }
}

void FileTree::list(std::map<std::string, std::pair<int, int>> &meta) {
    list(root, meta);
}