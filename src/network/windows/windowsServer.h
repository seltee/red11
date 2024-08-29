// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once

#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include "network/server.h"
#include "utils/utils.h"
#include "windowsConnection.h"

struct AcceptConnectionsControlData
{
    std::vector<NetworkError> *errors;
    std::vector<Connection *> *connections;
    FuncMessageProcessorCreator funcCreateMessageProcessor;
    std::mutex *mutex;
    NetworkApi *networkApi;
    SOCKET connectSocket;
};

class WindowsServer : public Server
{
public:
    EXPORT WindowsServer(NetworkApi &networkApi, int port, FuncMessageProcessorCreator funcCreateMessageProcessor);

    EXPORT bool isRunning() override final;
    EXPORT void setupServer() override final;
    EXPORT void cleanUp() override final;

protected:
    bool bIsRunning = false;
    SOCKET connectSocket;
    std::thread *connectThread = nullptr;
    AcceptConnectionsControlData acceptConnectionsControlData;
};