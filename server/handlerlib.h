#pragma once
#include <httplib.h>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace httplib;

static std::string GBKToUTF8(const char* strGBK)
{
	int len = MultiByteToWideChar(CP_ACP, 0, strGBK, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, strGBK, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	std::string strTemp = str;

	if (wstr) delete[] wstr;
	if (str) delete[] str;

	return strTemp;
}

static std::string UTF8ToGBK(const char* strUTF8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8, -1, NULL, 0);
	wchar_t* wszGBK = new wchar_t[len + 1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, strUTF8, -1, wszGBK, len);
	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	char* szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
	std::string strTemp(szGBK);

	if (wszGBK) delete[] wszGBK;
	if (szGBK) delete[] szGBK;

	return strTemp;
}


inline std::string Dump_headers(const Headers& headers)
{
	std::string s;
	char buf[BUFSIZ];

	for (auto it = headers.begin(); it != headers.end(); ++it) {
		const auto& x = *it;
		snprintf(buf, sizeof(buf), "%s: %s\n", x.first.c_str(), x.second.c_str());
		s += buf;
	}

	return s;
}

inline std::string Log(const Request& req, const Response& res)
{
	std::string s;
	char buf[BUFSIZ];

	s += "================================\n";

	snprintf(buf, sizeof(buf), "%s %s %s", req.method.c_str(), req.version.c_str(), req.path.c_str());
	s += buf;

	std::string query;
	for (auto it = req.params.begin(); it != req.params.end(); ++it) {
		const auto& x = *it;
		snprintf(buf, sizeof(buf), "%c%s=%s",
			(it == req.params.begin()) ? '?' : '&', x.first.c_str(), x.second.c_str());
		query += buf;
	}
	snprintf(buf, sizeof(buf), "%s\n", query.c_str());
	s += buf;

	s += Dump_headers(req.headers);

	s += "--------------------------------\n";

	snprintf(buf, sizeof(buf), "%d %s\n", res.status, res.version.c_str());
	s += buf;
	s += Dump_headers(res.headers);
	s += "\n";

	if (!res.body.empty()) {
		s += res.body;
	}

	s += "\n";

	return s;
}

inline void Hi(const Request& req,Response& res)
{
	res.set_content("Hello World!\n", "text/plain");
}

inline void Slow(const Request& req, Response& res) 
{
	std::this_thread::sleep_for(std::chrono::seconds(2));
	res.set_content("Slow...\n", "text/plain");
}

inline void Dump(const Request& req, Response& res) 
{
	res.set_content(Dump_headers(req.headers), "text/plain");
}

inline void Set_Error_Handler(const Request& req, Response& res)
{
	const char* fmt = "<p>Error Status: <span style='color:red;'>%d</span></p>";
	char buf[BUFSIZ];
	snprintf(buf, sizeof(buf), fmt, res.status);
	res.set_content(buf, "text/html");
}


inline void FileTest(const Request& req, Response& res)
{
	std::string file_name = "./doc/";
	
	//利用迭代器取出第一个参数(name,value)
	const auto& param_1 = *(req.params.begin());

	//接收文件名，并把字符串编码从UTF-8转为gb2312
	std::string tmp_file_name;
	tmp_file_name = param_1.second;

	std::fstream fs;
	file_name += UTF8ToGBK(tmp_file_name.c_str());
	fs.open(file_name, std::ios::in | std::ios::binary);
	
	if (!fs.is_open())
	{
		std::cout << file_name << std::endl;
		return;
	}
	std::stringstream ss;
	ss << fs.rdbuf();

	res.set_content(ss.str(), "application/octet-stream");
}

inline void CheckLessonList(const Request& req, Response& res)
{
	
}

inline void Set_Logger(const Request& req,const Response& res)
{
	printf("%s", Log(req, res).c_str());
}

