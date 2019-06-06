#include "guis/GuiNetwork.h"

#include "components/OptionListComponent.h"
#include "components/SliderComponent.h"
#include "components/SwitchComponent.h"

#include "components/OptionListComponent.h"
#include "components/SliderComponent.h"
#include "components/SwitchComponent.h"
#include "guis/GuiMsgBox.h"
#include "guis/GuiKeyboard.h"
#include "guis/GuiTextEditPopupKeyboard.h"
#include "guis/GuiSettings.h"
#include "views/UIModeController.h"
#include "views/ViewController.h"
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

GuiNetwork::GuiNetwork(Window* window) : GuiComponent(window), mMenu(window, "NETWORK SETTINGS"), mTimer(0), mState(0)
{

	// STATUS
	std::string wStatText = getNetStatus();
	updateStat = std::make_shared<DynamicTextComponent>(mWindow, "" + wStatText, Font::get(FONT_SIZE_MEDIUM), 0x777777FF);
	updateStat->setHorizontalAlignment(ALIGN_RIGHT);
	mMenu.addWithLabel("STATUS", updateStat);

	// IP
	std::string wIP = getIP();
	updateIP = std::make_shared<DynamicTextComponent>(mWindow, "" + wIP, Font::get(FONT_SIZE_MEDIUM), 0x777777FF);
	updateIP->setHorizontalAlignment(ALIGN_RIGHT);
	mMenu.addWithLabel("IP ADDRESS", updateIP);
	
	// WIFI ON OFF
	bool flagWifi = getWifiBool();
	wifi_enabled = std::make_shared<SwitchComponent>(mWindow);
	wifi_enabled->setState(flagWifi);
	mMenu.addWithLabel("ENABLE WIFI", wifi_enabled);
	addSaveFunc([this] {
		if (wifi_enabled->getState()){
			// enable wifi
			system("sudo ifconfig wlan0 up");
			system("sudo sed -i '/dtoverlay=pi3-disable-wifi/s/^#*/#/g' /boot/config.txt");
		} else{
			// disable wifi
			system("sudo ifconfig wlan0 down");
			system("sudo sed -i '/dtoverlay=pi3-disable-wifi/s/^#*//g' /boot/config.txt");
		}
		Settings::getInstance()->setBool("EnableWifi", wifi_enabled->getState());
	});
	
	//SSID
	ComponentListRow row;
	auto title = std::make_shared<TextComponent>(mWindow, "SSID", Font::get(FONT_SIZE_MEDIUM), 0x777777FF);
	auto editSSID = std::make_shared<TextComponent>(mWindow, Settings::getInstance()->getString("WifiSSID"), Font::get(FONT_SIZE_MEDIUM), 0x777777FF);
	editSSID->setHorizontalAlignment(ALIGN_RIGHT);
	auto updateSSID = [this,editSSID](const std::string& newVal) {
		std::stringstream callSupplicant;
		callSupplicant << "sudo sed -i 's/ssid=.*/ssid=\"" << newVal << "\"/' /etc/wpa_supplicant/wpa_supplicant.conf";
		editSSID->setValue(newVal);
		Settings::getInstance()->setString("WifiSSID", newVal);
		if (wifi_enabled->getState()){
			system("sudo ifconfig wlan0 down");
			system("sudo ifconfig wlan0 up");
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
	mMenu.addRow(row);
	
	//PASSWORD
	row.elements.clear();
	std::string wifiKey = Settings::getInstance()->getString("WifiKey");
	for(int i = 0; i < wifiKey.length(); ++i) wifiKey[i] = '*';
	title = std::make_shared<TextComponent>(mWindow, "PASSWORD", Font::get(FONT_SIZE_MEDIUM), 0x777777FF);
	auto editPass = std::make_shared<TextComponent>(mWindow, wifiKey, Font::get(FONT_SIZE_MEDIUM), 0x777777FF);
	editPass->setHorizontalAlignment(ALIGN_RIGHT);
	auto updatePass = [this,editPass](const std::string& newVal) {
		std::stringstream callSupplicant;
		callSupplicant << "sudo sed -i 's/psk=.*/psk=\"" << newVal << "\"/' /etc/wpa_supplicant/wpa_supplicant.conf";
		system(callSupplicant.str().c_str());
		Settings::getInstance()->setString("WifiKey", newVal);
		std::string wifiKey = newVal;
		for(int i = 0; i < wifiKey.length(); ++i) wifiKey[i] = '*';
		editPass->setValue(wifiKey);
		if (wifi_enabled->getState()){
			system("sudo ifconfig wlan0 down");
			system("sudo ifconfig wlan0 up");
		}
	};
	
	row.addElement(title, true);
	row.addElement(editPass, true);
	row.addElement(spacer, false);
	row.addElement(makeArrow(mWindow), false);

	row.makeAcceptInputHandler( [this, editPass, updatePass] {
		mWindow->pushGui(new GuiTextEditPopupKeyboard(mWindow, "PASSWORD", "", updatePass, false));
	});
	mMenu.addRow(row);
	
	// BUTTONS
	
	mMenu.addButton("BACK", "go back", [this] { save(); delete this; });
	mMenu.addButton("CONNECT", "connect to wifi", [this] { connect(); });

	addChild(&mMenu);
	setSize(mMenu.getSize());
	setPosition((Renderer::getScreenWidth() - mSize.x()) / 2, Renderer::getScreenHeight() * 0.15f);
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

bool GuiNetwork::input(InputConfig* config, Input input)
{
	if(config->isMappedTo("b", input) && input.value != 0)
	{
		save();
		delete this;
		return true;
	}

	if(config->isMappedTo("start", input) && input.value != 0)
	{
		// close everything
		save();
		Window* window = mWindow;
		while(window->peekGui() && window->peekGui() != ViewController::get())
			delete window->peekGui();
		return true;
	}
	
	return GuiComponent::input(config, input);
}

std::vector<HelpPrompt> GuiNetwork::getHelpPrompts()
{
	std::vector<HelpPrompt> prompts = mMenu.getHelpPrompts();

	prompts.push_back(HelpPrompt("b", "back"));
	prompts.push_back(HelpPrompt("start", "close"));

	return prompts;
}

void GuiNetwork::save() {
	mState = 0;
	if (mSaveFuncs.empty()) {
		return;
	}
	for (auto it = mSaveFuncs.begin(); it != mSaveFuncs.end(); it++) {
		(*it)();
	}
	Settings::getInstance()->saveFile();
}

void GuiNetwork::connect() {
	wifi_enabled->setState(true);
	save();
	mState = 1;
	mTrys = 0;
	updateStat->setValue("TRYING TO CONNECT");
}

void GuiNetwork::update(int deltaTime) {
	mTimer += deltaTime;
	if (mTimer > 2000 && mState == 1){
		updateStat->setValue(getNetStatus());
		updateIP->setValue(getIP());
		if(updateStat->getValue() == "NOT CONNECTED"){
			updateStat->setValue("TRYING TO CONNECT");
			mTimer = 0;
			if(mTrys > 3){
				mState = 0;
				window->pushGui(new GuiMsgBox(window, "WIFI CONNECTION FAILED"));
			}else{
				mTrys += 1;
			}
		}else{
			mState = 0;
			window->pushGui(new GuiMsgBox(window, "WIFI CONNECTED"));
		}
		
	}
	GuiComponent::update(deltaTime);
}