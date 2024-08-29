#include "messageReceiver.h"

void MessageReceiver::sendRequest(NetworkApiCall apiCall, const void *data, unsigned int size)
{
    NetworkMessage *answer = new NetworkMessage(NetworkMessageType::SendMessage, apiCall, (const char *)data, size);
    mutex.lock();
    messages.push_back(answer);
    mutex.unlock();
}

NetworkMessage *MessageReceiver::getNetworkMessage()
{

    mutex.lock();
    if (messages.size() > 0)
    {
        NetworkMessage *message = messages.at(0);
        messages.erase(messages.begin());
        mutex.unlock();
        return message;
    }
    mutex.unlock();
    return nullptr;
}