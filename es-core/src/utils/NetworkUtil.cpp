#include "utils/NetworkUtil.h"

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

namespace Utils
{
	namespace FileSystem
	{
		
		bool isIP()
		{
			FILE *wIPP;
			char wip[1035];
			
			std::string wStat;
			wIPP = popen("hostname -I", "r");
			while (fgets(wip, sizeof(wip), wIPP) != NULL) {
				wStat = wip;
				int trim = wStat.find("\n");
				wStat = wStat.substr(0, trim);
				if(wStat == "" | wStat == " "){
					return false;
				}else{
					return true;
				}
			}
			return false;
		}

		bool isWifi()
		{
			FILE *wifiOnOff;
			char wi[1035];
			bool flagWifi;
			std::string currentLine;
			std::size_t found;
			flagWifi = false;
			wifiOnOff = popen("ifconfig wlan0 | grep 'flags='", "r");
			while (fgets(wi, sizeof(wi), wifiOnOff) != NULL) {
				currentLine = wi;
				found = currentLine.find("RUNNING");
				if (found != std::string::npos) {
					flagWifi = true;
				}
			}
			return flagWifi;
		}
		
		std::string getIP()
		{
			FILE *wIPP;
			char wip[1035];
			std::string currentLine;
			std::size_t found;
			
			std::string wIP;
			wIPP = popen("hostname -I", "r");
			while (fgets(wip, sizeof(wip), wIPP) != NULL) {
				wIP = wip;
				found = wIP.find(".");
				if (found != std::string::npos) {
					int trim = wIP.find("\n");
					wIP = wIP.substr(0, trim-1);
				}else{
					wIP = "NOT CONNECTED";
				}
			}
			return wIP;
		}

		std::string getStatus()
		{
			bool ip;
			std::string wStatText;
			ip = isIP();
			
			if(ip){
				wStatText = "CONNECTED";
			}else{
				wStatText = "NOT CONNECTED";
			}
			return wStatText;
		}
	}
}