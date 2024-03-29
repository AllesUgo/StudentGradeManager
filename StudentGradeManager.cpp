﻿// StudentGradeManager.cpp: 定义应用程序的入口点。
//
#include <csignal>
#include <iostream>
#include <regex>
#include <csetjmp>
#include "rbslib/Network.h"
#include "rbslib/Storage.h"
#include "module_loader/loader.h"
#include "log/logger.h"
#include "configuration_manager/configuration_manager.h"
#include "rbslib/FileIO.h"
#include <memory>
#include <execinfo.h>
using namespace std;

static std::jmp_buf buf;
static bool IsExitProgram = false;

static void ExitProgram();
static void SignalHandle(int sig);

void PrintTrace(void)
{
	void* array[10];
	size_t size;
	char** strings;
	size_t i;

	size = backtrace(array, 10);
	strings = backtrace_symbols(array, size);

	Logger::LogError("Obtained %zd stack frames.", size);

	for (i = 0; i < size; i++)
	{
		Logger::LogError("%s", strings[i]);
	}

	free(strings);
}

static void SignalHandle(int sig)
{
	switch (sig)
	{
	case SIGSEGV:
		Logger::LogError("段错误，程序将尝试退出");
		PrintTrace();
		ExitProgram();
		break;
	case SIGINT:
		ExitProgram();
		break;
	default:
		break;
	}
}

static void ExitProgram()
{
	Logger::LogInfo("正在退出服务器");
	IsExitProgram = true;
	longjmp(buf, 1);
	fcloseall();
}

int main()
{
	std::signal(SIGPIPE, SignalHandle);
	std::signal(SIGINT, SignalHandle);
	std::signal(SIGSEGV, SignalHandle);

	//读取配置文件
	Logger::LogInfo("读取配置文件");
	try
	{
		Config::LoadConfig();
	}
	catch (std::exception const& ex)
	{
		Logger::LogError("读取配置文件失败: %s", ex.what());
		Logger::LogWarn("使用默认设置启动");
		Logger::LogInfo("保存默认的配置文件");
		Config::SaveConfig();
	}
	Logger::LogInfo("端口设置为: %d", Config::Port());
	RbsLib::Network::HTTP::HTTPServer server(Config::Port());
	Module::ModuleManager module_manager;
	Logger::LogInfo("模块路径设置为: %s", Config::ModulePath().c_str());
	module_manager.LoadModules(RbsLib::Storage::StorageFile(Config::ModulePath()));
	server.SetGetHandle([&module_manager](const RbsLib::Network::TCP::TCPConnection& x, RbsLib::Network::HTTP::RequestHeader& h) {
		//检查路径是否为根路径
		if (h.path == "/")
		{
			//构造302跳转
			RbsLib::Network::HTTP::ResponseHeader header;
			header.status = 302;
			header.headers.AddHeader("Location", "/html/login.html");
			x.Send(header.ToBuffer());
			return;
		}
		std::cmatch m;
		RbsLib::Network::HTTP::ResponseHeader header;
		header.status = 404;
		header.status_descraption = "not found";
		header.headers.AddHeader("Content-Length", "3");
		header.headers.AddHeader("Content-Type", "text/plain");
		RbsLib::Buffer not_found_content(4);
		not_found_content.SetData("404", 3);

		if (true == std::regex_match(h.path.c_str(), m, std::regex("^/app/(\\w+).(\\w+)$")))
		{
			try
			{
				module_manager.GetAction(m[1], m[2])(RbsLib::Network::HTTP::Request(x, h, RbsLib::Buffer(1)));
			}
			catch (Module::ModuleLoaderException const& ex)
			{
				Logger::LogWarn("模块%s.%s抛出异常:%s", m[1].str().c_str(), m[2].str().c_str(), ex.what());
				x.Send(header.ToBuffer());
				x.Send(not_found_content);
			}
		}
		else if (true == std::regex_match(h.path.c_str(), m, std::regex("^/html/(\\S+)$")))
		{
			try
			{
				auto buffer = RbsLib::Storage::StorageFile("html")[RbsLib::Storage::StorageFile(m[1]).GetName()].Open(RbsLib::Storage::FileIO::OpenMode::Read,
					RbsLib::Storage::FileIO::SeekBase::begin).Read(static_cast<int64_t>(1024) * 1024);
				RbsLib::Network::HTTP::ResponseHeader header;
				header.status = 200;
				header.status_descraption = "ok";
				header.headers.AddHeader("Content-Length", std::to_string(buffer.GetLength()));
				header.headers.AddHeader("Content-Type", "text/html");
				x.Send(header.ToBuffer());
				x.Send(buffer);
			}
			catch (const std::exception& ex)
			{
				Logger::LogWarn("加载网页%s时抛出异常:%s", m[1].str().c_str(), ex.what());
				x.Send(header.ToBuffer());
				x.Send(not_found_content);
			}
		}
		else
		{
			x.Send(header.ToBuffer());
			x.Send(not_found_content);
		}
		});
	server.SetPostHandle([&module_manager](const RbsLib::Network::TCP::TCPConnection& x, RbsLib::Network::HTTP::RequestHeader& h, const RbsLib::Buffer& buffer) {
		std::cmatch m;
		RbsLib::Network::HTTP::ResponseHeader header;
		header.status = 404;
		header.status_descraption = "not found";
		header.headers.AddHeader("Content-Length", "3");
		header.headers.AddHeader("Content-Type", "text/plain");
		RbsLib::Buffer not_found_content(4);
		not_found_content.SetData("404", 3);
		auto debug = buffer.ToString();

		if (true == std::regex_match(h.path.c_str(), m, std::regex("^/app/(\\w+).(\\w+)$")))
		{
			try
			{
				module_manager.GetAction(m[1], m[2])(RbsLib::Network::HTTP::Request(x, h, buffer));
			}
			catch (Module::ModuleLoaderException const& ex)
			{
				Logger::LogWarn("模块%s.%s抛出异常:%s", m[1].str().c_str(), m[2].str().c_str(), ex.what());
				x.Send(header.ToBuffer());
				x.Send(not_found_content);
			}
			catch (const std::exception& ex)
			{
				Logger::LogWarn("已引发异常:%s", ex.what());
			}
		}
		else
		{
			x.Send(header.ToBuffer());
			x.Send(not_found_content);
		}

		});
	try
	{
		if (!setjmp(buf))
		{
			server.LoopWait();
		}
	}
	catch (const std::exception& ex)
	{
		Logger::LogError("HTTP服务核心错误: %s", ex.what());
	}

	Logger::LogInfo("程序已退出");
	return 0;
}
