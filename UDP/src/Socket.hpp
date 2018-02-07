#pragma once

#include <vector>
#include <string>

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
#include "Common.hpp"

struct WSAStartupWrapper : NoCopy {
	WSAStartupWrapper();
	~WSAStartupWrapper() noexcept;
};

struct Socket : NoCopy {
	public:
		Socket();
		Socket( PADDRINFOW address_info );
		~Socket();
		void setBlocking( bool blocking ) const;
		const SOCKET& get() const;
	private:
		SOCKET socket_ = INVALID_SOCKET;
};

struct SockAddress {
	SockAddress()
		:	from_(),
			from_length_( sizeof( from_ ) )
	{}
	sockaddr from_;
	int from_length_;
};

int SendData(	const std::wstring& ip,
				const std::wstring& port,
				const std::vector<uint8_t>& buffer );