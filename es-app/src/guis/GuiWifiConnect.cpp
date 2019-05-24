#include "EmulationStation.h"
#include "guis/GuiWifiConnect.h"
#include "guis/GuiKeyboard.h"
#include "Window.h"
#include "Log.h"
#include "Settings.h"
#include "guis/GuiMsgBox.h"
#include "guis/GuiSettings.h"
#include "utils/FileSystemUtil.h"

#include <iostream>
#include <fstream>
#include <string>

#include "components/ButtonComponent.h"
#include "components/SwitchComponent.h"
#include "components/SliderComponent.h"
#include "components/TextComponent.h"
#include "components/OptionListComponent.h"
#include "components/MenuComponent.h"
#include "guis/GuiTextEditPopupKeyboard.h"

GuiWifiConnect::GuiWifiConnect(Window* window, std::string wifiName, bool encrypted) : GuiComponent(window), mMenu(window, "Connect to wifi"), mVersion(window)
{
	// CONNECT TO WIFI

	// ENTER PASSWORD >
	// GET FULL DETAILS >
	// CONNECT >

	// --------------

	std::string windowName = "Connect to > " + wifiName;
	mMenu.setTitle(windowName.c_str(), Font::get(FONT_SIZE_LARGE));

	auto ed = std::make_shared<TextComponent>(mWindow, "", Font::get(FONT_SIZE_SMALL, FONT_PATH_LIGHT), 0x777777FF, ALIGN_RIGHT);


	/// ======  ENTER PASSWORD  ======
	// if network is ecrypted:
	if (encrypted) {
		addEntry("ENTER PASSWORD", 0x777777FF, true, [this, wifiName, ed] {
			// create callback function to store password from keyboard popup
			auto updateVal = [ed](const std::string& newVal) { ed->setValue(newVal); }; // ok callback (apply new value to ed)

			// popup the keyboard.
			mWindow->pushGui(new GuiTextEditPopupKeyboard(mWindow, wifiName + " PASSWORD", ed->getValue(), updateVal, false));
		});
	}
	else {
		// When network is NOT encrpyted and OPEN:
		addEntry("ADD PASSWORD [NETOWRK IS OPEN]", 0x777777FF, true, [this, wifiName, ed] {
			// create callback function to store password from keyboard popup
			auto updateVal = [ed](const std::string& newVal) { ed->setValue(newVal); }; // ok callback (apply new value to ed)

			// popup the keyboard.
			mWindow->pushGui(new GuiTextEditPopupKeyboard(mWindow, wifiName + " PASSWORD", ed->getValue(), updateVal, false));
		});
	}
	

	addChild(&mMenu);
	mMenu.addButton("BACK", "go back", [this] { delete this; });
	mMenu.addButton("CONNECT", "connect", [this, ed, wifiName, encrypted] {
		if (ed->getValue().length() < 8 && encrypted) {
			mWindow->pushGui(new GuiMsgBox(mWindow, "Password is not long enough!  Must be at least 8 characters long.", "Ok", nullptr));
			return;
		}
		mWindow->pushGui(new GuiMsgBox(mWindow, "Network: " + wifiName + "\n Password: " + ed->getValue(), "Connect", [this, wifiName, ed] {
			// Quick and dirty just send the info to wificonnect
			std::string cmdStr = "sudo " + Utils::FileSystem::getHomePath() + "/.emulationstation/app/wifi/./wificonnect --ssid '" + wifiName + "' --password " + ed->getValue();
			const char* cmd = cmdStr.c_str();

			// Make sure wificonnect exists
			std::string path = Utils::FileSystem::getHomePath() + "/.emulationstation/app/wifi/wificonnect";
			if (Utils::FileSystem::exists(path)) {
				system(cmd);
				mConnected = true;
			} else {
				mWindow->pushGui(new GuiMsgBox(mWindow, "wificonnect is missing.  This is used to send wifi info to wpa_supplicant.  This should be in ~/.emulationstation/app/wifi", "OK", nullptr));
				LOG(LogError) << "WifiConnect: Couldn't find wificonnect in " << path << " folder";
			}

			//std::ofstream oFile;
			//oFile.open("~/.emulationstation/networks.lst", std::ofstream::out | std::ofstream::app);
			//oFile << wifiName;
			//oFile.close();

			// If wpa_supplicant.conf couldn't be opened.
			//mWindow->pushGui(new GuiMsgBox(mWindow, "Could not open /etc/wpa_supplicant/wpa_supplicant.conf file.  This could be permission issues.", "Ok", nullptr, "Cancel", nullptr));

		},

			"Cancel", nullptr));
	});

	setSize(mMenu.getSize());
	setPosition((Renderer::getScreenWidth() - mSize.x()) / 2, Renderer::getScreenHeight() * 0.15f);
}

void GuiWifiConnect::onSizeChanged()
{
	
}

void GuiWifiConnect::addEntry(const char* name, unsigned int color, bool add_arrow, const std::function<void()>& func)
{
	std::shared_ptr<Font> font = Font::get(FONT_SIZE_MEDIUM);
	
	// populate the list
	ComponentListRow row;
	row.addElement(std::make_shared<TextComponent>(mWindow, name, font, color), true);

	if(add_arrow)
	{
		std::shared_ptr<ImageComponent> bracket = makeArrow(mWindow);
		row.addElement(bracket, false);
	}
	
	row.makeAcceptInputHandler(func);

	mMenu.addRow(row);
}

bool GuiWifiConnect::input(InputConfig* config, Input input)
{
	if(GuiComponent::input(config, input))
		return true;

	if((config->isMappedTo("b", input) || config->isMappedTo("start", input)) && input.value != 0)
	{
		delete this;
		return true;
	}

	if (mConnected) {
		delete this;
		return true;
	}

	return false;
}

std::vector<HelpPrompt> GuiWifiConnect::getHelpPrompts()
{
	std::vector<HelpPrompt> prompts;
	prompts.push_back(HelpPrompt("up/down", "choose"));
	prompts.push_back(HelpPrompt("a", "select"));
	prompts.push_back(HelpPrompt("start", "close"));
	return prompts;
}
