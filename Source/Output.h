#pragma once

#include <string>
#include <filesystem>
#include <fstream>
#include <unordered_map>

class Output
{
public:
	Output(std::string_view fileName)
	{
		m_FilePath = std::string(fileName);

		m_FilePath = std::filesystem::current_path().string() + "/" + m_FilePath.string();

		m_File.open(m_FilePath, std::ios::out);

		if (!m_File.is_open())
		{
			std::string str = "Error opening file " + m_FilePath.string();
			throw std::exception(str.c_str());
		}
	}

	~Output()
	{
		m_File.close();
	}

	template<typename T>
	static void SetFormat(std::string_view format)
	{
		m_FormatMap[typeid(T).hash_code()] = format;
	}

	template<typename... Ts>
	static void SkipLines(int numberOfLines = 1)
	{
		for(int i = 0; i < numberOfLines; i++)
			Write();
	}

	template<typename... Ts>
	static void WriteComment(Ts... values)
	{
		Write("# ", values...);
	}

	template<typename First, typename Second, typename... Rest>
	static constexpr void Write(First value1, Second value2, Rest... values)
	{
		if constexpr(std::is_same_v<const char*, First>)
		{
			m_Padding = strlen(value1);
		}

		Write(value1);
		Write(value2, values...);

		if(sizeof...(values) == 0)
			Write();
	}


private:
	static std::string_view GetFormat(size_t typeIndex)
	{
		for (auto it = m_FormatMap.begin(); it != m_FormatMap.end(); it++)
		{
			if (it->first == typeIndex) return it->second;
		}

		return "{}";
	}

	static void Write() { m_File << '\n'; }

	template<typename Last>
	static void Write(Last value) { m_File << std::vformat(GetFormat(typeid(Last).hash_code()), std::make_format_args(value)); }

	template<typename T, int rows, int cols>
	static void Write(Eigen::Matrix<T, rows, cols> matrix)
	{
		for (int i = 0; i < matrix.rows(); i++)
		{
			if (i > 0)
			{
				for (int k = 0; k < m_Padding; k++)
				{
					m_File << " ";
				}
			}
			for (int j = 0; j < matrix.cols(); j++)
			{
				
				m_File << std::vformat(GetFormat(typeid(T).hash_code()), std::make_format_args(matrix(i, j)));
			}

			m_File << '\n';
		}
	}

private:
	inline static std::ofstream m_File;
	std::filesystem::path m_FilePath;
	inline static std::unordered_map<size_t, std::string_view> m_FormatMap;

	inline static int m_Padding = 0;
};