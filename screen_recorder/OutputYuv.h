#pragma once
#include <fstream>
#include <string>

class OutputYuv 
{
public:
	OutputYuv();
	~OutputYuv();

	bool Initial(const std::string& file_path_);
	bool WriteYuv(void *data, uint32_t size);
	bool close();

private:
	std::string	__file_path;
	std::fstream __yuv_file;
};

