#pragma once

#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <functional>

class Input
{
public:
	Input(std::string_view fileName)
	{
		m_FilePath = std::string(fileName);

		m_FilePath = std::filesystem::current_path().string() + "/" + m_FilePath.string();

		m_File.open(m_FilePath, std::ios::in);

		if (!m_File.is_open())
		{
			std::string_view str = "Error opening file " + m_FilePath.string();
			throw std::exception(str.data());
		}
	}

	~Input()
	{
		m_File.close();
	}

	template<typename... Ts>
	void GetData(Ts&... values)
	{
		ParseFile();

		(m_FileContents >> ... >> values);

		if (m_FileContents.fail())
		{
			std::string str = "Error reading data from line: " + std::to_string(m_LineNumber);
			throw std::exception(str.c_str());
		}
	}

private:
	void ParseFile()
	{
		m_LineNumber++;
		std::getline(m_File, m_Line);

		if (m_Line[0] == '#' || m_Line.empty())
		{
			m_Line = {};
			ParseFile();
		}
		else
		{
			m_FileContents = std::istringstream(m_Line);
		}
	}

private:
	std::ifstream m_File;
	std::string m_Line;
	std::istringstream m_FileContents;
	std::filesystem::path m_FilePath;
	size_t m_LineNumber = 0;
};

