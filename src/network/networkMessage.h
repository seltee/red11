// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "networkUtils.h"
#include "utils/utils.h"

class NetworkMessage
{
public:
    EXPORT NetworkMessage(NetworkMessageType type, NetworkApiCall apiCall, const char *data, unsigned int size);
    EXPORT NetworkMessage(NetworkMessageType type, NetworkApiCall apiCall);
    EXPORT ~NetworkMessage();

    inline void destroy() { delete this; }

    inline NetworkMessageType getType() { return type; }
    inline NetworkApiCall getApiCall() { return apiCall; }
    const char *getFullMessageData() { return fullMessageData; };
    const char *getData() { return fullMessageData + sizeof(NetworkMessageHeader); };
    const int getFullMessageSize() { return nMessageFullSize; };
    inline unsigned int getSize() { return size; };

protected:
    NetworkMessageType type;
    NetworkApiCall apiCall;
    char *fullMessageData;
    int nMessageFullSize;
    unsigned int size;
};