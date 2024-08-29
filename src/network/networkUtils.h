// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"

#define NETWORK_BUFFER_LENGTH (1024 * 16)

typedef unsigned int NetworkApiCall;

struct NetworkMessageHeader
{
    unsigned int code;
    unsigned int size;
};

enum class NetworkMessageType
{
    None,
    SetupConnection,
    SendMessage,
    RecvMessage
};

enum class NetworkCode
{
    NoError,
    FailedToConnect,
    FailedToSendMessage,
    FailedToSetup
};

struct NetworkSetupData
{
    char address[20];
    int port;
};

struct NetworkError
{
    std::string message;
    NetworkCode code;
};