// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "messageProcessor.h"

MessageProcessor::~MessageProcessor()
{
}

void MessageProcessor::doAction(NetworkActionCode actionCode, const char *data, unsigned int size)
{
}

void MessageProcessor::sendRequest(NetworkApiCall apiCall, const void *data, unsigned int size)
{
    NetworkMessage *answer = new NetworkMessage(NetworkMessageType::SendMessage, apiCall, (const char *)data, size);
    mutex.lock();
    messages.push_back(answer);
    mutex.unlock();
}

NetworkMessage *MessageProcessor::getNetworkMessage()
{
    mutex.lock();
    if (messages.size() > 0)
    {
        NetworkMessage *message = messages.at(0);
        messages.erase(messages.begin());
        mutex.unlock();
        return message;
    }
    mutex.unlock();
    return nullptr;
}