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
	
	// WIFI ON OFF
	bool flagWifi = Utils::Network::isWifi();
	wifi_enabled = std::make_shared<SwitchComponent>(mWindow);
	wifi_enabled->setState(flagWifi);
	mMenu.addWithLabel("ENABLE WIFI", wifi_enabled);
	
	//SSID
	title = std::make_shared<TextComponent>(mWindow, "SSID", Font::get(FONT_SIZE_MEDIUM), 0x777777FF);
	auto editSSID = std::make_shared<TextComponent>(mWindow, Settings::getInstance()->getString("WifiSSID"), Font::get(FONT_SIZE_MEDIUM), 0x777777FF);
	editSSID->setHorizontalAlignment(ALIGN_RIGHT);
	auto updateSSID = [this,editSSID](const std::string& newVal) {
		editSSID->setText(newVal);
		Settings::getInstance()->setString("WifiSSID", newVal);
		writeNetworkSettings();
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
	mMenu.addButton("BACK", "go back", [this] { save(); delete this; });
	mMenu.addButton("CONNECT", "connect to wifi", [this] { connect(); });

	addChild(&mMenu);
	setSize(mMenu.getSize());
	setPosition((Renderer::getScreenWidth() - mSize.x()) / 2, Renderer::getScreenHeight() * 0.15f);
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
	if (wifi_enabled->getState()){
		// enable wifi
		system("sudo ifconfig wlan0 up");
		//system("sudo sed -i '/dtoverlay=pi3-disable-wifi/s/^#*/#/g' /boot/config.txt");
	} else{
		// disable wifi
		system("sudo ifconfig wlan0 down");
		//system("sudo sed -i '/dtoverlay=pi3-disable-wifi/s/^#*//g' /boot/config.txt");
	}
	Settings::getInstance()->setBool("EnableWifi", wifi_enabled->getState());
	Settings::getInstance()->saveFile();
}

void GuiNetwork::connect() {
	save();
	Window* window = mWindow;
	window->pushGui(new GuiWifiConnect(window));
}

void GuiNetwork::writeNetworkSettings() {
	std::ofstream wpaconf;
	wpaconf.open ("/etc/wpa_supplicant/wpa_supplicant.conf");
	wpaconf << "ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev" << "\n";
	wpaconf << "country=AT" << "\n";
	wpaconf << "network={" << "\n";
	wpaconf << "ssid=\"" << Settings::getInstance()->getString("WifiSSID") << "\"" << "\n";
	wpaconf << "psk=\"" << Settings::getInstance()->getString("WifiKey") << "\"" << "\n";
	wpaconf << "}" << "\n";
	wpaconf.close();
}

void GuiNetwork::update(int deltaTime) {
	/*
	mTimer += deltaTime;
	if (mTimer > 1000 && mState == 1){
		mTrys += 1;
		std::string trying = "TRYING TO CONNECT ";
		std::string status = getNetStatus();
		std::string ip = getIP();
		if(status == "NOT CONNECTED"){
			if(mTrys > 15){
				mState = 0;
				Window* window = mWindow;
				window->pushGui(new GuiMsgBox(window, "WIFI CONNECTION FAILED"));
			}else{
				std::stringstream sTrys;
				sTrys << trying << mTrys;
				updateStat->setText(sTrys.str());
				mTimer = 0;
			}
		}else{
			mState = 0;
			mTrys = 0;
			updateStat->setText(status);
			updateIP->setText(ip);
			Window* window = mWindow;
			window->pushGui(new GuiMsgBox(window, "WIFI CONNECTED"));
		}
		
	}
	*/
	GuiComponent::update(deltaTime);
	
}