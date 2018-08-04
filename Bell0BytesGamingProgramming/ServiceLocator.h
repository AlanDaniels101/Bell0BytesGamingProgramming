#pragma once

#pragma region "Description"

/*******************************************************************************************************************************
* Service locator
*
* Implementation of the Service Locator design pattern
* - http://gameprogrammingpatterns.com/service-locator.html
*
* Access a service without being coupled to the class that implements the service
*
********************************************************************************************************************************/

#pragma endregion

#pragma region "Includes"

#include "Log.h"

#pragma endregion

namespace util
{
	class ServiceLocator
	{
	public:
		static Logger<FileLogPolicy>* GetFileLogger() { return fileLogger.get(); };
		static void ProvideFileLoggingService(std::shared_ptr<Logger<FileLogPolicy>> providedFileLogger);
	private:
		static std::shared_ptr<Logger<FileLogPolicy>> fileLogger;
	};
}

