// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "networkApi.h"
#include "networkUtils.h"
#include "utils/utils.h"
#include "messageReceiver.h"
#include <vector>
#include <mutex>

class Server
{
public:
    EXPORT Server(NetworkApi &networkApi, int port, FuncMessageReceiverCreator funcCreateMessageReceiver);

    EXPORT void startServer();
    virtual bool isRunning() = 0;

    inline bool hasErrors() { return errors.size() > 0; }
    inline NetworkError fetchError()
    {
        if (errors.size() > 0)
        {
            mutex.lock();
            NetworkError error = errors.at(0);
            errors.erase(errors.begin());
            mutex.unlock();
            return error;
        }
        NetworkError error;
        error.code = NetworkCode::NoError;
        error.message = std::string("No error");
        return error;
    }

protected:
    virtual void setupServer() = 0;
    virtual void cleanUp() = 0;

    std::vector<NetworkError> errors;

    NetworkApi *networkApi;
    int port;

    std::mutex mutex;
    FuncMessageReceiverCreator funcCreateMessageReceiver;
};
