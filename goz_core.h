#pragma once


/*
GoZ files are really simple.

First 32 bytes are always the string "GoZb 1.0 ZBrush GoZ Binary......" 

After this comes a repeating sequence of a 16 byte "header" followed by that header's corresponding data block. 

The header contains four values.
A "block_flag" enum describing what data is contained.
The length of the data block plus the length of the header.
The count of values stored within the data block.
And the last value I do not know the purpose of. Likely unused.

Lastly comes 16 bytes of zero.
// EoF header.

*/


#include <cstdint>
#include <vector>
#include <string>

constexpr char magic[] = "GoZb 1.0 ZBrush GoZ Binary......";
constexpr uint8_t magic_length = 32;
constexpr uint8_t header_length = 16;

enum class block_flag : uint32_t
{
	end_of_file = 0,
	name = 1,
	vertex_coordinates = 10001,
	polygon_indices = 20001,

	// Optional
	uv_data = 25001,
	vertex_color = 35001,
	mask_data = 30002,
	poly_group_data = 40001,
	crease_data = 40002,
};

// I DON'T KNOW HOW TO HANDLE CREASE AHH
typedef uint8_t crease;

typedef uint16_t poly_group_id;

typedef uint16_t vertex_mask;

struct vertex_color
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a; // unused
};

struct vertex
{
	float x;
	float y;
	float z;
};

struct uv
{
	float u;
	float v;
};

// For triangles uv4 is 0,0
struct polygon_uv
{
	uv uv1;
	uv uv2;
	uv uv3;
	uv uv4;
};

// For triangles v4 is UINT32_MAX.
struct polygon
{
	uint32_t v1;
	uint32_t v2;
	uint32_t v3;
	uint32_t v4;
};

struct goz_header
{
	block_flag flag; // What type of data is stored in the block
	uint32_t data_length; // How many bytes in the block
	uint32_t data_count; // How many items in the block
	uint32_t extra; // Not sure about this one
};

struct goz_data
{
	uint32_t vertex_count = 0;
	uint32_t polygon_count = 0;

	std::string name;

	std::vector<polygon_uv> uv_data;
	std::vector<crease> edge_crease;
	std::vector<vertex_mask> vertex_mask;
	std::vector<vertex_color> vertex_color;
	std::vector<poly_group_id> poly_group_ids;
	std::vector<polygon> polygon_indices;
	std::vector<vertex> vertex_coordinates;
};
