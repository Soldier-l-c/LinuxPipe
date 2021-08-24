#pragma once

enum ERR_CODE
{
	E_SUCCESS,
	E_EXPECTED_DATA,
	E_FAIE
};
#define RESQUEST_PIPE "/root/gcc/tttt/request_pipe"
#define RESPONSE_PIPE "/root/gcc/tttt/response_pipe"
namespace Pipe
{
	int32_t PipeWrite(const char* pipeName, const std::string& dat);
}
namespace Json
{
	std::string MakeReaponseStr(const ERR_CODE errcode, const std::string& errMsg);
	bool JsonParse(const char* buffer, ERR_CODE& code, std::stringstream& errMsg);
}