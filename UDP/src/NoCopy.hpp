#pragma once

struct NoCopy {
	NoCopy() = default;
	NoCopy( const NoCopy& ) = delete;
	const NoCopy& operator=( const NoCopy& ) = delete;
};