// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "server.h"

Server::Server(NetworkApi &networkApi, int port, FuncMessageReceiverCreator funcCreateMessageReceiver)
{
    this->networkApi = &networkApi;
    this->port = port;
    this->funcCreateMessageReceiver = funcCreateMessageReceiver;
}

void Server::startServer()
{
    if (!isRunning())
        setupServer();
}
