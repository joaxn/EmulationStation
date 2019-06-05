#include "guis/GuiNetwork.h"

#include "components/OptionListComponent.h"
#include "components/SliderComponent.h"
#include "components/SwitchComponent.h"
#include "guis/GuiMsgBox.h"

#include "components/OptionListComponent.h"
#include "components/SliderComponent.h"
#include "components/SwitchComponent.h"
#include "guis/GuiMsgBox.h"
#include "guis/GuiKeyboard.h"
#include "guis/GuiTextEditPopupKeyboard.h"
#include "views/UIModeController.h"
#include "views/ViewController.h"
#include "CollectionSystemManager.h"
#include "EmulationStation.h"
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

GuiNetwork::GuiNetwork(Window* window) : GuiComponent(window), mMenu(window, "NETWORK SETTINGS"), mTimer(0)
{

	openNetworkSettings();

	addChild(&mMenu);
	setSize(mMenu.getSize());
	setPosition((Renderer::getScreenWidth() - mSize.x()) / 2, Renderer::getScreenHeight() * 0.15f);
}

void GuiNetwork::openNetworkSettings()
{
	auto s = new GuiSettings(mWindow, "NETWORK SETTINGS");

	// STATUS
	std::string wStatText = getNetStatus();
	auto show_stat = std::make_shared<TextComponent>(mWindow, "" + wStatText, Font::get(FONT_SIZE_MEDIUM), 0x777777FF);
	s->addWithLabel("STATUS", show_stat);

	// IP
	std::string wIP = getIP();
	updateIP = std::make_shared<TextComponent>(mWindow, "" + wIP, Font::get(FONT_SIZE_MEDIUM), 0x777777FF);
	s->addWithLabel("IP ADDRESS", updateIP);
	
	// WIFI ON OFF
	bool flagWifi = getWifiBool();
	auto wifi_enabled = std::make_shared<SwitchComponent>(mWindow);
	wifi_enabled->setState(flagWifi);
	s->addWithLabel("ENABLE WIFI", wifi_enabled);
	s->addSaveFunc([this,wifi_enabled] {
		if (wifi_enabled->getState()){
			updateIP->setValue("TRYING TO CONNECT");
			// enable wifi
			//system("sudo ifconfig wlan0 up");
			//system("sudo sed -i '/dtoverlay=pi3-disable-wifi/s/^#*/#/g' /boot/config.txt");
		} else{
			updateIP->setValue("NOT CONNECTED");
			// disable wifi
			//system("sudo ifconfig wlan0 down");
			//system("sudo sed -i '/dtoverlay=pi3-disable-wifi/s/^#*//g' /boot/config.txt");
		}
		Settings::getInstance()->setBool("EnableWifi", wifi_enabled->getState());
		Settings::getInstance()->saveFile();
	});
	
	//SSID
	ComponentListRow row;
	auto title = std::make_shared<TextComponent>(mWindow, "SSID", Font::get(FONT_SIZE_MEDIUM), 0x777777FF);
	auto editSSID = std::make_shared<TextComponent>(mWindow, Settings::getInstance()->getString("WifiSSID"), Font::get(FONT_SIZE_MEDIUM), 0x777777FF);
	editSSID->setHorizontalAlignment(ALIGN_RIGHT);
	auto updateSSID = [this,editSSID,wifi_enabled](const std::string& newVal) {
		std::stringstream callSupplicant;
		callSupplicant << "sudo sed -i 's/ssid=.*/ssid=\"" << newVal << "\"/' /etc/wpa_supplicant/wpa_supplicant.conf";
		editSSID->setValue(newVal);
		Settings::getInstance()->setString("WifiSSID", newVal);
		Settings::getInstance()->saveFile();
		if (wifi_enabled->getState()){
			system("sudo ifconfig wlan0 down");
			system("sudo ifconfig wlan0 up");
			updateIP->setValue("TRYING TO CONNECT");
		}
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
	s->addRow(row);
	
	//PASSWORD
	row.elements.clear();
	std::string wifiKey = Settings::getInstance()->getString("WifiKey");
	for(int i = 0; i < wifiKey.length(); ++i) wifiKey[i] = '*';
	title = std::make_shared<TextComponent>(mWindow, "PASSWORD", Font::get(FONT_SIZE_MEDIUM), 0x777777FF);
	auto editPass = std::make_shared<TextComponent>(mWindow, wifiKey, Font::get(FONT_SIZE_MEDIUM), 0x777777FF);
	editPass->setHorizontalAlignment(ALIGN_RIGHT);
	auto updatePass = [this,editPass,wifi_enabled](const std::string& newVal) {
		std::stringstream callSupplicant;
		callSupplicant << "sudo sed -i 's/psk=.*/psk=\"" << newVal << "\"/' /etc/wpa_supplicant/wpa_supplicant.conf";
		system(callSupplicant.str().c_str());
		Settings::getInstance()->setString("WifiKey", newVal);
		Settings::getInstance()->saveFile();
		std::string wifiKey = newVal;
		for(int i = 0; i < wifiKey.length(); ++i) wifiKey[i] = '*';
		editPass->setValue(wifiKey);
		if (wifi_enabled->getState()){
			system("sudo ifconfig wlan0 down");
			system("sudo ifconfig wlan0 up");
			updateIP->setValue("TRYING TO CONNECT");
		}
	};
	
	row.addElement(title, true);
	row.addElement(editPass, true);
	row.addElement(spacer, false);
	row.addElement(makeArrow(mWindow), false);

	row.makeAcceptInputHandler( [this, editPass, updatePass] {
		mWindow->pushGui(new GuiTextEditPopupKeyboard(mWindow, "PASSWORD", "", updatePass, false));
	});
	s->addRow(row);
	
	mWindow->pushGui(s);
}

std::string GuiNetwork::getIP()
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

std::string GuiNetwork::getNetStatus()
{
	FILE *wIPP;
	char wip[1035];
	
	std::string wStat;
	std::string wStatText;
	wIPP = popen("hostname -I", "r");
	while (fgets(wip, sizeof(wip), wIPP) != NULL) {
		wStat = wip;
		int trim = wStat.find("\n");
		wStat = wStat.substr(0, trim);
		if(wStat == "" | wStat == " "){
			wStatText = "NOT CONNECTED";
		}else{
			wStatText = "CONNECTED";
		}
	}
	return wStatText;
}

bool GuiNetwork::getWifiBool()
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

void GuiMenu::update(int deltaTime) {
	mTimer += deltaTime;
	if (mTimer > 2000 && mCurrent == "NETWORK"){
		mTimer = 0;
	}
	GuiComponent::update(deltaTime);
}