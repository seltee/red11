// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "networkApi.h"
#include "networkUtils.h"
#include "utils/utils.h"
#include "messageProcessor.h"
#include "connection.h"
#include <vector>
#include <mutex>

typedef bool (*IsQualifyForBroadcast)(Connection *connection);

class Server
{
public:
    EXPORT Server(NetworkApi &networkApi, int port, FuncMessageProcessorCreator funcCreateMessageProcessor);

    EXPORT void startServer();
    virtual bool isRunning() = 0;

    EXPORT void broadcast(NetworkActionCode actionCode, const char *data, unsigned int size, IsQualifyForBroadcast checkIfQualify = nullptr);
    EXPORT void filterDisconnected();

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

    inline int getConnectionsAmount()
    {
        return connections.size();
    }
    inline const std::vector<Connection *> *getConnectionsList()
    {
        return &connections;
    };

    EXPORT void lockConnections();
    EXPORT void unlockConnections();

protected:
    virtual void setupServer() = 0;
    virtual void cleanUp() = 0;

    std::vector<NetworkError> errors;

    NetworkApi *networkApi;
    int port;

    std::mutex mutex;
    FuncMessageProcessorCreator funcCreateMessageProcessor;

    std::vector<Connection *> connections;
};
