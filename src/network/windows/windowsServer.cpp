// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "windowsServer.h"
#include "red11.h"

#pragma comment(lib, "Ws2_32.lib")

void clientConnectionReceiver(ClientControlData *clientControlData);
void clientConnectionSender(ClientControlData *clientControlData);
void acceptConnections(ServerControlData *serverControlData);

WindowsServer::WindowsServer(NetworkApi &networkApi, int port, FuncMessageReceiverCreator funcCreateMessageReceiver) : Server(networkApi, port, funcCreateMessageReceiver)
{
    memset(&serverControlData, 0, sizeof(ServerControlData));
}

bool WindowsServer::isRunning()
{
    return bIsRunning;
}

void WindowsServer::setupServer()
{
    bIsRunning = false;
    WSADATA wsaData;

    // Initialize Winsock
    int nResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (nResult != 0)
    {
        Red11::getLogger()->logFileAndConsole("WSAStartup failed: %i", nResult);
        return;
    }

    // Create a socket for the server
    connectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (connectSocket == INVALID_SOCKET)
    {
        Red11::getLogger()->logFileAndConsole("Error at socket():  %i", WSAGetLastError());
        WSACleanup();
        return;
    }

    // Setup the TCP listening socket
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    nResult = bind(connectSocket, (sockaddr *)&serverAddr, sizeof(serverAddr));
    if (nResult == SOCKET_ERROR)
    {
        Red11::getLogger()->logFileAndConsole("Bind failed: %i", WSAGetLastError());
        closesocket(connectSocket);
        WSACleanup();
        return;
    }

    nResult = listen(connectSocket, SOMAXCONN);
    if (nResult == SOCKET_ERROR)
    {
        Red11::getLogger()->logFileAndConsole("Listen failed: %i", WSAGetLastError());
        closesocket(connectSocket);
        WSACleanup();
        return;
    }

    Red11::getLogger()->logFileAndConsole("Server is listening on port %i ...", port);

    // Start a thread to accept connections
    serverControlData.errors = &this->errors;
    serverControlData.connections = &this->connections;
    serverControlData.mutex = &this->mutex;
    serverControlData.connectSocket = connectSocket;
    serverControlData.networkApi = networkApi;
    serverControlData.funcCreateMessageReceiver = funcCreateMessageReceiver;

    mutex.lock();
    connectThread = new std::thread(acceptConnections, &serverControlData);
    bIsRunning = true;
    mutex.unlock();
}

void WindowsServer::cleanUp()
{
    // Cleanup
    closesocket(connectSocket);
    WSACleanup();
    bIsRunning = false;
}

void clientConnectionReceiver(ClientControlData *clientControlData)
{
    clientControlData->mutex->lock();
    SOCKET clientSocket = clientControlData->clientSocket;
    // std::vector<NetworkError> *errors = clientControlData->errors;
    std::mutex *mutex = clientControlData->mutex;
    NetworkApi *networkApi = clientControlData->networkApi;
    MessageReceiver *messageReceiver = clientControlData->messageReceiver;
    clientControlData->isConnected = true;
    mutex->unlock();

    char cBuff[NETWORK_BUFFER_LENGTH];

    NetworkMessageHeader *header = reinterpret_cast<NetworkMessageHeader *>(cBuff);
    char *body = cBuff + sizeof(NetworkMessageHeader);

    // Receive until the peer closes the connection
    while (true)
    {
        int nResult = recv(clientSocket, cBuff, NETWORK_BUFFER_LENGTH, 0);
        if (nResult > 0)
        {
            if (nResult >= sizeof(NetworkMessageHeader))
            {
                NetworkApiDescriptor networkApiDescriptor = networkApi->getDescriptor(header->code);
                if (networkApiDescriptor.nSize == header->size)
                {
                    messageReceiver->receiveMessage(header->code, header->size, body);
                }
            }
        }
        else if (nResult == 0)
        {
            Red11::getLogger()->logConsole("Connection closed");
            break;
        }
        else
        {
            Red11::getLogger()->logConsole("recv failed: %i", WSAGetLastError());
            break;
        }
    }

    Red11::getLogger()->logConsole("Client disconnected");

    mutex->lock();
    clientControlData->isConnected = false;
    mutex->unlock();
}

void clientConnectionSender(ClientControlData *clientControlData)
{
    clientControlData->mutex->lock();
    SOCKET clientSocket = clientControlData->clientSocket;
    std::vector<NetworkError> *errors = clientControlData->errors;
    std::mutex *mutex = clientControlData->mutex;
    NetworkApi *networkApi = clientControlData->networkApi;
    MessageReceiver *messageReceiver = clientControlData->messageReceiver;
    clientControlData->mutex->unlock();

    char cBuff[NETWORK_BUFFER_LENGTH];
    int nBuffSize = NETWORK_BUFFER_LENGTH;

    NetworkMessageHeader *header = reinterpret_cast<NetworkMessageHeader *>(cBuff);
    char *body = cBuff + sizeof(NetworkMessageHeader);

    // Receive until the peer closes the connection
    while (true)
    {
        if (messageReceiver->isHasMessages())
        {
            mutex->lock();
            NetworkMessage *networkMessage = messageReceiver->getNetworkMessage();

            if (networkMessage)
            {
                if (networkMessage->getType() == NetworkMessageType::SendMessage)
                {
                    // Send a message
                    mutex->unlock();
                    int nResult = send(clientSocket, networkMessage->getFullMessageData(), networkMessage->getFullMessageSize(), 0);
                    mutex->lock();
                    if (nResult == SOCKET_ERROR)
                    {
                        Red11::getLogger()->logFileAndConsole("Send failed: %i", WSAGetLastError());
                        errors->push_back({std::string("Failed to send request"), NetworkCode::FailedToSendMessage});
                        mutex->unlock();
                        break;
                    }
                }

                networkMessage->destroy();
            }
            mutex->unlock();
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
    }
}

void acceptConnections(ServerControlData *serverControlData)
{
    serverControlData->mutex->lock();
    std::vector<NetworkError> *errors = serverControlData->errors;
    std::vector<ConnectionData *> *connections = serverControlData->connections;
    std::mutex *mutex = serverControlData->mutex;
    NetworkApi *networkApi = serverControlData->networkApi;
    FuncMessageReceiverCreator funcCreateMessageReceiver = serverControlData->funcCreateMessageReceiver;
    serverControlData->mutex->unlock();

    while (true)
    {
        Red11::getLogger()->logFileAndConsole("Waiting for connection");

        SOCKET clientSocket = accept(serverControlData->connectSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET)
        {
            Red11::getLogger()->logFileAndConsole("Accept failed: %i", WSAGetLastError());
            continue;
        }

        Red11::getLogger()->logFileAndConsole("Client connected, creating thread...");

        // Create a thread to handle the client
        ClientControlData *clientControlData = new ClientControlData();
        clientControlData->mutex = mutex;
        clientControlData->errors = errors;
        clientControlData->clientSocket = clientSocket;
        clientControlData->networkApi = networkApi;
        clientControlData->messageReceiver = funcCreateMessageReceiver();

        ConnectionData *connection = new ConnectionData();
        connection->bIsConnected = true;
        connection->userData = nullptr;
        connection->clientThreadReceiver = new std::thread(clientConnectionReceiver, clientControlData);
        connection->clientThreadReceiver = new std::thread(clientConnectionSender, clientControlData);

        mutex->lock();
        connections->push_back(connection);
        mutex->unlock();
    }
}