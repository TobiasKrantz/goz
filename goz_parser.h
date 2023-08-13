#pragma once

#include "goz_core.h"

#include <fstream>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include<iterator>

class goz_parser
{
public:
	static goz_data parse(const std::filesystem::path& file_path);

private:
	explicit goz_parser(const std::filesystem::path& file_path);
	~goz_parser();

	void read(goz_data& data);
	void assert_valid_magic_number();
	void seek_headers();
	void assert_valid_headers() const;

	template <typename DataType>
	void read_data(std::vector<DataType>& data_vector, const block_flag flag);

	std::ifstream m_file_handle;
	std::filesystem::path m_file_path;

	std::unordered_map<block_flag, goz_header> m_header_map;
	std::unordered_map<block_flag, std::streampos> m_header_offset;
};
