//------------------------------------------------------------------------
// Name:    String.h
// Author:  jjuiddong
// Date:    2012-12-05
// 
// 유니코드, 멀티바이트문자 에 대응할 수 있는 스트링 클래스를 정의한다.
//
// 한글이 잘 변환되기 위해서 반드시 호출해주어야 한다. (아래에 있는 함수)
// std::locale::global(std::locale(""));
//------------------------------------------------------------------------

#pragma once


namespace common
{
	std::string wstring2string(const std::wstring &wstr);
	std::wstring string2wstring(const std::string &str);
	std::string variant2string(const _variant_t &var);

	std::string format(const char* fmt, ...);
	std::wstring formatw(const char* fmt, ...);

}

