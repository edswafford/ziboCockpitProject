#pragma once

#include <string>
#include <sstream>
#include <mutex>
#include <fstream>

// log message levels
enum Level
{
	Finest,
	Finer,
	Fine,
	Config,
	Info,
	Warning,
	Severe
};

class logger;

class logstream : public std::ostringstream
{
public:
	logstream(logger& oLogger, Level nLevel);
	logstream(const logstream& ls);
	~logstream() override;

private:
	logger& m_oLogger;
	Level m_nLevel;
};

class logger
{
public:
	logger(std::string filename);
	virtual ~logger();

	void log(Level nLevel, std::string oMessage);
	void flush(){ m_oFile.flush(); }

	logstream operator()();
	logstream operator()(Level nLevel);

private:
	static std::string getTimestamp();

private:
	std::mutex mutex;
	std::ofstream m_oFile;
};
