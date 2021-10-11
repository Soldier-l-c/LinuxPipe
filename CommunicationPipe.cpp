#include "stdafx.h"
#include "CommunicationPipe.h"
#include "IPipeCallBack.h"
#include "util.h"
#define MAX_LEN 128
CCommunicatPipe::CCommunicatPipe(const std::string& pipeName) :m_strPipeName(pipeName)
{
	auto unlinkRes = _unlink(m_strPipeName.c_str());
	std::cout << "Unlink pipe. FileName:[" << m_strPipeName << "] Res:["<< unlinkRes <<"]" << std::endl;
	std::cout << "OPen pipe. FileName:[" << m_strPipeName << "]" << std::endl;

#ifndef WIN32
	m_nFileId = _open(m_strPipeName.c_str(), O_RDONLY, 0);
#else
	auto code = _sopen_s(&m_nFileId, m_strPipeName.c_str(), O_RDONLY | _O_CREAT, _SH_DENYNO,
		_S_IREAD | _S_IWRITE);//O_RDONLY只读_O_CREAT不存在创建
#endif // !WIN32
	std::cout<<"OPen pipe file success. FileName:  "<<m_strPipeName<<std::endl;

	m_bIsInited = m_nFileId > 0;
}

CCommunicatPipe::~CCommunicatPipe()
{
	_close(m_nFileId);
	auto code = _unlink(m_strPipeName.c_str());
}

void CCommunicatPipe::StartRead()
{
	if (!m_bIsInited)
	{
		std::cout << "CCommunicatPipe Init failed. Name:" << m_strPipeName << std::endl;
		return;
	}
	std::cout<<"Start read pipe name: "<<m_strPipeName<<std::endl;
	__Read();

}

void CCommunicatPipe::SetCallBack(std::shared_ptr<IPipeCallBack> ptr)
{
	m_spCallBack = ptr;
}

void CCommunicatPipe::__Read()
{
	auto func = [this]
	{
		while (true)
		{
			char line[MAX_LEN] = { 0 };
			int32_t nRead{ 0 };
			std::string data;
			while ((nRead = _read(m_nFileId, line, MAX_LEN - 1)) > 0)
			{
				line[nRead] = '\0';
				data += line;
			}
			
			if (data.length() > 0)
			{
				std::cout << "Data read: " << data << std::endl;
				m_spCallBack->OnDataRead(data.c_str(), data.length());
			}

			if (!__ResidentProcess())
			{
				return;
			}
		}
	};

	std::thread(func).join();
}

bool CCommunicatPipe::__ResidentProcess()
{
	return !(m_strPipeName == RESPONSE_PIPE);
}
