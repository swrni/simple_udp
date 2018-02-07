#pragma once

#include <vector>
#include <array>
#include <atomic>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

#include "Socket.hpp"

constexpr const int receive_buffer_size = 512;
using ReceiveBuffer = std::array<char, receive_buffer_size>;

class Message {
	public:
		using Data = std::vector<uint8_t>;
	
		Message(	const SockAddress& sock_address,
					const ReceiveBuffer& buffer, 
					const size_t received_bytes )
				:	sock_address_( sock_address ),
					receive_buffer_( ConstructVector( buffer, received_bytes ) )
		{}

		const SockAddress sock_address_;
		const std::vector<uint8_t> receive_buffer_;

	private:
		std::vector<uint8_t> ConstructVector(	const ReceiveBuffer& read_buffer,
												const size_t received_bytes );
};

struct MessageHandler {
	public:
		using HandleMessage = void(*)( const void*, const Message& );
		MessageHandler(	HandleMessage handle_message = nullptr,
						const void* saved_arg = nullptr )
					:	handle_message_( handle_message ),
						saved_arg_( saved_arg )
		{}
		void Invoke( const Message& message ) const {
			handle_message_( saved_arg_, message );
		}
	private:
		HandleMessage handle_message_;
		const void* saved_arg_;
};

struct ListenContext {
	const std::atomic<bool>& running_;
	const std::vector<std::wstring> ports_;
	const MessageHandler message_handler_;
	const DWORD ms_to_wait_ = WSA_INFINITE;
	const wchar_t* ip_ = L"127.0.0.1";
};

bool ListenPorts( ListenContext context );