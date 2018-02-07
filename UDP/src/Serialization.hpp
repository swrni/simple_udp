#pragma once

#include <iostream>
#include <vector>

template <typename Item>
bool WriteToVector( std::vector<uint8_t>& out, const Item& in ) {	
	const size_t initial_length = out.size();
	const size_t additional_length = sizeof( Item ) / sizeof( uint8_t );

	out.resize( initial_length + additional_length );
	
	void* destination = &out[ initial_length ];

	const void* source = &in;
	const size_t source_size = sizeof( Item );

	const auto result = memcpy_s(
		destination,
		additional_length,
		source,
		source_size
	);
	return ( result == 0 );
}

template <typename Item>
Item ReadFromVector( const std::vector<uint8_t>& in, size_t& read_marker ) {
	const void* p = &in[ read_marker ];
	read_marker += sizeof( Item );
	return *reinterpret_cast<const Item*>( p );
}

template <typename Item, typename SizeType = uint32_t>
bool WriteItem( std::vector<uint8_t>& data, const Item& item ) {
	// 1. Write item size.
	constexpr const SizeType item_size = (SizeType)sizeof( item );
	if ( !WriteToVector<SizeType>( data, item_size ) ) {
		return false;
	}
	// 2. Write the actual item.
	return WriteToVector<Item>( data, item );
}

template <typename Item, typename SizeType = uint32_t>
bool ReadItem( const std::vector<uint8_t>& data, Item& item, size_t& read_marker ) {
	// 1. Write item size.
	const SizeType item_size = ReadFromVector<SizeType>( data, read_marker );
	
	constexpr const SizeType correct_item_size = (SizeType)sizeof( item );
	if ( item_size != correct_item_size ) {
		return false;
	}

	// 2. Write the actual item.
	item = ReadFromVector<Item>( data, read_marker );
	return true;
}