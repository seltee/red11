// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "networkMessage.h"

NetworkMessage::NetworkMessage(NetworkMessageType type, NetworkApiCall apiCall, const char *data, unsigned int size)
{
    this->type = type;
    this->apiCall = apiCall;
    this->size = (data && size) ? size : 0;

    this->nMessageFullSize = sizeof(NetworkMessageHeader) + this->size;
    this->fullMessageData = new char[this->nMessageFullSize];

    NetworkMessageHeader *header = reinterpret_cast<NetworkMessageHeader *>(this->fullMessageData);
    header->code = apiCall;
    header->size = this->size;

    if (data && header->size > 0)
    {
        char *body = reinterpret_cast<char *>(this->fullMessageData + sizeof(NetworkMessageHeader));
        memcpy(body, data, size);
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