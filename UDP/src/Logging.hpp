#pragma once

#include <iostream>
#include <string>

#include "Common.hpp"

class Log : NoCopy {
	public:
		using LogFunction = void(*)( const std::wstring& );
	public:
		static void FatalError( const std::wstring& msg );
		static void Error( const std::wstring& msg );
		static void setLogFunction( LogFunction log_function );
	private:
		static LogFunction log_function_;
};