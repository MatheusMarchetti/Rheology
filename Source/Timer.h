#pragma once

#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <string>

class Timer
{
public:
	Timer() {}

	void Start() { start = std::chrono::high_resolution_clock::now(); }
	void End() { end = std::chrono::high_resolution_clock::now(); }

	void Now() { now = std::chrono::system_clock::now(); }

	std::string GetCurrentDate()
	{
		auto now = std::chrono::system_clock::now();
		auto in_time_t = std::chrono::system_clock::to_time_t(now);

		std::stringstream ss;
		ss << std::put_time(std::localtime(&in_time_t), "%d-%m-%Y %X");

		return ss.str();
	}

	std::chrono::duration<double> Duration() { return end - start; }

	operator double()
	{
		return Duration().count();
	}

private:
	std::chrono::high_resolution_clock::time_point start;
	std::chrono::high_resolution_clock::time_point end;
	std::chrono::system_clock::time_point now;

};