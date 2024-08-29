// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "server.h"
#include <red11.h>

Server::Server(NetworkApi &networkApi, int port, FuncMessageProcessorCreator funcCreateMessageProcessor)
{
    this->networkApi = &networkApi;
    this->port = port;
    this->funcCreateMessageProcessor = funcCreateMessageProcessor;
}

void Server::startServer()
{
    if (!isRunning())
        setupServer();
}

void Server::broadcast(NetworkActionCode actionCode, const char *data, unsigned int size, IsQualifyForBroadcast checkIfQualify)
{
    if (data == nullptr)
        size = 0;
    if (size == 0)
        data = nullptr;

    mutex.lock();
    for (auto &con : connections)
    {
        if (con->isConnected())
        {
            if (checkIfQualify && checkIfQualify(con))
            {
                con->getMessageProcessor()->doAction(actionCode, data, size);
            }
            else
            {
                con->getMessageProcessor()->doAction(actionCode, data, size);
            }
        }
    }
    mutex.unlock();
}

void Server::filterDisconnected()
{
    mutex.lock();
    auto it = connections.begin();
    while (it != connections.end())
    {
        if ((*it)->isConnected())
        {
            ++it;
        }
        else
        {
            Red11::getLogger()->logConsole("Remove disconnected");
            // delete (*it)->controlData;
            // delete (*it)->messageProcessor;
            delete (*it);
            it = connections.erase(it);
        }
    }
    mutex.unlock();
}

void Server::lockConnections()
{
    mutex.lock();
}

void Server::unlockConnections()
{
    mutex.unlock();
}
