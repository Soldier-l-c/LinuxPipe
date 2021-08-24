#include "stdafx.h"
#include "util.h"
#define MAX_LEN 128
int32_t Pipe::PipeWrite(const char* pipeName, const std::string& data)
{
	int writeFd{0};
#ifndef WIN32
	if (mkfifo(pipeName, S_IRUSR | S_IWUSR) < 0 && (errno != EEXIST))
	{
		perror("make fifo failed:");
		return -1;
	}
#endif // !WIN32

#ifndef WIN32
	writeFd = _open(pipeName, O_WRONLY, 0);
#else
	int hFile{ 0 };
	_sopen_s(&hFile, pipeName, O_RDONLY, 0, 0);
#endif // !WIN32

	/*向管道写入数据*/
	auto len = _write(writeFd, data.c_str(), data.length());
	std::cout << "Data write len: " << len << " data:" << data<<std::endl;
	_close(writeFd);
	return 0;
}

std::string Json::MakeReaponseStr(const ERR_CODE errcode, const std::string& errMsg)
{
	rapidjson::Document extInfo;
	rapidjson::Document::AllocatorType& allocator = extInfo.GetAllocator();	//获取分配器
	extInfo.SetObject();//将当前的Document设置为一个object，也就是说，整个Document是一个Object类型的dom元素
	rapidjson::Value strValKey;
	extInfo.AddMember("errcode", errcode, allocator);
	switch (errcode)
	{
	case E_SUCCESS:
		extInfo.AddMember("msg", "ok", allocator);
		break;
	case E_FAIE:
	case E_EXPECTED_DATA:
		strValKey.SetString(errMsg.c_str(), errMsg.length(), allocator);
		extInfo.AddMember("msg", strValKey, allocator);
		break;
	default:
		break;
	}

	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	extInfo.Accept(writer);
	return buffer.GetString();
}

bool Json::JsonParse(const char* buffer, ERR_CODE& code, std::stringstream& errMsg)
{
	CHECK_NULL_PTR(buffer);
	rapidjson::Document doc;
	doc.Parse(buffer);
	if (!doc.IsObject())
	{
		code = E_EXPECTED_DATA;
		errMsg << "Error, post should be json!" << "<br/>";
		return false;
	}
	std::string name, password;
	if (doc.HasMember("name") && doc["name"].IsString())
	{
		name = doc["name"].GetString();
	}
	if (doc.HasMember("password") && doc["password"].IsString())
	{
		password = doc["password"].GetString();
	}
	return true;
}
