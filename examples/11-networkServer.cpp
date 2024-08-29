// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "red11.h"
#include "shared.h"
#include <string>

const NetworkActionCode actionSetUsersPosition = 0;

struct UserDataListItem
{
    int index;
    Vector3 position;
    Quat rotation;
};

class MessageProcessorUser : public MessageProcessor
{
public:
    MessageProcessorUser()
    {
        mutex.lock();
        this->position = Vector3(randf(-2.4f, 2.4f), randf(0.5f, 2.0f), randf(-2.4f, 2.4f));
        this->rotation = Quat(0, 0, 0, 1.0f);

        static int lastIndex = 0;
        index = lastIndex;
        lastIndex++;
        mutex.unlock();
    }

    void receiveMessage(NetworkApiCall code, int size, const char *body) override final
    {
        if (code == apiFunctions->getDataRequest)
        {
            GetDataResponseData response;
            mutex.lock();
            memcpy(response.pos, &position, sizeof(response.pos));
            memcpy(response.rotation, &rotation, sizeof(response.rotation));
            response.index = index;
            mutex.unlock();

            sendRequest(apiFunctions->getDataResponse, &response, sizeof(GetDataResponseData));
        }
        if (code == apiFunctions->setPositionRequest)
        {
            const SetPositionRequestData *request = reinterpret_cast<const SetPositionRequestData *>(body);
            mutex.lock();
            memcpy(&position, request->pos, sizeof(request->pos));
            memcpy(&rotation, request->rotation, sizeof(request->rotation));
            mutex.unlock();
        }
    }

    void doAction(NetworkActionCode actionCode, const char *data, unsigned int size) override final
    {
        if (actionCode == actionSetUsersPosition)
        {
            sendRequest(apiFunctions->setUserPosition, data, size);
        }
    }

    static ApiFunctions *apiFunctions;

    inline int getIndex() { return index; }
    inline Vector3 getPosition() { return position; }
    inline Quat getRotation() { return rotation; }

protected:
    Vector3 position;
    Quat rotation;
    int index;
};
ApiFunctions *MessageProcessorUser::apiFunctions;

MessageProcessor *funcCreateMessageProcessorUser()
{
    return new MessageProcessorUser();
}

APPMAIN
{
    Red11::openConsole();

    // Network
    const int port = 8081;

    NetworkApi api = NetworkApi(0);
    ApiFunctions apiFunctions = ApiFunctions(api);
    MessageProcessorUser::apiFunctions = &apiFunctions;

    Server *server = Red11::createServer(api, port, funcCreateMessageProcessorUser);

    server->startServer();

    DeltaCounter deltaCounter;
    float updPositions = 0.0f;
    float updAmountOfUser = 0.0f;
    while (true)
    {
        float delta = deltaCounter.getDeltaFrameCounter();
        updPositions += delta;
        updAmountOfUser += delta;

        if (updPositions > 0.02f && server->getConnectionsAmount() > 0)
        {
            server->filterDisconnected();
            server->lockConnections();
            int usersAmount = server->getConnectionsAmount();
            int size = sizeof(UserDataListItem) * usersAmount;
            char *listPtr = new char[size];
            UserDataListItem *list = (UserDataListItem *)listPtr;
            for (int i = 0; i < usersAmount; i++)
            {
                Connection *connection = server->getConnectionsList()->at(i);
                MessageProcessorUser *processor = reinterpret_cast<MessageProcessorUser *>(connection->getMessageProcessor());

                list[i].index = processor->getIndex();
                list[i].position = processor->getPosition();
                list[i].rotation = processor->getRotation();
            }
            server->unlockConnections();

            server->broadcast(actionSetUsersPosition, listPtr, size);
            updPositions = 0.0f;
        }

        if (updAmountOfUser > 3.0f)
        {
            Red11::getLogger()->logConsole("Users: %i", server->getConnectionsAmount());
            updAmountOfUser = 0.0f;
        }
    }

    return 0;
}