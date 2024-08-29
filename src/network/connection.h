// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"
#include "networkUtils.h"
#include "messageProcessor.h"
#include "networkApi.h"
#include <vector>

class Connection
{
public:
    EXPORT Connection(MessageProcessor *messageProcessor, NetworkApi *networkApi, std::vector<NetworkError> *errors, bool bStatusConnected = true);
    EXPORT virtual ~Connection();

    inline void setConnectionState(bool bState) { bIsConnected = bState; }
    inline bool isConnected() { return bIsConnected; }

    inline NetworkApi *getNetworkApi() { return networkApi; }
    inline MessageProcessor *getMessageProcessor() { return messageProcessor; }
    inline std::vector<NetworkError> *getErrorsList() { return errors; }

protected:
    std::vector<NetworkError> *errors;
    MessageProcessor *messageProcessor;
    NetworkApi *networkApi;
    bool bIsConnected = false;
};