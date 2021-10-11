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
	writeFd = _open(pipeName, O_RDWR | O_APPEND | O_CREAT, 0);
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

#ifndef WIN32
#include <unistd.h>
#include <errno.h>
#include <sys/file.h>
#define  PID_BUF_LEN   (20)
#define  RUN_PID_FILE  "/var/run/myserver.pid"
#endif

bool SingleInstance::IsRunning()
{
#ifdef WIN32
	HANDLE hObject = CreateMutex(nullptr, false, L"Global\\{DE45BDFF-B5D1-4B65-BA78-09EC77CA57A0}_Pipe");

	if (nullptr == hObject || GetLastError() == ERROR_ALREADY_EXISTS)
	{
		if (nullptr != hObject)CloseHandle(hObject);
		return true;
	}

	return false;
#else
	int fd = open(RUN_PID_FILE, O_WRONLY | O_CREAT);
	if (fd < 0)
	{
		printf("open run pid err(%d)! %s\n", errno, RUN_PID_FILE);
		return -1;
	}

	// 加锁
	// LOCK_SH 建立共享锁定。多个进程可同时对同一个文件作共享锁定。
	// LOCK_EX 建立互斥锁定。一个文件同时只有一个互斥锁定。
	if (flock(fd, LOCK_EX | LOCK_NB) == -1)
	{
		//加不上锁，则是服务正在运行，已上锁了
		printf("server is runing now! errno=%d\n", errno);
		close(fd);
		return 1;
	}

	// 加锁成功，证明服务没有运行
	// 文件句柄不要关，也不要解锁
	// 进程退出，自动就解锁了
	printf("myserver is not running! begin to run..... pid=%ld\n", (long)getpid());

	char pid_buf[PID_BUF_LEN] = { 0 };
	snprintf(pid_buf, sizeof(pid_buf) - 1, "%ld\n", (long)getpid());

	// 把进程pid写入到/var/run/myserver.pid文件
	write(fd, pid_buf, strlen(pid_buf));

	return 0;
#endif // WIN32
}
