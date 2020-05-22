#ifndef __READ_FILE_HPP_
#define __READ_FILE_HPP_

#include <fstream>
#include <string>
#include <cerrno>

namespace util
{
	//http://insanecoding.blogspot.com/2011/11/how-to-read-in-file-in-c.html
	std::string read_file(const std::string& filepath)
	{
		std::ifstream in(filepath, std::ios::in | std::ios::binary);
		if (in)
		{
			std::string contents;
			in.seekg(0, std::ios::end);
			contents.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&contents[0], contents.size());
			in.close();
			return(contents);
		}
		throw(errno);
	}
}


#endif