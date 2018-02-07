#include "Logging.hpp"

void LogToConsole( const std::wstring& msg ) {
	std::wcout << msg;
}

Log::LogFunction Log::log_function_ = LogToConsole;

void Log::FatalError( const std::wstring& msg ) {
	log_function_( msg );
	log_function_( L"Terminating..." );
	std::terminate();
}
void Log::Error( const std::wstring& msg ) {
	log_function_( msg );
}

void Log::setLogFunction( LogFunction log_function ) {
	log_function_ = log_function;
}