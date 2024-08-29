// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"
#include "networkUtils.h"
#include "networkMessage.h"
#include <mutex>

class MessageReceiver
{
public:
    virtual void receiveMessage(NetworkApiCall code, int size, const char *body) = 0;

    EXPORT void sendRequest(NetworkApiCall apiCall, const void *data, unsigned int size);

    EXPORT NetworkMessage *getNetworkMessage();

    inline std::mutex *getMutex() { return &mutex; };
    inline bool isHasMessages()
    {
        mutex.lock();
        bool bHasMessages = messages.size() > 0;
        mutex.unlock();
        return bHasMessages;
    }

protected:
    std::mutex mutex;

private:
    std::vector<NetworkMessage *> messages;
};

typedef MessageReceiver *(*FuncMessageReceiverCreator)();