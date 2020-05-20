#ifndef __LOGGER_HPP_
#define __LOGGER_HPP_

#include <string>
#include <fstream>
#include <iostream>
#include <cstdlib>



namespace core
{
	class logger
	{
		inline static const std::string Logfile = "log.txt";


	public:

		static void log(const std::string& str)
		{
			std::ofstream fout(Logfile, std::ios_base::app);
			fout << str << std::endl;
			fout.close();
		}
	};
}


#endif