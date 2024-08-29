// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "connection.h"

Connection::Connection(MessageProcessor *messageProcessor, NetworkApi *networkApi, std::vector<NetworkError> *errors, bool bStatusConnected)
{
    this->messageProcessor = messageProcessor;
    this->networkApi = networkApi;
    this->errors = errors;
    this->bIsConnected = bStatusConnected;
}

Connection::~Connection()
{
    delete messageProcessor;
}