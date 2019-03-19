#include "OutputYuv.h"

OutputYuv::OutputYuv()
{
}

OutputYuv::~OutputYuv()
{
}

bool OutputYuv::Initial(const std::string &file_path_)
{
	__yuv_file.open(file_path_, std::fstream::out);
	if (!__yuv_file.is_open()) 
	{
		__yuv_file.clear();
		goto error;
	}

	return true;

error:
	return false;
}

bool OutputYuv::WriteYuv(void * data, uint32_t size)
{
	__yuv_file.write((const char*)data, size);

	return true;
}

bool OutputYuv::close()
{
	__yuv_file.close();
	return true;
}
