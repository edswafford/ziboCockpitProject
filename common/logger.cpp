#define _CRT_SECURE_NO_WARNINGS
#include "Logger.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>

logger::logger(const std::string filename)
{
	m_oFile.open(filename, std::fstream::out | std::fstream::ate); // removed append std::fstream::app |
}

logger::~logger()
{
	m_oFile.flush();
	m_oFile.close();
}

logstream logger::operator()()
{
	return logstream(*this, Info);
}

logstream logger::operator()(const Level nLevel)
{
	return logstream(*this, nLevel);
}

std::string logger::getTimestamp()
{
	using namespace std::chrono;

	// get current time
	auto now = system_clock::now();

	// get number of milliseconds for the current second
	// (remainder after division into seconds)
	auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

	// convert to std::time_t in order to convert to std::tm (broken time)
	auto timer = system_clock::to_time_t(now);

	// convert to broken time
	std::tm bt = *std::localtime(&timer);
#if _MSC_VER < 1900
	return "No Timestamp ";
#else
	std::ostringstream oss;

	oss << std::put_time(&bt, "%T"); // HH:MM:SS
	oss << '.' << std::setfill('0') << std::setw(3) << ms.count();

	return oss.str();
#endif
}

void logger::log(const Level nLevel, const std::string oMessage)
{
	const static char* LevelStr[] = { "Finest", "Finer", "Fine", "Config", "Info", "Warning", "Severe" };

	std::lock_guard<std::mutex> lock(mutex);
	m_oFile << '[' << getTimestamp() << ']'
		<< '[' << LevelStr[nLevel] << "]\t"
		<< oMessage << std::endl;
}

logstream::logstream(logger& oLogger, const Level nLevel) :
	m_oLogger(oLogger), m_nLevel(nLevel)
{
}

logstream::logstream(const logstream& ls) :
	m_oLogger(ls.m_oLogger), m_nLevel(ls.m_nLevel)
{
	// As of GCC 8.4.1 basic_stream is still lacking a copy constructor
	// (part of C++11 specification)
	//
	// GCC compiler expects the copy constructor even thought because of
	// RVO this constructor is never used
}

logstream::~logstream()
{
	m_oLogger.log(m_nLevel, str());
}