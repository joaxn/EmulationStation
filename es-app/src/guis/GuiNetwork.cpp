#include "guis/GuiNetwork.h"

#include "components/OptionListComponent.h"
#include "components/SliderComponent.h"
#include "components/SwitchComponent.h"
#include "components/SliderComponent.h"
#include "guis/GuiMsgBox.h"
#include "guis/GuiKeyboard.h"
#include "guis/GuiTextEditPopupKeyboard.h"
#include "guis/GuiSettings.h"
#include "guis/GuiWifiConnect.h"
#include "views/UIModeController.h"
#include "views/ViewController.h"
#include "utils/NetworkUtil.h"
#include "Scripting.h"
#include "SystemData.h"
#include "Log.h"
#include <SDL_events.h>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <map>

GuiNetwork::GuiNetwork(Window* window) : GuiComponent(window), mMenu(window, "NETWORK SETTINGS"), mTimer(0), mState(0)
{
	ComponentListRow row;
	
	// STATUS
	auto title = std::make_shared<TextComponent>(mWindow, "STATUS", Font::get(FONT_SIZE_MEDIUM), 0x777777FF);
	std::string wStatText = Utils::Network::getStatus();
	updateStat = std::make_shared<TextComponent>(mWindow, "" + wStatText, Font::get(FONT_SIZE_MEDIUM), 0x777777FF);
	updateStat->setHorizontalAlignment(ALIGN_RIGHT);
	row.addElement(title, true);
	row.addElement(updateStat, true);
	mMenu.addRow(row);
	row.elements.clear();

	// IP
	title = std::make_shared<TextComponent>(mWindow, "IP ADDRESS", Font::get(FONT_SIZE_MEDIUM), 0x777777FF);
	std::string wIP = Utils::Network::getIP();
	updateIP = std::make_shared<TextComponent>(mWindow, "" + wIP, Font::get(FONT_SIZE_MEDIUM), 0x777777FF);
	updateIP->setHorizontalAlignment(ALIGN_RIGHT);
	row.addElement(title, true);
	row.addElement(updateIP, true);
	mMenu.addRow(row);
	row.elements.clear();
	
	//WIFI COUNTRY
	title = std::make_shared<TextComponent>(mWindow, "WIFI COUNTRY", Font::get(FONT_SIZE_MEDIUM), 0x777777FF);
	wifi_countries = std::make_shared< OptionListComponent<std::string> >(mWindow, "WIFI COUNTRY", false);
	std::map<std::string,std::string> countryMap;
	countryMap["US"] = "United States";
	countryMap["CA"] =	"Canada";
	countryMap["JP3"] =	"Japan";
	countryMap["DE"] =	"Germany";
	countryMap["NL"] =	"Netherlands";
	countryMap["IT"] =	"Italy";
	countryMap["PT"] =	"Portugal";
	countryMap["LU"] =	"Luxembourg";
	countryMap["NO"] =	"Norway";
	countryMap["FI"] =	"Finland";
	countryMap["DK"] =	"Denmark";
	countryMap["CH"] =	"Switzerland";
	countryMap["CZ"] =	"Czech Republic";
	countryMap["ES"] =	"Spain";
	countryMap["GB"] =	"United Kingdom";
	countryMap["KR"] =	"South Korea";
	countryMap["CN"] =	"China";
	countryMap["FR"] =	"France";
	countryMap["HK"] =	"Hong Kong";
	countryMap["SG"] =	"Singapore";
	countryMap["TW"] =	"Taiwan";
	countryMap["BR"] =	"Brazil";
	countryMap["IL"] =	"Israel";
	countryMap["SA"] =	"Saudi Arabia";
	countryMap["LB"] =	"Lebanon";
	countryMap["AE"] =	"United Arab Emirates";
	countryMap["ZA"] =	"South Africa";
	countryMap["AR"] =	"Argentina";
	countryMap["AU"] =	"Australia";
	countryMap["AT"] =	"Austria";
	countryMap["BO"] =	"Bolivia";
	countryMap["CL"] =	"Chile";
	countryMap["GR"] =	"Greece";
	countryMap["IS"] =	"Iceland";
	countryMap["IN"] =	"India";
	countryMap["IE"] =	"Ireland";
	countryMap["KW"] =	"Kuwait";
	countryMap["LI"] =	"Liechtenstein";
	countryMap["LT"] =	"Lithuania";
	countryMap["MX"] =	"Mexico";
	countryMap["MA"] =	"Morocco";
	countryMap["NZ"] =	"New Zealand";
	countryMap["PL"] =	"Poland";
	countryMap["PR"] =	"Puerto Rico";
	countryMap["SK"] =	"Slovak Republic";
	countryMap["SI"] =	"Slovenia";
	countryMap["TH"] =	"Thailand";
	countryMap["UY"] =	"Uruguay";
	countryMap["PA"] =	"Panama";
	countryMap["RU"] =	"Russia";
	countryMap["KW"] =	"Kuwait";
	countryMap["LI"] =	"Liechtenstein";
	countryMap["LT"] =	"Lithuania";
	countryMap["MX"] =	"Mexico";
	countryMap["MA"] =	"Morocco";
	countryMap["NZ"] =	"New Zealand";
	countryMap["PL"] =	"Poland";
	countryMap["PR"] =	"Puerto Rico";
	countryMap["SK"] =	"Slovak Republic";
	countryMap["SI"] =	"Slovenia";
	countryMap["TH"] =	"Thailand";
	countryMap["UY"] =	"Uruguay";
	countryMap["PA"] =	"Panama";
	countryMap["RU"] =	"Russia";
	countryMap["EG"] =	"Egypt";
	countryMap["TT"] =	"Trinidad & Tobago";
	countryMap["TR"] =	"Turkey";
	countryMap["CR"] =	"Costa Rica";
	countryMap["EC"] =	"Ecuador";
	countryMap["HN"] =	"Honduras";
	countryMap["KE"] =	"Kenya";
	countryMap["UA"] =	"Ukraine";
	countryMap["VN"] =	"Vietnam";
	countryMap["BG"] =	"Bulgaria";
	countryMap["CY"] =	"Cyprus";
	countryMap["EE"] =	"Estonia";
	countryMap["MU"] =	"Mauritius";
	countryMap["RO"] =	"Romania";
	countryMap["CS"] =	"Serbia & Montenegro";
	countryMap["ID"] =	"Indonesia";
	countryMap["PE"] =	"Peru";
	countryMap["VE"] =	"Venezuela";
	countryMap["JM"] =	"Jamaica";
	countryMap["BH"] =	"Bahrain";
	countryMap["OM"] =	"Oman";
	countryMap["JO"] =	"Jordan";
	countryMap["BM"] =	"Bermuda";
	countryMap["CO"] =	"Colombia";
	countryMap["DO"] =	"Dominican Republic";
	countryMap["GT"] =	"Guatemala";
	countryMap["PH"] =	"Philippines";
	countryMap["LK"] =	"Sri Lanka";
	countryMap["SV"] =	"El Salvador";
	countryMap["TN"] =	"Tunisia";
	countryMap["PK"] =	"Pakistan";
	countryMap["QA"] =	"Qatar";
	countryMap["DZ"] =	"Algeria";
	for (map<std::string, std::string>::iterator p = countryMap.begin(); p != countryMap.end(); ++p ) {
		wifi_countries->add(p->second, p->first, Settings::getInstance()->getString("WifiCountry") == p->first);
	}
	row.addElement(title, true);
	row.addElement(wifi_countries, true);
	mMenu.addRow(row);
	row.elements.clear();
		
	
	//SSID
	title = std::make_shared<TextComponent>(mWindow, "NETWORK (SSID)", Font::get(FONT_SIZE_MEDIUM), 0x777777FF);
	auto editSSID = std::make_shared<TextComponent>(mWindow, Settings::getInstance()->getString("WifiSSID"), Font::get(FONT_SIZE_MEDIUM), 0x777777FF);
	editSSID->setHorizontalAlignment(ALIGN_RIGHT);
	auto updateSSID = [this,editSSID](const std::string& newVal) {
		editSSID->setText(newVal);
		Settings::getInstance()->setString("WifiSSID", newVal);
		writeNetworkSettings();
		Settings::getInstance()->saveFile();
	};
	auto spacer = std::make_shared<GuiComponent>(mWindow);
	spacer->setSize(Renderer::getScreenWidth() * 0.005f, 0);

	row.addElement(title, true);
	row.addElement(editSSID, true);
	row.addElement(spacer, false);
	row.addElement(makeArrow(mWindow), false);
	row.makeAcceptInputHandler( [this, editSSID, updateSSID] {
		mWindow->pushGui(new GuiTextEditPopupKeyboard(mWindow, "SSID", editSSID->getValue(), updateSSID, false));
	});
	mMenu.addRow(row);
	row.elements.clear();
	
	//PASSWORD
	title = std::make_shared<TextComponent>(mWindow, "PASSWORD", Font::get(FONT_SIZE_MEDIUM), 0x777777FF);
	std::string wifiKey = Settings::getInstance()->getString("WifiKey");
	for(int i = 0; i < wifiKey.length(); ++i) wifiKey[i] = '*';
	auto editPass = std::make_shared<TextComponent>(mWindow, wifiKey, Font::get(FONT_SIZE_MEDIUM), 0x777777FF);
	editPass->setHorizontalAlignment(ALIGN_RIGHT);
	auto updatePass = [this,editPass](const std::string& newVal) {
		Settings::getInstance()->setString("WifiKey", newVal);
		std::string wifiKey = newVal;
		for(int i = 0; i < wifiKey.length(); ++i) wifiKey[i] = '*';
		editPass->setText(wifiKey);
		writeNetworkSettings();
		Settings::getInstance()->saveFile();
	};
	
	row.addElement(title, true);
	row.addElement(editPass, true);
	row.addElement(spacer, false);
	row.addElement(makeArrow(mWindow), false);
	row.makeAcceptInputHandler( [this, editPass, updatePass] {
		mWindow->pushGui(new GuiTextEditPopupKeyboard(mWindow, "PASSWORD", "", updatePass, false));
	});
	mMenu.addRow(row);
	row.elements.clear();
	
	// BUTTONS
	mMenu.addButton("BACK", "go back", [this] { delete this; });
	mMenu.addButton("CONNECT", "connect to wifi", [this] { connect(); });

	addChild(&mMenu);
	setSize(mMenu.getSize());
	setPosition((Renderer::getScreenWidth() - mSize.x()) / 2, Renderer::getScreenHeight() * 0.15f);
}

void GuiNetwork::connect() {
	writeNetworkSettings();
	Window* window = mWindow;
	window->pushGui(new GuiWifiConnect(window,[this] {
		updateStat->setText(Utils::Network::getStatus());
		updateIP->setText(Utils::Network::getIP());
	}));
}

bool GuiNetwork::input(InputConfig* config, Input input)
{
	if(config->isMappedTo("b", input) && input.value != 0)
	{
		Settings::getInstance()->saveFile();
		delete this;
		return true;
	}

	if(config->isMappedTo("start", input) && input.value != 0)
	{
		Settings::getInstance()->saveFile();
		// close everything
		Window* window = mWindow;
		while(window->peekGui() && window->peekGui() != ViewController::get())
			delete window->peekGui();
		return true;
	}
	return GuiComponent::input(config, input);
}

void GuiNetwork::writeNetworkSettings() {
	std::ofstream wpaconf;
	wpaconf.open ("/etc/wpa_supplicant/wpa_supplicant.conf");
	wpaconf << "ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev" << "\n";
	wpaconf << "country=" << Settings::getInstance()->getString("WifiCountry") << "\n";
	wpaconf << "network={" << "\n";
	wpaconf << "ssid=\"" << Settings::getInstance()->getString("WifiSSID") << "\"" << "\n";
	wpaconf << "psk=\"" << Settings::getInstance()->getString("WifiKey") << "\"" << "\n";
	wpaconf << "}" << "\n";
	wpaconf.close();
}