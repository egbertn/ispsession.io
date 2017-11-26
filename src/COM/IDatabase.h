#pragma once
//the minimal interface we have to support to write and read application states
MIDL_INTERFACE("BD0DDE4D-EC2A-4E91-948D-6B3EFBAFE75B")
IDatabase: public IUnknown
{
public:
	STDMETHOD(get_KeyCount)(INT * count) = 0;
	STDMETHOD(get_KeyStates)(
		std::vector<string> &changedKeys,
		std::vector<string> &newKeys,
		std::vector<string> &otherKeys,
		std::vector<std::pair<char*, INT>> & expireKeys,
		std::vector<string> &removedKeys) = 0;
	STDMETHOD(SerializeKey)(BSTR Key, IStream* binaryString) = 0;
	STDMETHOD(DeserializeKey)(const std::string& binaryString) = 0;
};
