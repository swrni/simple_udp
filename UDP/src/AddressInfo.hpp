#pragma once

#include <iostream>
#include <memory>
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

#include "Common.hpp"
#include "Socket.hpp"
#include "Logging.hpp"

namespace AddressInfoSafe {

struct AddressInfoDeleter {
	void operator()( PADDRINFOW );
};

using AddressInfoWrapper = std::unique_ptr<ADDRINFOW, AddressInfoDeleter>;

AddressInfoWrapper ValidateAddress(	const std::wstring& ip,
									const std::wstring& port );

constexpr auto Hints() {
	ADDRINFOW hints = {0};
	hints.ai_protocol = IPPROTO_UDP;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	return hints;
}

bool Bind( const Socket& socket, PADDRINFOW result_info );

};

/*struct AddressInfo : NoCopy {
	private:
	public:
		static constexpr auto Hints() {
			ADDRINFOW hints = {0};
			hints.ai_protocol = IPPROTO_UDP;
			hints.ai_family = AF_UNSPEC;
			hints.ai_socktype = SOCK_DGRAM;
			return hints;
		}
	public:
		AddressInfo(	const std::wstring& ip,
						const std::wstring& port );
		~AddressInfo();

		bool Bind( const Socket& socket ) const;

		constexpr const PADDRINFOW get() const {
			return result_info_;
		}
	private:
		PADDRINFOW result_info_ = nullptr;
};*/