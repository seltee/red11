// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "networkMessage.h"

NetworkMessage::NetworkMessage(NetworkMessageType type, NetworkApiCall apiCall, const char *data, unsigned int size)
{
    this->type = type;
    this->apiCall = apiCall;
    if (data && size > 0)
    {
        this->nMessageFullSize = sizeof(NetworkMessageHeader) + size;
        this->fullMessageData = new char[this->nMessageFullSize];
        NetworkMessageHeader *header = reinterpret_cast<NetworkMessageHeader *>(this->fullMessageData);
        char *body = reinterpret_cast<char *>(this->fullMessageData + sizeof(NetworkMessageHeader));

        header->code = apiCall;
        header->size = size;

        memcpy(body, data, size);
    }
    else
    {
        this->fullMessageData = nullptr;
        this->size = 0;
    }
}

NetworkMessage::NetworkMessage(NetworkMessageType type, NetworkApiCall apiCall)
{
    this->type = type;
    this->apiCall = apiCall;
    this->fullMessageData = nullptr;
    this->nMessageFullSize = sizeof(NetworkMessageHeader);
    this->size = 0;
}

NetworkMessage::~NetworkMessage()
{
    if (fullMessageData)
        delete[] fullMessageData;
}