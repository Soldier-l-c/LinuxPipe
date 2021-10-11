#include "stdafx.h"
#include "util.h"
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
	auto code = _sopen_s(&writeFd, pipeName, _O_RDWR | _O_APPEND | _O_CREAT,
		_SH_DENYNO, _S_IREAD | _S_IWRITE);//_O_APPEND追加方式写，_O_CREAT不存在创建， _SH_DENYNO共享读写
#endif // !WIN32

	/*向管道写入数据*/
	auto len = _write(writeFd, data.c_str(), (int)data.length());
	std::cout << "Data write len: " << len << " data:" << data << std::endl;

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
