#pragma once

//////////////////////////////////////////////////////////////////////////
// 包含常用头文件
#include <iostream>
#include <assert.h>

// 包含特定头文件
#if defined(_WIN32)
#	include <WinSock2.h>
#	include <Windows.h>
#	include <Shlwapi.h>
#else
#	include <dlfcn.h>
#	include <sys/socket.h>
#	include <netinet/in.h>
#	include <arpa/inet.h>
#endif


//////////////////////////////////////////////////////////////////////////
// 定义DEBUG
#if defined( _DEBUG ) || defined( DEBUG ) || defined (__DEBUG__)
#	ifndef DEBUG
#       define DEBUG
#   endif
#endif

//////////////////////////////////////////////////////////////////////////
// 关闭警告
#if defined(_MSC_VER)
#	pragma warning(disable : 4068)
#	pragma warning(disable : 4251)
#	pragma warning(disable : 4275)
#	pragma warning(disable : 4290)
#	pragma warning(disable : 4819)
#	pragma warning(disable : 4996)
#endif

//////////////////////////////////////////////////////////////////////////
// 定义NULL
#if ! defined(NULL)
#	if defined(__cplusplus) 
#		define NULL 0
#	else
#		define NULL ((void *)0)
#	endif
#endif


//////////////////////////////////////////////////////////////////////////
// 安全释放指针
#if ! defined(SAFE_DELETE)
#	define SAFE_DELETE(p) do{if(p!=NULL){delete p;p=NULL;}}while(0)
#endif

#if ! defined(SAFE_DELETE_ARRAY)
#	define SAFE_DELETE_ARRAY(p) do{if(p!=NULL){delete[] p;p=NULL;}}while(0)
#endif

//////////////////////////////////////////////////////////////////////////
#define endll std::endl << std::endl // double end line definition