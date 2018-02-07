#pragma once

#include <iostream>
#include <string>
// #include <vector>
// #include <array>
// #include <memory>

// #include <chrono>



#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

#include "NoCopy.hpp"

/*struct NoCopy {
	NoCopy() = default;
	NoCopy( const NoCopy& ) = delete;
	const NoCopy& operator=( const NoCopy& ) = delete;
};*/

class WindowsError : NoCopy {
	public:
		static std::wstring ToString() {
			const auto error_code = WSAGetLastError();
			return L"ERROR (" + std::to_wstring( error_code ) + L"):\n" +
				ToString( error_code );
		}
		static std::wstring ToString( const int error_code ) {
			return std::wstring(
				WindowsError( error_code ).get()
			);
		}

	public:
		WindowsError( const int error_code ) {
			FormatMessageW(
				flags_,
				nullptr,
				error_code,
				MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
				(LPWSTR)&message_,
				0,
				nullptr
			);
		}
		~WindowsError() {
			LocalFree( message_ );
		}
		const wchar_t* get() const {
			return message_;
		}

	private:
		wchar_t* message_ = nullptr;
		constexpr static const int flags_ = 
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS;
};

/*struct Log {
	using Logger = void(*)( const std::wstring& );
	static Logger logger_;
	static void Error( const std::wstring& msg ) {
		if ( logger_ ) {
			( *logger_ )( msg );
		}
	}
};

inline void LogError() {
	const auto log_to_console = []( const std::wstring& msg ) {
		std::wcout << msg;
	};
	if ( !Log::logger_ ) {
		Log::logger_ = log_to_console;
	}
	Log::Error( L"Bytes sent < 0" );
	Log::Error( WindowsError::ToString() );
}*/

/*#include "Logging.hpp"
inline void LogError() {
	Log::Error( 
		WindowsError::ToString()
	);
}*/

/*inline std::wstring ErrorMessage( const int error_code ) {
	return std::wstring(
		WindowsError( error_code ).get()
	);

	wchar_t* error_message = nullptr;
	const auto flags =
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS;
	FormatMessageW(
		flags,
		nullptr,
		error_code,
		MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
		(LPWSTR)&error_message,
		0,
		nullptr
	);
	const std::wstring error( error_message );
	LocalFree( error_message );
	return error;
}

inline void LogError() {
	const auto error_code = WSAGetLastError();
	std::wcout << L"ERROR (" << std::to_wstring( error_code ) << L"):\n";
	// std::wcout << L"'" << ErrorMessage( error_code ) << L"'\n";
	std::wcout << WindowsError::ToString( error_code );
	std::terminate();
}*/
