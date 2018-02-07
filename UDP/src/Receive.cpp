#include "Receive.hpp"

#include "AddressInfo.hpp"
#include "Common.hpp"

#include <algorithm>


class Event : NoCopy {
	public:
		Event()
			:	event_( WSACreateEvent() )
		{
			if ( event_ == WSA_INVALID_EVENT ) {
				Log::FatalError( 
					WindowsError::ToString()
				);
			}
		}
		~Event() {
			if ( WSACloseEvent( event_ ) != TRUE ) {
				Log::FatalError( 
					WindowsError::ToString()
				);
			}
		}
		constexpr const WSAEVENT& get() const {
			return event_;
		}
	private:
		const WSAEVENT event_;
};

int WaitForEvents( const std::vector<WSAEVENT>& event_handles, const DWORD ms_to_wait ) {
	const auto event_wait_result = WSAWaitForMultipleEvents(
		(DWORD)event_handles.size(),
		&event_handles[0],
		FALSE,
		ms_to_wait,
		FALSE
	);
	if ( event_wait_result == WSA_WAIT_FAILED ) {
		Log::FatalError( 
			WindowsError::ToString()
		);
	}
	else if ( event_wait_result == WSA_WAIT_TIMEOUT ) {
		return -1;
	}
	return event_wait_result - WSA_WAIT_EVENT_0;
}

struct NetworkEventContext {
	Socket socket_;
	Event event_;
};

WSANETWORKEVENTS EnumerateNetworkEvents( const NetworkEventContext& context ) {
	WSANETWORKEVENTS network_events;
	const auto enum_events_result = WSAEnumNetworkEvents(
		context.socket_.get(),
		context.event_.get(),
		&network_events
	);
	if ( enum_events_result == SOCKET_ERROR ) {
		Log::FatalError(
			WindowsError::ToString()
		);
	}
	return network_events;
}

std::vector<uint8_t> Message::ConstructVector(	const ReceiveBuffer& read_buffer,
												const size_t received_bytes )
{
	std::vector<uint8_t> buffer( received_bytes );
	// std::copy from read_buffer to buffer.
	std::copy(
		std::begin( read_buffer ),
		std::begin( read_buffer ) + received_bytes,
		std::begin( buffer )
	);
	return buffer;
}

int Receive(	const Socket& socket,
				SockAddress& sock_address,
				ReceiveBuffer& receive_buffer )
{
	// std::cout << "[Receive] Receiving data...\n";
	const int bytes_received = recvfrom(
		socket.get(),
		receive_buffer.data(),
		receive_buffer_size,
		0,
		&sock_address.from_,
		&sock_address.from_length_
	);
	if ( bytes_received < 0 ) {
		if ( WSAGetLastError() == WSAEWOULDBLOCK ) {
			Log::Error( L"Socket set to blocking!\n" );
		}
		Log::Error(
			WindowsError::ToString()
		);
	}
	return bytes_received;
}

bool ReceiveData(	const Socket& socket,
					const MessageHandler& message_handler,
					ReceiveBuffer& receive_buffer )
{
	SockAddress sock_address;

	const auto bytes_received = Receive(
		socket,
		sock_address,
		receive_buffer
	);
	if ( bytes_received < 0 ) {
		return false;
	}

	message_handler.Invoke(
		Message(
			sock_address,
			receive_buffer,
			(size_t)bytes_received
		)
	);
	return true;
}

bool ListenPorts( ListenContext context ) {
	const int connections = (int)context.ports_.size();

	if ( connections == 0 ) {
		Log::Error( L"No ports specified!\n" );
		return false;
	}

	std::vector<NetworkEventContext> contexts( connections );
	std::vector<WSAEVENT> event_handles( connections );

	for ( int i=0; i<connections; ++i ) {
		{
			const auto address_info = AddressInfoSafe::ValidateAddress(
				context.ip_,
				context.ports_[i]
			);
			if ( !address_info ) {
				Log::Error( WindowsError::ToString() );
				continue;
			}
			const auto bind_result = AddressInfoSafe::Bind(
				contexts[i].socket_,
				address_info.get()
			);
			if ( !bind_result ) {
				Log::FatalError( WindowsError::ToString() );
				return false;
			}
		}

		const auto event_select_result = WSAEventSelect(
			contexts[i].socket_.get(),
			contexts[i].event_.get(),
			FD_READ
		);
		if ( event_select_result != 0 ) {
			Log::FatalError( 
				WindowsError::ToString()
			);
		}

		event_handles[i] = contexts[i].event_.get();
	}
	
	while ( context.running_ ) {
		const int event_index = WaitForEvents(
			event_handles,
			context.ms_to_wait_
		);
		if ( event_index < 0 ) {
			Log::Error( L"Time out. Stopping...\n" );
			return true;
		}
		const WSANETWORKEVENTS connect_events = EnumerateNetworkEvents(
			contexts[ event_index ]
		);
		
		const bool no_read_event = !( connect_events.lNetworkEvents & FD_READ );
		const bool error_encountered = ( connect_events.iErrorCode[ FD_READ_BIT ] != 0 );
		if ( no_read_event || error_encountered ) {
			Log::FatalError( 
				WindowsError::ToString()
			);
		}
		
		std::array<char, 512> receive_buffer = {0};
		const auto receive_result = ReceiveData(
			contexts[event_index].socket_,
			context.message_handler_,
			receive_buffer
		);
		if ( !receive_result ) {
			return false;
		}
	}
	return true;
}