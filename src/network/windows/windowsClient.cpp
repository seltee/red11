// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "windowsClient.h"
#include "red11.h"

#pragma comment(lib, "Ws2_32.lib")

void sendingFunction(NetworkControlData *networkControlData);
void receivingFunction(NetworkControlData *networkControlData);

WindowsClient::WindowsClient(NetworkApi &networkApi, MessageProcessor &messageProcessor, const std::string &address, int port) : Client(networkApi, messageProcessor, address, port)
{
    memset(&networkControlData, 0, sizeof(NetworkControlData));
}

WindowsClient::~WindowsClient()
{
    cleanUp();
}

bool WindowsClient::isConnected()
{
    if (!sendingThread || !sendingThread->joinable())
        return false;
    if (!networkControlData.isConnected)
        return false;
    return true;
}

void WindowsClient::runSendingReceivingTask()
{
    networkControlData.requests = &this->requests;
    networkControlData.errors = &this->errors;
    networkControlData.messageProcessor = this->messageProcessor;
    networkControlData.networkApi = this->networkApi;
    networkControlData.mutex = &this->mutex;
    networkControlData.isConnected = false;

    sendingThread = new std::thread(sendingFunction, &networkControlData);
    receivingThread = new std::thread(receivingFunction, &networkControlData);

    bServiceIsRunning = true;
}

void WindowsClient::cleanUp()
{
    // Cleanup
    closesocket(networkControlData.connectSocket);
    WSACleanup();
}

void sendingFunction(NetworkControlData *networkControlData)
{
    networkControlData->mutex->lock();
    std::vector<NetworkMessage *> *requests = networkControlData->requests;
    std::vector<NetworkError> *errors = networkControlData->errors;
    std::mutex *mutex = networkControlData->mutex;
    networkControlData->isConnected = false;
    mutex->unlock();

    while (1)
    {
        if (requests->size() > 0)
        {
            mutex->lock();
            NetworkMessage *request = requests->at(0);
            requests->erase(requests->begin());

            // Send message to server
            if (request->getType() == NetworkMessageType::SendMessage)
            {
                // Send a message
                mutex->unlock();
                int nResult = send(networkControlData->connectSocket, request->getFullMessageData(), request->getFullMessageSize(), 0);
                mutex->lock();
                if (nResult == SOCKET_ERROR)
                {
                    Red11::getLogger()->logFileAndConsole("Send failed: %i", WSAGetLastError());
                    networkControlData->isConnected = false;
                    errors->push_back({std::string("Failed to send request"), NetworkCode::FailedToSendMessage});
                    mutex->unlock();
                    break;
                }
            }
            else if (request->getType() == NetworkMessageType::SetupConnection)
            {
                // Setup connection to server
                NetworkSetupData *data = (NetworkSetupData *)request->getData();
                int port = data->port;
                std::string address = data->address;
                networkControlData->isConnected = false;
                WSADATA wsaData;
                int nResult;

                // Initialize Winsock
                nResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
                if (nResult != 0)
                {
                    Red11::getLogger()->logFileAndConsole("WSAStartup failed: %i", nResult);
                    mutex->lock();
                    errors->push_back({std::string("WSAStartup failed"), NetworkCode::FailedToSetup});
                    mutex->unlock();
                    break;
                }

                // Create a socket for connecting to the server
                SOCKET connectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                if (connectSocket == INVALID_SOCKET)
                {
                    Red11::getLogger()->logFileAndConsole("Error at socket(): %i", WSAGetLastError());
                    WSACleanup();
                    mutex->lock();
                    errors->push_back({std::string("Error at socket()"), NetworkCode::FailedToSetup});
                    mutex->unlock();
                    break;
                }

                // Set up the server address and port
                sockaddr_in serverAddr;
                serverAddr.sin_family = AF_INET;
                serverAddr.sin_port = htons(port);                         // Server port
                inet_pton(AF_INET, address.c_str(), &serverAddr.sin_addr); // Server IP address (localhost in this case)

                // Connect to the server
                mutex->unlock();
                nResult = connect(connectSocket, (sockaddr *)&serverAddr, sizeof(serverAddr));
                mutex->lock();

                if (nResult == SOCKET_ERROR)
                {
                    Red11::getLogger()->logFileAndConsole("Connect failed: %i", WSAGetLastError());
                    closesocket(connectSocket);
                    WSACleanup();
                    errors->push_back({std::string("Connect failed"), NetworkCode::FailedToConnect});
                    mutex->unlock();
                    break;
                }

                Red11::getLogger()->logFileAndConsole("Connected to server...");

                networkControlData->connectSocket = connectSocket;
                networkControlData->isConnected = true;
            }
            request->destroy();
            mutex->unlock();
        }
        else
            std::this_thread::sleep_for(std::chrono::microseconds(50));
    }

    mutex->lock();
    networkControlData->isConnected = false;
    mutex->unlock();
}

void receivingFunction(NetworkControlData *networkControlData)
{
    networkControlData->mutex->lock();
    std::vector<NetworkMessage *> *requests = networkControlData->requests;
    std::vector<NetworkError> *errors = networkControlData->errors;
    MessageProcessor *messageProcessor = networkControlData->messageProcessor;
    NetworkApi *networkApi = networkControlData->networkApi;
    std::mutex *mutex = networkControlData->mutex;
    mutex->unlock();

    char cBuff[NETWORK_BUFFER_LENGTH];

    NetworkMessageHeader *header = reinterpret_cast<NetworkMessageHeader *>(cBuff);
    char *body = cBuff + sizeof(NetworkMessageHeader);

    while (true)
    {
        if (networkControlData->isConnected)
        {
            int nResult = recv(networkControlData->connectSocket, cBuff, NETWORK_BUFFER_LENGTH, 0);
            if (nResult > 0)
            {
                if (nResult >= sizeof(NetworkMessageHeader))
                {
                    NetworkApiDescriptor networkApiDescriptor = networkApi->getDescriptor(header->code);
                    if (networkApiDescriptor.bIsVolatile || networkApiDescriptor.nSize == header->size)
                    {
                        messageProcessor->receiveMessage(header->code, header->size, body);
                    }
                }
            }
            else if (nResult == 0)
            {
                Red11::getLogger()->logFileAndConsole("Connection closed by server.");
                break;
            }
            else
            {
                Red11::getLogger()->logFileAndConsole("recv failed: %i", WSAGetLastError());
                break;
            }
        }
        else
            std::this_thread::sleep_for(std::chrono::microseconds(100));
    }

    mutex->lock();
    networkControlData->isConnected = false;
    mutex->unlock();
}