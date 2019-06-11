#pragma once
#ifndef ES_CORE_UTILS_NETWORK_UTIL_H
#define ES_CORE_UTILS_NETWORK_UTIL_H

#include <string>

namespace Utils
{
	namespace Network
	{

		std::string getIP();
		std::string getStatus();
		bool isIP();
		bool isWifi();

	} // Network::

} // Utils::

#endif // ES_CORE_UTILS_NETWORK_UTIL_H
