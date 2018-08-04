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

#include "stdafx.h"
#include "ServiceLocator.h"

#pragma endregion


namespace util
{
	std::shared_ptr<Logger<FileLogPolicy>> ServiceLocator::fileLogger = NULL;
	void ServiceLocator::ProvideFileLoggingService(std::shared_ptr<Logger<FileLogPolicy>> providedFileLogger)
	{
		fileLogger = providedFileLogger;
	}
}
