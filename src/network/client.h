// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "networkApi.h"
#include "networkUtils.h"
#include "networkMessage.h"
#include "messageProcessor.h"
#include "utils/utils.h"
#include <vector>
#include <mutex>

class Client
{
public:
    EXPORT Client(NetworkApi &networkApi, MessageProcessor &messageProcessor, const std::string &address, int port);
    EXPORT virtual ~Client();

    EXPORT void request(NetworkApiCall apiCall, const void *data, unsigned int size);

    EXPORT void connectToServer();

    virtual bool isConnected() = 0;

    inline const std::string &getAddress() const { return address; }
    inline const NetworkApi *getApi() const { return networkApi; }
    inline int getPort() const { return port; }
    inline int getQueueSize()
    {
        mutex.lock();
        int size = requests.size();
        mutex.unlock();
        return size;
    }
    inline bool hasErrors() { return errors.size() > 0; }
    inline NetworkError fetchError()
    {
        if (errors.size() > 0)
        {
            NetworkError error = errors.at(0);
            errors.erase(errors.begin());
            return error;
        }
        NetworkError error;
        error.code = NetworkCode::NoError;
        error.message = std::string("No error");
        return error;
    }

protected:
    virtual void runSendingReceivingTask() = 0;
    virtual void cleanUp() = 0;

    NetworkApi *networkApi;
    std::string address;
    int port;

    std::vector<NetworkMessage *> requests;
    std::vector<NetworkError> errors;

    std::mutex mutex;
    bool bServiceIsRunning = false;

    MessageProcessor *messageProcessor;
};