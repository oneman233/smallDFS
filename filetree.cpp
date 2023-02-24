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
        if (!node->isUploaded) {
            while (node && node->value != name)
                node = node->nextSibling;
            if (node == nullptr)
                return false;
            *parent = node;
            node = node->firstSon;
        } else {
            // 提升后的节点直接查找 map
            if (node->sonIdx.count(name) == 0)
                return false;
            *parent = node;
            node = node->sons[node->sonIdx[name]];
        }
    }
    return node->isFile;
}

bool FileTree::insertNode(const std::string &path, const bool isFile) const {
    TreeNode *parent = nullptr;
    bool isFound = findNode(path, &parent);
    if (isFound) return false;

    std::vector<std::string> pathFolders = Split(path, '/');
    auto *newNode = new TreeNode(path, isFile);

    // 新节点首先更新其 parent 指针
    newNode->parent = parent;

    // 根据 parent 是否提升来更新 parent
    if (!parent->isUploaded) {
        TreeNode *son = parent->firstSon;
        if (son == nullptr)
            parent->firstSon = newNode;
        else {
            while (son->nextSibling != nullptr)
                son = son->nextSibling;
            son->nextSibling = newNode;
        }
    } else {
        // 首先在子数组中插入新节点
        // 随后更新 map
        parent->sons.push_back(newNode);
        parent->sonIdx[path] = (int) parent->sons.size() - 1;
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