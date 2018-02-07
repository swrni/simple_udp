#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <memory>

#include <chrono>



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


void TestNewWriteAndRead() {
	std::vector<uint8_t> data;
	{
		const std::wstring msg_to_write( L"Kvak" );
		std::wcout << L"msg_to_write: " << msg_to_write << L'\n';
		WriteItem( data, msg_to_write );
	}
	std::wstring msg_to_read;
	{
		size_t read_marker = 0;
		ReadItem( data, msg_to_read, read_marker );
	}
	
	std::wcout << L"msg_to_read: " <<  msg_to_read << L'\n';
}

inline void TestWrite() {
	using Type = float;

	std::vector<uint8_t> data_vector;
	const Type item_in = -17.0f;
	WriteToVector( data_vector, item_in );

	size_t read_marker = 0;
	const Type item_out = ReadFromVector<Type>( data_vector, read_marker );
		
	std::wcout << L"Item in: " << std::to_wstring( item_in ) << L'\n';
	std::wcout << L"Item out: " << std::to_wstring( item_out ) << L'\n';

	return;
}

inline void TestWriteAndRead() {
	using Type = std::wstring;

	std::vector<uint8_t> data_vector;
	{
		const Type item_in( L"kvak" );
		std::wcout << L"Item in: " << item_in << L'\n';
		WriteToVector( data_vector, item_in );
	}

	size_t read_marker = 0;
	const Type item_out = ReadFromVector<Type>( data_vector, read_marker );
	
	std::wcout << L"Item out: " << item_out << L'\n';

	return;
}

inline void TestMultiWrite() {
	std::vector<uint8_t> data;
	{
		const uint16_t write_1 = 115;
		const float write_2 = -3.5;
		const uint32_t write_3 = 24;
		
		WriteToVector<uint16_t>( data, write_1 );
		WriteToVector<float>( data, write_2 );
		WriteToVector<uint32_t>( data, write_3 );
	}
	const uint16_t unused=0;
}
