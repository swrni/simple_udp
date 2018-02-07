#include "Socket.hpp"

#include "AddressInfo.hpp"

// WSAStartupWrapper
WSAStartupWrapper::WSAStartupWrapper() {
	WSADATA unused;
	const auto result = WSAStartup(
		MAKEWORD( 2, 2 ),
		&unused
	);
	if ( result != 0 ) {
		Log::FatalError( 
			WindowsError::ToString()
		);
	}
}

WSAStartupWrapper::~WSAStartupWrapper() noexcept {
	if ( WSACleanup() == SOCKET_ERROR ) {
		Log::FatalError( 
			WindowsError::ToString()
		);
	}
}

// Socket
Socket::Socket()
	:	socket_(
			WSASocket(
				AF_UNSPEC,
				SOCK_DGRAM,
				IPPROTO_UDP,
				nullptr,
				0,
				0
			)
		)
{
	if ( socket_ == INVALID_SOCKET ) {
		Log::FatalError( 
			WindowsError::ToString()
		);
	}
}

Socket::Socket( PADDRINFOW address_info ) {
	socket_ = WSASocket(
		address_info->ai_family,
		address_info->ai_socktype,
		address_info->ai_protocol,
		nullptr,
		0,
		0
	);
	if ( socket_ == INVALID_SOCKET ) {
		Log::Error( 
			WindowsError::ToString()
		);
	}
}

Socket::~Socket() {
	closesocket( socket_ );
}

void Socket::setBlocking( bool blocking ) const {
	u_long mode = ( blocking ) ? 0u : 1u;
	if ( ioctlsocket( socket_, FIONBIO, &mode ) != NO_ERROR ) {
		Log::FatalError( 
			WindowsError::ToString()
		);
	}
}

const SOCKET& Socket::get() const {
	return socket_;
}

int SendData(	const std::wstring& ip,
				const std::wstring& port,
				const std::vector<uint8_t>& buffer )
{
	if ( ip.size() == 0 ) {
		Log::Error( L"Ip is empty!" );
		return -1;
	}
	if ( port.size() == 0 ) {
		Log::Error( L"Port is empty!" );
		return -1;
	}
	if ( buffer.size() == 0 ) {
		Log::Error( L"Trying to send empty buffer" );
		return -1;
	}

	/*const auto& address_info = AddressInfo(
		ip,
		port
	);*/
	const auto address_info = AddressInfoSafe::ValidateAddress(
		ip,
		port
	);
	if ( !address_info ) {
		Log::Error( WindowsError::ToString() );
		return -1;
	}
	
	const Socket socket( address_info.get() );
	
	return sendto(
		socket.get(),
		(const char*)buffer.data(),
		(int)buffer.size(),
		0,
		address_info.get()->ai_addr,
		(int)address_info.get()->ai_addrlen
	);
}