#pragma once

#pragma region "Description"

/*******************************************************************************************************************************
* Logging utility
*
* Log daemon, log interface, severity definitions, file logger
*
********************************************************************************************************************************/

#pragma endregion

#pragma region "Includes"

#include "stdafx.h"

#pragma endregion

namespace util
{
	// Interface to [open / close / write to] log streams
	class LogPolicyInterface
	{
	public:
		virtual ~LogPolicyInterface() noexcept = default;

		virtual bool OpenOutputStream(const std::wstring& name) = 0;
		virtual void CloseOutputStream() = 0;
		virtual void Write(const std::string& msg) = 0;
	};

	// File logging policy
	class FileLogPolicy : LogPolicyInterface
	{
	public:
		FileLogPolicy() : outputStream() {};
		~FileLogPolicy() {};

		bool OpenOutputStream(const std::wstring& filename) override;
		void CloseOutputStream() override;
		void Write(const std::string& msg) override;

	private:
		std::ofstream outputStream;
	};

	// Log levels
	enum SeverityType
	{
		info = 0,
		debug,
		warning,
		error,
		config,
	};

	// Logging daemon
	template<typename LogPolicy>
	class Logger;

	template<typename LogPolicy>
	void LoggingDaemon(Logger<LogPolicy>* logger)
	{
		std::unique_lock<std::timed_mutex> lock(logger->writeMutex, std::defer_lock);
		do
		{
			std::this_thread::sleep_for(std::chrono::milliseconds{ 50 });
			if (logger->logBuffer.size())
			{
				if (!lock.try_lock_for(std::chrono::milliseconds{ 50 }))
					continue;
				for (auto& x : logger->logBuffer)
					logger->policy.Write(x);
				logger->logBuffer.clear();
				lock.unlock();
			}
		} while (logger->isStillRunning.test_and_set() || logger->logBuffer.size());
	}

	// Logger
	template<typename LogPolicy>
	class Logger
	{
	public:
		Logger(const std::wstring& name);
		~Logger();

		void SetThreadName(const std::string& name);

		template<SeverityType severity>
		void Print(std::stringstream stream);

		template<SeverityType severity>
		void Print(std::string stream);

		template<typename Policy>
		friend void LoggingDaemon(Logger<Policy>* logger);
	private:
		unsigned int logLineNumber;								// used to save the current line number
		std::map<std::thread::id, std::string> threadName;		// defines a human-readable name for each thread
		LogPolicy policy;										// the log policy (i.e. write to file, ...)
		std::timed_mutex writeMutex;							// mutual exclusive writer
		std::vector<std::string> logBuffer;						// the content to log
		std::thread daemon;										// the actual logging daemon
		std::atomic_flag isStillRunning{ ATOMIC_FLAG_INIT };	// lock-free boolean to check whether our daemon is still running or not
	};

	template<typename LogPolicy>
	Logger<LogPolicy>::Logger(const std::wstring& name) :
		logLineNumber(0),
		threadName(),
		policy(),
		writeMutex(),
		logBuffer()
	{
		if (policy.OpenOutputStream(name))
		{
			isStillRunning.test_and_set();	// mark the logging daemon as running
			daemon = std::move(std::thread{ LoggingDaemon<LogPolicy>, this });
		}
		else
		{
			throw std::runtime_error("Unable to open log file!");
		}
	}

	template<typename LogPolicy>
	Logger<LogPolicy>::~Logger()
	{
#ifndef NDEBUG
		// print closing message
		util::ServiceLocator::GetFileLogger()->Print<util::SeverityType::info>("The file logger was shut down.");
#endif
		// terminate the daemon by clearing the still running flag and letting it join to the main thread
		isStillRunning.clear();
		daemon.join();

		// clear the thread name map
		threadName.clear();
		std::map<std::thread::id, std::string>().swap(threadName);

		// clear the log vector
		logBuffer.clear();
		logBuffer.shrink_to_fit();

		// close the output stream
		policy.CloseOutputStream();
	}

	template<typename LogPolicy>
	void Logger<LogPolicy>::SetThreadName(const std::string& name)
	{
		threadName[std::this_thread::get_id()] = name;
	}

	template<typename LogPolicy>
	template<SeverityType severity>
	void Logger<LogPolicy>::Print(std::stringstream stream)
	{
		std::stringstream logStream;

		if (!(severity == SeverityType::config))
		{
			SYSTEMTIME localTime;
			GetLocalTime(&localTime);

			// Log header: log#: MM/dd/yyyy hh:mm:ss
			if (logLineNumber != 0)
			{
				logStream << "\r\n";
			}
			logStream << logLineNumber++ << ": " << localTime.wMonth << "/" << localTime.wDay << "/" << localTime.wYear << " " << localTime.wHour << ":" << localTime.wMinute << ":" << localTime.wSecond << "\t";

			// Log warning level
			switch (severity)
			{
			case SeverityType::info:
				logStream << "INFO:    ";
				break;
			case SeverityType::debug:
				logStream << "DEBUG:   ";
				break;
			case SeverityType::warning:
				logStream << "WARNING: ";
				break;
			case SeverityType::error:
				logStream << "ERROR:   ";
				break;
			}

			// Log thread name
			logStream << threadName[std::this_thread::get_id()] << ":\t";
		}

		// Log message
		logStream << stream.str();
		std::lock_guard<std::timed_mutex> lock(writeMutex);
		logBuffer.push_back(logStream.str());
	}

	template<typename LogPolicy>
	template<SeverityType severity>
	void Logger<LogPolicy>::Print(std::string msg)
	{
		std::stringstream stream;
		stream << msg.c_str();
		this->Print<severity>(std::stringstream(stream.str()));
	}
}


