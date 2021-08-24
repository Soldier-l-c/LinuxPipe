#include "stdafx.h"
#include "PipeResponseCallBack.h"

void PipeResponse::OnDataRead(const char* data, const int dataLe)
{
	std::cout << "Content-type:text/html\r\n\r\n";
	std::cout << data;
}
