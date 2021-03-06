#pragma once
#ifndef ES_CORE_UTILS_NETWORK_UTIL_H
#define ES_CORE_UTILS_NETWORK_UTIL_H

#include <string>

namespace Utils
{
	namespace Network
	{

		bool isIP();
		bool isWifi();
		std::string getIP();
		std::string getStatus();

	} // Network::

} // Utils::

#endif // ES_CORE_UTILS_NETWORK_UTIL_H
