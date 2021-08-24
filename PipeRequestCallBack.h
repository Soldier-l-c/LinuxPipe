#pragma once
#include "IPipeCallBack.h"
class PipeRequestCallback :public IPipeCallBack
{
public:
	virtual void OnDataRead(const char* data, const int dataLe) override;
};