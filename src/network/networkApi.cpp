// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "networkApi.h"

struct SysInitialCallData
{
    int nApiVersion;
};

NetworkApi::NetworkApi(int nVersion)
{
    this->nVersion = nVersion;
    sysInitialCall = addFixedSizeApiCall(sizeof(SysInitialCallData));
    sysCallWrongCall = addFixedSizeApiCall(0);
    sysCallWrongVersion = addFixedSizeApiCall(0);
    sysCallSuccess = addFixedSizeApiCall(0);
}

NetworkApiCall NetworkApi::addFixedSizeApiCall(int nSize)
{
    int index = apis.size();
    mutex.lock();
    NetworkApiDescriptor descriptor;
    descriptor.nSize = nSize;
    descriptor.bIsVolatile = true;
    apis.push_back(descriptor);
    mutex.unlock();
    return index;
}

NetworkApiCall NetworkApi::addVolatileSizeApiCall()
{
    int index = apis.size();
    mutex.lock();
    NetworkApiDescriptor descriptor;
    descriptor.nSize = 0;
    descriptor.bIsVolatile = true;
    apis.push_back(descriptor);
    mutex.unlock();
    return index;
}

NetworkApiDescriptor NetworkApi::getDescriptor(int number)
{
    mutex.lock();
    if (number >= 0 && number < apis.size())
    {
        NetworkApiDescriptor networkApiDescriptor = apis.at(number);
        mutex.unlock();
        return networkApiDescriptor;
    }
    mutex.unlock();

    NetworkApiDescriptor networkApiDescriptor;
    networkApiDescriptor.nSize = 0;
    return networkApiDescriptor;
}