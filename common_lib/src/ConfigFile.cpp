#include "ConfigFile.h"

#include <fstream>
#include <iostream>
#include <memory>

#define delimiter ':'

using namespace common_lib;

ConfigFile::ConfigStatus ConfigFile::pullValuesFromFile(const std::string & filePath)
{
	std::ifstream file(filePath, std::ios::in);
	
	if (file.is_open())
	{
		for(std::string line; std::getline(file, line); )
		{
			std::string::size_type pos = std::string::npos;
			if ((pos = line.find(delimiter)) != std::string::npos)
			{
				m_store[ line.substr(0, pos)] = line.substr(pos+1, line.length()-1);
			}
		}

		return ConfigFile::ConfigStatus::CONFIG_SUCCESS;
	}

	return ConfigFile::ConfigStatus::CONFIG_ERROR;
}

ConfigFile::ConfigStatus ConfigFile::pushValuesToFile(const std::string & filePath)
{
	std::ofstream file(filePath, std::ios::out);

	if (file.is_open())
	{
		for (std::map<std::string, std::string>::iterator it = m_store.begin(); it != m_store.end(); ++it)
		{
			file << it->first << delimiter << it->second << "\n";
		}

		return ConfigFile::ConfigStatus::CONFIG_SUCCESS;
	}

	return ConfigFile::ConfigStatus::CONFIG_ERROR;
}

std::string ConfigFile::readValueOrDefault(const std::string& key, const std::string& defaultvalue) const
{
	if (m_store.find(key) != m_store.end())
	{
		return m_store.at(key);
	}

	return defaultvalue;
}

void ConfigFile::writeValue(const std::string& key, const std::string& value)
{
	m_store[key] = value;
}
