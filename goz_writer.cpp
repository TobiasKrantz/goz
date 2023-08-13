#include "goz_writer.h"

template <typename DataType>
void write_buffer(std::ofstream& file_handle, const std::vector<DataType>& data_vector, const block_flag flag)
{
	uint32_t byte_length = data_vector.size() * sizeof(DataType);
	byte_length += header_length;

	const goz_header header = {
		flag,
		byte_length,
		data_vector.size(),
		0
	};


	file_handle.write(reinterpret_cast<const char*>(&header), sizeof(goz_header));
	file_handle.write(reinterpret_cast<const char*>(data_vector.data()), data_vector.size() * sizeof(DataType));
}

void write_name(std::ofstream& file_handle, const std::string& name)
{
	uint32_t byte_length = name.size();
	byte_length += header_length;

	const goz_header header = {
		block_flag::name,
		byte_length,
		1,
		0
	};

	file_handle.write(reinterpret_cast<const char*>(&header), sizeof(goz_header));
	file_handle.write(name.data(), name.size());
}


void goz_writer::write(const std::filesystem::path& file_path, const goz_data* goz_data)
{
	std::ofstream file_handle;

	file_handle.open(file_path, std::ifstream::binary);

	if (!file_handle.is_open())
	{
		throw std::runtime_error("Unable to open file");
	}

	if (goz_data->vertex_coordinates.empty())
	{
		throw std::runtime_error("Uninitialized Vertex goz_data");
	}

	if (goz_data->polygon_indices.empty())
	{
		throw std::runtime_error("Uninitialized Polygon goz_data");
	}

	file_handle.write(magic, magic_length);
	write_name(file_handle, goz_data->name);

	write_buffer(file_handle, goz_data->vertex_coordinates, block_flag::vertex_coordinates);
	write_buffer(file_handle, goz_data->polygon_indices, block_flag::polygon_indices);
	write_buffer(file_handle, goz_data->uv_data, block_flag::uv_data);
	write_buffer(file_handle, goz_data->vertex_color, block_flag::vertex_color);
	write_buffer(file_handle, goz_data->vertex_mask, block_flag::mask_data);
	write_buffer(file_handle, goz_data->poly_group_ids, block_flag::poly_group_data);
	write_buffer(file_handle, goz_data->edge_crease, block_flag::crease_data);

	constexpr char eof_header[header_length] = {0};
	file_handle.write(eof_header, header_length);

	file_handle.close();
}
