// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "networkUtils.h"
#include "utils/utils.h"
#include <vector>
#include <mutex>

struct NetworkApiDescriptor
{
    int nSize;
    bool bIsVolatile = false;
};

class NetworkApi
{
public:
    EXPORT NetworkApi(int nVersion);
    EXPORT NetworkApiCall addFixedSizeApiCall(int nSize);
    EXPORT NetworkApiCall addVolatileSizeApiCall();
    EXPORT NetworkApiDescriptor getDescriptor(int number);

    inline NetworkApiDescriptor *getNetworkApiDescriptor(NetworkApiCall apiCall)
    {
        if (apiCall >= 0 && apiCall < apis.size())
            return &apis.at(apiCall);
        return nullptr;
    }
    inline int getVersion() const { return nVersion; }

protected:
    std::vector<NetworkApiDescriptor> apis;

    NetworkApiCall sysInitialCall;
    NetworkApiCall sysCallWrongCall;
    NetworkApiCall sysCallWrongVersion;
    NetworkApiCall sysCallSuccess;

    int nVersion;

    std::mutex mutex;
};