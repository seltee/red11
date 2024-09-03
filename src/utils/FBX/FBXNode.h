// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"
#include <vector>
#include <stdio.h>

struct FBXNodeDataBinding
{
    char type;
    void *data;
    unsigned int numElements;
};

struct FBXNodeRecordHeader
{
    unsigned int endOffset;
    unsigned int numProperties;
    unsigned int propertyListLen;
};

class FBXNode
{
public:
    EXPORT FBXNode(int level, FILE *file);
    EXPORT ~FBXNode();

    EXPORT void process();
    EXPORT void readHead();
    EXPORT void readValues();
    EXPORT void print();
    EXPORT bool isName(const char *name);
    EXPORT FBXNode *findNode(const char *name);
    EXPORT bool doSkip(const char *name);

    std::vector<FBXNode *> children;

    unsigned int endOffset = 0;
    unsigned int numProperties = 0;
    unsigned int propertyListLen = 0;
    char *name = nullptr;
    bool bIsZero = false;
    bool bMarkedToRemove = false;
    int level = 0;
    FILE *file = nullptr;
    std::vector<FBXNodeDataBinding> bindedData;
};
