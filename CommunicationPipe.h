#pragma once
#include "IPipeCallBack.h"
class CCommunicatPipe
{
public:
	CCommunicatPipe(const std::string& pipeName);
	~CCommunicatPipe();
	void StartRead();
	void SetCallBack(std::shared_ptr<IPipeCallBack> ptr);
private:
	void __Read();
	bool __ResidentProcess();
private:
	std::string m_strPipeName;
	int32_t m_nFileId{ 0 };
	bool m_bIsInited{ false };
	std::weak_ptr<IPipeCallBack> m_spCallBack;
};