#include "goz_parser.h"

goz_parser::goz_parser(const std::filesystem::path& file_path) : m_file_path(file_path)
{
	m_file_handle.open(m_file_path, std::ios::in | std::ifstream::binary);

	if (!m_file_handle.is_open())
	{
		throw std::runtime_error("Unable to open file");
	}
}

goz_parser::~goz_parser()
{
	m_file_handle.close();
}

template <typename DataType>
void goz_parser::read_data(std::vector<DataType>& data_vector, const block_flag flag)
{
	if (m_header_map.count(flag) == 0) return; // No data

	data_vector.clear();
	data_vector.resize(m_header_map[flag].data_count);

	m_file_handle.seekg(m_header_offset[flag]);
	m_file_handle.read(reinterpret_cast<char*>(data_vector.data()), m_header_map[flag].data_length);
}

goz_data goz_parser::parse(const std::filesystem::path& file_path)
{
	goz_data data;
	goz_parser parser(file_path);
	parser.assert_valid_magic_number();
	parser.seek_headers();
	parser.assert_valid_headers();
	parser.read(data);

	return data;
}

void goz_parser::read(goz_data& data)
{
	data.polygon_count = m_header_map.at(block_flag::polygon_indices).data_count;
	data.vertex_count = m_header_map.at(block_flag::vertex_coordinates).data_count;

	// Read name
	data.name.resize(m_header_map.at(block_flag::name).data_length);
	m_file_handle.seekg(magic_length + header_length, std::ios::beg);
	m_file_handle.read(data.name.data(), m_header_map.at(block_flag::name).data_length);

	// Required Buffer Data
	read_data(data.vertex_coordinates, block_flag::vertex_coordinates);
	read_data(data.polygon_indices, block_flag::polygon_indices);

	// Optional Buffer Data
	read_data(data.uv_data, block_flag::uv_data);
	read_data(data.vertex_color, block_flag::vertex_color);
	read_data(data.poly_group_ids, block_flag::poly_group_data);
	read_data(data.edge_crease, block_flag::crease_data);
	read_data(data.vertex_mask, block_flag::mask_data);
}

void goz_parser::assert_valid_headers() const
{
	if (m_header_map.count(block_flag::name) == 0)
	{
		throw std::runtime_error("Invalid file content: Found no name");
	}

	if (m_header_map.count(block_flag::vertex_coordinates) == 0)
	{
		throw std::runtime_error("Invalid file content: Found no vertex coordinates");
	}

	if (m_header_map.count(block_flag::polygon_indices) == 0)
	{
		throw std::runtime_error("Invalid file content: Found no polygon indices");
	}
}

void goz_parser::assert_valid_magic_number()
{
	// Set position to start
	m_file_handle.seekg(0);

	// Read 32 bytes
	char magic_number[magic_length];
	m_file_handle.read(magic_number, magic_length);

	bool is_equal = true;

	for (int i = 0; i < magic_length; i++)
	{
		if (magic_number[i] != magic[i])
		{
			is_equal = false;
			break;
		}
	}

	if (!is_equal)
	{
		throw std::runtime_error("Invalid file content: Wrong magic Number");
	}
}

void goz_parser::seek_headers()
{
	const uintmax_t file_size = std::filesystem::file_size(m_file_path);

	if (file_size < (magic_length + header_length))
	{
		throw std::runtime_error("Invalid file content: File too short");
	}

	// Seek 32 bytes from beginning.
	m_file_handle.seekg(magic_length, std::ios::beg);

	while (!m_file_handle.eof())
	{
		uint32_t header_buffer[4];

		m_file_handle.read(reinterpret_cast<char*>(header_buffer), sizeof(header_buffer));
		goz_header header = *reinterpret_cast<goz_header*>(header_buffer);

		// EoF
		if (header.flag == block_flag::end_of_file)
		{
			break;
		}

		if (header.data_length < header_length)
		{
			throw std::runtime_error("Invalid file content: Unexpected block length");
		}

		header.data_length -= header_length;


		const uintmax_t next_read = static_cast<uintmax_t>(m_file_handle.tellg()) + static_cast<uintmax_t>(header_length) + static_cast<uintmax_t>(header.data_length);

		if (next_read > file_size)
		{
			throw std::runtime_error("Invalid file content: Unexpected EoF");
		}

		if ((header.data_length % header.data_count) != 0)
		{
			throw std::runtime_error("Invalid file content: Invalid header data");
		}

		m_header_map.emplace(header.flag, header);
		m_header_offset.emplace(header.flag, m_file_handle.tellg());

		// Skip to next header.
		m_file_handle.seekg(header.data_length, std::ios::cur);
	}
}
