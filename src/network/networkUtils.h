// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"

#define NETWORK_BUFFER_LENGTH (32 * 1024)

typedef unsigned int NetworkApiCall;
typedef unsigned int NetworkActionCode;

struct NetworkMessageHeader
{
    unsigned int code;
    unsigned int size;
};

enum class NetworkMessageType
{
    None,
    Action,
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