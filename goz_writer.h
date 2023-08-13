#pragma once

#include "goz_core.h"
#include <fstream>
#include <filesystem>

class goz_writer
{
public:
	static void write(const std::filesystem::path& file_path, const goz_data* goz_data);
};
