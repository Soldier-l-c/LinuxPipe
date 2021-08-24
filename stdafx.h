#pragma once
#ifndef STDAFX
#define STDAFX

#include <iostream>
#include <sstream>
#include <thread>
#include <string>
//#include <shared_ptr>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <rapidjson/istreamwrapper.h>

#ifdef WIN32
#include <Windows.h>
#ifndef _UNISTD_H
#define _UNISTD_H
#include <io.h>
#include <process.h>
#endif /* _UNISTD_H */
#else 
#include <unistd.h>
#define _read read
#define _write write
#define _open open
#define _close close
#define _unlink unlink
#define int32_t int
#endif // WIN32

#define CHECK_NULL_PTR(ptr)\
	if(!ptr)return false;
#endif // !STDAFX

