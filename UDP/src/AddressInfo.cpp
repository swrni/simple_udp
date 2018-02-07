
#include "AddressInfo.hpp"

namespace AddressInfoSafe {
	
void AddressInfoDeleter::operator()( PADDRINFOW address_info ) {
	FreeAddrInfo( address_info );
}

AddressInfoWrapper ValidateAddress(	const std::wstring& ip,
									const std::wstring& port )
{
	PADDRINFOW result_info = nullptr;
	constexpr const auto hints = Hints();
	GetAddrInfo(
		ip.c_str(),
		port.c_str(),
		&hints,
		&result_info
	);
	return AddressInfoWrapper( result_info );
}


bool Bind( const Socket& socket, PADDRINFOW result_info ) {
	const auto result = bind(
		socket.get(),
		result_info->ai_addr,
		(int)result_info->ai_addrlen
	);
	return ( result == 0 );
}

};

/*AddressInfo::AddressInfo(	const std::wstring& ip,
							const std::wstring& port )
{
	constexpr const auto hints = Hints();
	const auto result = GetAddrInfo(
		ip.c_str(),
		port.c_str(),
		&hints,
		&result_info_
	);
	if ( result != 0 ) {
		Log::Error( WindowsError::ToString() );
		// Log::FatalError( L"Invalid ip (" + ip + L") and/or port (" + port + L")\n" );
	}
}

AddressInfo::~AddressInfo() {
	FreeAddrInfo( result_info_ );
}

bool AddressInfo::Bind( const Socket& socket ) const {
	const auto result = bind(
		socket.get(),
		result_info_->ai_addr,
		(int)result_info_->ai_addrlen
	);
	return ( result == 0 );
}*/