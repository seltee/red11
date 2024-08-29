// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "network/connection.h"
#include <winsock2.h>
#include <ws2tcpip.h>

class WindowsConnection : public Connection
{
public:
    WindowsConnection(
        MessageProcessor *messageProcessor,
        NetworkApi *networkApi,
        std::vector<NetworkError> *errors,
        std::mutex *mutex,
        SOCKET clientSocket,
        bool bStatusConnected);

    std::thread *clientThreadReceiver = nullptr;
    std::thread *clientThreadSender = nullptr;
    std::mutex *mutex = nullptr;
    SOCKET clientSocket;
};
