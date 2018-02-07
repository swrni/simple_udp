#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <memory>
// #include <chrono>
#include <thread>
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

#include "Common.hpp"
#include "Logging.hpp"
#include "Serialization.hpp"
#include "Socket.hpp"
#include "Receive.hpp"

// MessageHandler( HandleMessage handle_message = nullptr, const void* saved_arg = nullptr )

struct ThreadContext : NoCopy {
	const std::atomic<bool>& running_;
	const std::vector<std::wstring> ports_;
	// const MessageHandler& message_handler_;
	MessageHandler message_handler_;
	ThreadContext(	const std::atomic<bool>& running,
					const std::vector<std::wstring>& ports,
					MessageHandler message_handler )
				:	running_( running ),
					ports_( ports ),
					message_handler_( message_handler )
	{}
};

void ReceiveFromPorts() {
	std::atomic<bool> running = true;

	const std::vector<std::wstring> ports = { L"27999" };

	const auto handler = []( const void*, const Message& message ) {
		std::cout
			<< "Bytes received:'"
			<< std::to_string( message.receive_buffer_.size() )
			<< "'\n";
		const std::string str_msg(
			(const char*)message.receive_buffer_.data(),
			message.receive_buffer_.size()
		);
		std::cout << "Message received: '" << str_msg << "'\n";
	};

	std::thread t(
		ListenPorts,
		ListenContext{
			running,
			ports,
			MessageHandler( handler )
		}
	);
	t.detach();
	Sleep( 9000 );
	running = false;
}

int wmain( int argc, wchar_t* argv[] ) {
	// Send or receive.
	const WSAStartupWrapper wsa_startup_wrapper;

	const std::wstring ip( L"127.0.0.1" );

	// Print args.
	std::wcout << L"Args:\n";
	for ( int i=0; i<argc; ++i ) {
		std::wcout << L"\t'" << argv[i] << L"'\n";
	}

	if ( ( argc == 2 ) && ( std::wcscmp( argv[1], L"recv" ) == 0 ) ) {
		std::wcout << L"Listening for 9 seconds and terminating after.\n";
		ReceiveFromPorts();
	}
	else if ( ( argc == 4 ) && ( std::wcscmp( argv[1], L"send" ) == 0 ) ) {
		std::wcout << L"Sending...\n";
		const std::wstring data( argv[2] );
		
		const auto data_length = (size_t)data.length();
		std::vector<uint8_t> data_vector( data_length + 1 );
		for ( int i=0; i<data_length; ++i ) {
			data_vector[i] = (uint8_t)data[i];
		}
		data_vector[ data_length ] = '\0';
		
		const std::wstring port( argv[3] );
		std::wcout << L"port:" << port << L"\n";
		const auto bytes_sent = SendData( ip, port, data_vector );
		if ( bytes_sent == 0 ) {
			return 0;
		}
		else {
			std::wcout << L"TODO::\n";
		}
	}
	else {
		std::wcout << L"Unrecognized command!\n";
		std::wcout << L"Pass args 'recv' or 'send some_message 27999'\n";
		return -1;
	}

	std::wcout << L"-.-\n";
	return 0;
}