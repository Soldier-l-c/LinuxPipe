#include "stdafx.h"
#include "util.h"
#include "CommunicationPipe.h"
#include "PipeRequestCallBack.h"
#include "PipeResponseCallBack.h"
std::string GetPostData()
{
	std::stringstream errMsg;
	ERR_CODE code{ E_SUCCESS };
#ifdef WIN32
	const char* lenstr = nullptr;
#else 
	char* lenstr = getenv("CONTENT_LENGTH");
#endif // WIN32
	if (lenstr == NULL)
	{
		code = E_FAIE;
		errMsg << "Error, CONTENT_LENGTH should be entered!";
	}
	else
	{
		int len = atoi(lenstr);
		std::shared_ptr<char> postPtr = std::make_shared<char>(len + 2);
		fgets(postPtr.get(), len + 1, stdin);
		return postPtr.get();
	}
	return "";
}

bool IsRunning()
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

#endif // WIN32

}

void OnFirstProcessRun()
{
	std::cout<<"First process"<<std::endl;
	CCommunicatPipe pipe(RESQUEST_PIPE);
	std::cout<<"First process_1"<<std::endl;
	std::shared_ptr<IPipeCallBack> spCallBack(static_cast<IPipeCallBack*>(new PipeRequestCallback()));
	pipe.SetCallBack(spCallBack);
	pipe.StartRead();
}

void OnSecondProcessRun()
{
	auto data = GetPostData();
	Pipe::PipeWrite(RESQUEST_PIPE, data);
	
	std::cout<<"Second process"<<std::endl;
	CCommunicatPipe pipe(RESPONSE_PIPE);
	std::cout<<"Second process_1"<<std::endl;

	std::shared_ptr<IPipeCallBack> spCallBack(static_cast<IPipeCallBack*>(new PipeResponse()));
	pipe.SetCallBack(spCallBack);
	pipe.StartRead();

}

int main()
{
	std::cout<<"Started process"<<std::endl;
	
	if (!IsRunning())
	{
		OnFirstProcessRun();
	}
	else
	{
		OnSecondProcessRun();
#ifdef WIN32
		system("pause");
#endif // WIN32
	}
	return 0;
}
