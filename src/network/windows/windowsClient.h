// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include "network/client.h"
#include "utils/utils.h"

struct NetworkControlData
{
    std::vector<NetworkMessage *> *requests;
    std::vector<NetworkError> *errors;
    MessageReceiver *messageReceiver;
    std::mutex *mutex;
    NetworkApi *networkApi;
    SOCKET connectSocket;
    bool isConnected;
};

class WindowsClient : public Client
{
public:
    EXPORT WindowsClient(NetworkApi &networkApi, MessageReceiver &messageReceiver, const std::string &address, int port);
    EXPORT ~WindowsClient();

    bool isConnected() override final;

protected:
    void runSendingReceivingTask() override final;
    void cleanUp() override final;

    std::thread *sendingThread;
    std::thread *receivingThread;

    NetworkControlData networkControlData;
};