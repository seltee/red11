// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "windowsConnection.h"

WindowsConnection::WindowsConnection(
    MessageProcessor *messageProcessor,
    NetworkApi *networkApi,
    std::vector<NetworkError> *errors,
    std::mutex *mutex,
    SOCKET clientSocket,
    bool bStatusConnected) : Connection(messageProcessor, networkApi, errors, bStatusConnected)
{
    this->clientSocket = clientSocket;
    this->mutex = mutex;
}