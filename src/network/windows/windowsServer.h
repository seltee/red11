// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once

#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include "network/server.h"
#include "utils/utils.h"

struct ConnectionData
{
    std::thread *clientThreadReceiver;
    std::thread *clientThreadSender;
    void *userData;
    bool bIsConnected;
};

struct ServerControlData
{
    std::vector<NetworkError> *errors;
    std::vector<ConnectionData *> *connections;
    FuncMessageReceiverCreator funcCreateMessageReceiver;
    std::mutex *mutex;
    NetworkApi *networkApi;
    SOCKET connectSocket;
};

struct ClientControlData
{
    std::vector<NetworkError> *errors;
    MessageReceiver *messageReceiver;
    std::mutex *mutex;
    NetworkApi *networkApi;
    SOCKET clientSocket;
    bool isConnected;
};

class WindowsServer : public Server
{
public:
    EXPORT WindowsServer(NetworkApi &networkApi, int port, FuncMessageReceiverCreator funcCreateMessageReceiver);

    EXPORT bool isRunning() override final;
    EXPORT void setupServer() override final;
    EXPORT void cleanUp() override final;

protected:
    bool bIsRunning = false;
    SOCKET connectSocket;
    std::thread *connectThread = nullptr;
    ServerControlData serverControlData;
    std::vector<ConnectionData *> connections;
};