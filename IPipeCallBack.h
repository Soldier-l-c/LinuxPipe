#pragma once
class IPipeCallBack
{
public:
	virtual void OnDataRead(const char* data, const int dataLen) = 0;
};
