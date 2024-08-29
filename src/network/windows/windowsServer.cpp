// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "windowsServer.h"
#include "red11.h"

#pragma comment(lib, "Ws2_32.lib")

void clientConnectionReceiver(WindowsConnection *clientControlData);
void clientConnectionSender(WindowsConnection *clientControlData);
void acceptConnections(AcceptConnectionsControlData *acceptConnectionsControlData);

WindowsServer::WindowsServer(NetworkApi &networkApi, int port, FuncMessageProcessorCreator funcCreateMessageProcessor) : Server(networkApi, port, funcCreateMessageProcessor)
{
    memset(&acceptConnectionsControlData, 0, sizeof(AcceptConnectionsControlData));
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
    acceptConnectionsControlData.errors = &this->errors;
    acceptConnectionsControlData.connections = &this->connections;
    acceptConnectionsControlData.mutex = &this->mutex;
    acceptConnectionsControlData.connectSocket = connectSocket;
    acceptConnectionsControlData.networkApi = networkApi;
    acceptConnectionsControlData.funcCreateMessageProcessor = funcCreateMessageProcessor;

    mutex.lock();
    connectThread = new std::thread(acceptConnections, &acceptConnectionsControlData);
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

void clientConnectionReceiver(WindowsConnection *connection)
{
    std::mutex *mutex = connection->mutex;
    SOCKET clientSocket = connection->clientSocket;
    NetworkApi *networkApi = connection->getNetworkApi();
    MessageProcessor *messageProcessor = connection->getMessageProcessor();

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
                if (networkApiDescriptor.bIsVolatile || networkApiDescriptor.nSize == header->size)
                {
                    messageProcessor->receiveMessage(header->code, header->size, body);
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
    connection->setConnectionState(false);
    mutex->unlock();
}

void clientConnectionSender(WindowsConnection *connection)
{
    std::vector<NetworkError> *errors = connection->getErrorsList();
    // NetworkApi *networkApi = connection->getNetworkApi();
    MessageProcessor *messageProcessor = connection->getMessageProcessor();
    std::mutex *mutex = connection->mutex;
    SOCKET clientSocket = connection->clientSocket;

    char cBuff[NETWORK_BUFFER_LENGTH];

    // NetworkMessageHeader *header = reinterpret_cast<NetworkMessageHeader *>(cBuff);
    // char *body = cBuff + sizeof(NetworkMessageHeader);

    // Receive until the peer closes the connection
    while (true)
    {
        if (messageProcessor->isHasMessages())
        {
            mutex->lock();
            NetworkMessage *networkMessage = messageProcessor->getNetworkMessage();

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

void acceptConnections(AcceptConnectionsControlData *acceptConnectionsControlData)
{
    std::vector<NetworkError> *errors = acceptConnectionsControlData->errors;
    std::vector<Connection *> *connections = acceptConnectionsControlData->connections;
    std::mutex *mutex = acceptConnectionsControlData->mutex;
    NetworkApi *networkApi = acceptConnectionsControlData->networkApi;
    FuncMessageProcessorCreator funcCreateMessageProcessor = acceptConnectionsControlData->funcCreateMessageProcessor;

    while (true)
    {
        Red11::getLogger()->logFileAndConsole("Waiting for connection");

        SOCKET clientSocket = accept(acceptConnectionsControlData->connectSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET)
        {
            Red11::getLogger()->logFileAndConsole("Accept failed: %i", WSAGetLastError());
            continue;
        }

        Red11::getLogger()->logFileAndConsole("Client connected, creating thread...");

        WindowsConnection *connection = new WindowsConnection(
            funcCreateMessageProcessor(),
            networkApi,
            errors,
            mutex,
            clientSocket,
            true);

        connection->clientThreadReceiver = new std::thread(clientConnectionReceiver, connection);
        connection->clientThreadSender = new std::thread(clientConnectionSender, connection);

        mutex->lock();
        connections->push_back(connection);
        mutex->unlock();
    }
}