// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "red11.h"
#include <string>

struct UserRequest
{
    char userName[16];
};

struct UserResponse
{
    char responseToUser[32];
};

struct ApiFunctions
{
    NetworkApiCall userRequest;
    NetworkApiCall userResponse;
};

class MessageReceiverUser : public MessageReceiver
{
public:
    MessageReceiverUser()
    {
    }

    void receiveMessage(NetworkApiCall code, int size, const char *body) override final
    {
        if (code == apiFunctions.userRequest)
        {
            char buff[17];
            memcpy(buff, body, 16);
            buff[16] = 0;
            std::string answer = std::string("Hello, ") + std::string(buff);
            UserResponse userResponse;
            userResponse.responseToUser[0] = 0;
            strcpy_s(userResponse.responseToUser, answer.c_str());
            sendRequest(apiFunctions.userResponse, &userResponse, sizeof(UserResponse));

            Red11::getLogger()->logConsole("Sent back");
        }
    }

    static ApiFunctions apiFunctions;
};
ApiFunctions MessageReceiverUser::apiFunctions;

MessageReceiver *funcCreateMessageReceiverUser()
{
    return new MessageReceiverUser();
}

APPMAIN
{
    Red11::openConsole();

    // Network
    const int port = 8081;

    ApiFunctions apiFunctions;
    NetworkApi api = NetworkApi(0);
    apiFunctions.userRequest = api.addFixedSizeApiCall(sizeof(UserRequest));
    apiFunctions.userResponse = api.addFixedSizeApiCall(sizeof(UserResponse));
    MessageReceiverUser::apiFunctions = apiFunctions;

    Server *server = Red11::createServer(api, port, funcCreateMessageReceiverUser);

    server->startServer();

    DeltaCounter deltaCounter;
    while (true)
    {
        deltaCounter.getDeltaFrameCounter();
    }

    return 0;
}