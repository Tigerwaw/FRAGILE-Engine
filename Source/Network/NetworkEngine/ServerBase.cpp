#include "ServerBase.h"
#include <iostream>

#include "NetMessage.h"

ServerBase::~ServerBase()
{
    myComm.Destroy();
}

void ServerBase::StartServer()
{
    myComm.Init(true, false, "");
    myShouldReceive = true;
}

void ServerBase::Update()
{
    if (myShouldReceive)
    {
        Receive();
    }
}

void ServerBase::Receive()
{
    for (int i = 0; i < myMessagesHandledPerTick; i++)
    {
        sockaddr_in otherAddress = {};
        NetBuffer receiveBuffer;
        int bytesReceived = myComm.ReceiveData(receiveBuffer, otherAddress);
        if (bytesReceived > 0)
        {
            NetMessage* receivedMessage = ReceiveMessage(receiveBuffer);

            if (receivedMessage)
            {
                receivedMessage->Deserialize(receiveBuffer);
                HandleMessage(receivedMessage, otherAddress, bytesReceived);
                delete receivedMessage;
            }
        }
        else
        {
            break;
        }
    }
}

void ServerBase::SendToClient(NetBuffer& aBuffer, int aClientIndex) const
{
    myComm.SendData(aBuffer, myClients[aClientIndex].address);
}

void ServerBase::SendToAllClients(NetBuffer& aBuffer) const
{
    for (auto& client : myClients)
    {
        myComm.SendData(aBuffer, client.address);
    }
}

void ServerBase::SendToAllClientsExcluding(NetBuffer& aBuffer, const int aClientIndex) const
{
    for (auto& client : myClients)
    {
        if (GetClientIndex(client.address) == aClientIndex) continue;

        myComm.SendData(aBuffer, client.address);
    }
}

bool ServerBase::DoesClientExist(const sockaddr_in& aAddress) const
{
    if (myClients.empty()) return false;

    for (int i = 0; i < myClients.size(); i++)
    {
        if (myClients[i] == aAddress)
        {
            return true;
        }
    }

    return false;
}

const int ServerBase::GetClientIndex(const sockaddr_in& aAddress) const
{
    for (int i = 0; i < myClients.size(); i++)
    {
        if (myClients[i] == aAddress)
        {
            return i;
        }
    }

    return -1;
}