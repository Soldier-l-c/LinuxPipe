#pragma once
#include "IPipeCallBack.h"
class PipeResponse :public IPipeCallBack
{
public:
	virtual void OnDataRead(const char* data, const int dataLe) override;

};