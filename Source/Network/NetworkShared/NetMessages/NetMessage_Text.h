#pragma once
#include "NetworkEngine/NetMessage.h"
#include <string>

class NetMessage_Text : public NetMessage
{
public:
	NetMessage_Text();
	void SetData(const std::string& aString) { myData = aString; }
	const std::string& GetData() { return myData; }

	void Serialize(NetBuffer& aBuffer) override;
	void Deserialize(NetBuffer& aBuffer) override;

	void GetStringRepresentation(char* outString, int aBufferSize) const override;

protected:
	std::string myData;
};