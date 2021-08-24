#include "stdafx.h"
#include "util.h"
#include "PipeRequestCallBack.h"

void PipeRequestCallback::OnDataRead(const char* data, const int dataLe)
{
	std::stringstream errMsg;
	ERR_CODE code{ E_SUCCESS };
	Json::JsonParse(data, code, errMsg);

	auto response = Json::MakeReaponseStr(code, errMsg.str());
	Pipe::PipeWrite(RESPONSE_PIPE, response);
}
