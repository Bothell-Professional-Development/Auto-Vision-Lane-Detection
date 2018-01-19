#pragma once

#include <string>
#include <map>

namespace common_lib
{

class ConfigFile
{
public:
	enum class ConfigStatus
	{
		SUCCESS,
		ERROR
	};

public:
	ConfigStatus pullValuesFromFile(const std::string& filePath);
	ConfigStatus pushValuesToFile(const std::string& filePath);

	std::string readValueOrDefault(const std::string& key, const std::string& defaultvalue) const;
	void writeValue(const std::string& key, const std::string& value);

private:
	std::map<std::string, std::string> m_store;
};

};