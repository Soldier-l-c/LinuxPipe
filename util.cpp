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
		_SH_DENYNO, _S_IREAD | _S_IWRITE);//_O_APPEND׷�ӷ�ʽд��_O_CREAT�����ڴ����� _SH_DENYNO�����д
#endif // !WIN32

	/*��ܵ�д������*/
	auto len = _write(writeFd, data.c_str(), (int)data.length());
	std::cout << "Data write len: " << len << " data:" << data << std::endl;

	_close(writeFd);
	return 0;
}

std::string Json::MakeReaponseStr(const ERR_CODE errcode, const std::string& errMsg)
{
	rapidjson::Document extInfo;
	rapidjson::Document::AllocatorType& allocator = extInfo.GetAllocator();	//��ȡ������
	extInfo.SetObject();//����ǰ��Document����Ϊһ��object��Ҳ����˵������Document��һ��Object���͵�domԪ��
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

	// ����
	// LOCK_SH ��������������������̿�ͬʱ��ͬһ���ļ�������������
	// LOCK_EX ��������������һ���ļ�ͬʱֻ��һ������������
	if (flock(fd, LOCK_EX | LOCK_NB) == -1)
	{
		//�Ӳ����������Ƿ����������У���������
		printf("server is runing now! errno=%d\n", errno);
		close(fd);
		return 1;
	}

	// �����ɹ���֤������û������
	// �ļ������Ҫ�أ�Ҳ��Ҫ����
	// �����˳����Զ��ͽ�����
	printf("myserver is not running! begin to run..... pid=%ld\n", (long)getpid());

	char pid_buf[PID_BUF_LEN] = { 0 };
	snprintf(pid_buf, sizeof(pid_buf) - 1, "%ld\n", (long)getpid());

	// �ѽ���pidд�뵽/var/run/myserver.pid�ļ�
	write(fd, pid_buf, strlen(pid_buf));

	return 0;
#endif // WIN32
}
