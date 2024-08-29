// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "client.h"

Client::Client(NetworkApi &networkApi, MessageReceiver &messageReceiver, const std::string &address, int port)
{
    this->networkApi = &networkApi;
    this->messageReceiver = &messageReceiver;
    this->address = address;
    this->port = port;
}

Client::~Client()
{
}

void Client::request(NetworkApiCall apiCall, const void *data, unsigned int size)
{
    NetworkMessage *request = new NetworkMessage(NetworkMessageType::SendMessage, apiCall, (const char *)data, size);
    mutex.lock();
    requests.push_back(request);
    mutex.unlock();
}

void Client::connectToServer()
{
    if (!bServiceIsRunning)
        runSendingReceivingTask();

    char *data = new char[sizeof(NetworkSetupData)];
    NetworkSetupData *setupData = reinterpret_cast<NetworkSetupData *>(data);
    setupData->address[0] = 0;
    strcpy_s(setupData->address, address.c_str());
    setupData->port = port;

    NetworkMessage *request = new NetworkMessage(NetworkMessageType::SetupConnection, 0, data, sizeof(NetworkSetupData));
    mutex.lock();
    requests.push_back(request);
    mutex.unlock();

    delete[] data;
}