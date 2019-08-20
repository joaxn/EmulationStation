#include "guis/GuiStorage.h"

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

#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <stdlib.h>

GuiStorage::GuiStorage(Window* window) : GuiComponent(window), mMenu(window, "STORAGE")
{
	ComponentListRow row;
	
	/*----------------------------------------------*/
	//CALCULATION
	/*----------------------------------------------*/
	FILE *fp;
	char path[1035];
	fp = popen("df /stat/sda1 -B M --output=size,avail,pcent", "r");

	std::string cLine;
	while (fgets(path, sizeof(path), fp) != NULL) {
		// Grab the last line.
		cLine = path;
	}

	// format the info out and put into ints.
	std::string f1, f2, f3;
	int f = cLine.find("M");
	f1 = cLine.substr(0, f);
	int q = cLine.substr(f + 1).find("M");
	f2 = cLine.substr(f + 1, q);
	int p = cLine.find("%");
	f3 = cLine.substr(p - 3, p);

	// convert to ints
	float iSize = stof(f1);
	float iAvailable = stof(f2);
	int iPerc = stoi(f3);

	// get size in GB then put it back to string
	std::string totalSizeInGb;
	std::string totalAvailableInGb;
	std::stringstream ss, s2;
	ss << std::setprecision(4) << float(iSize / 1000);
	s2 << std::setprecision(4) << float(iAvailable / 1000);
	totalSizeInGb = ss.str();
	totalAvailableInGb = s2.str();

	// get used
	int iUsed = iSize - iAvailable;

	// Get percents for progressbars
	//float pUsed = (float(iSize / iUsed) * 10);		// Old garbage way.
	std::stringstream s3;
	s3 << iPerc;
	
	/*----------------------------------------------*/
	//BAR
	/*----------------------------------------------*/
	auto pbar_total = std::make_shared<SliderComponent>(mWindow, 0, 100.f, 0, s3.str() + "%");
	pbar_total->setValue((float)iPerc);
	row.addElement(pbar_total, true);
	mMenu.addRow(row);
	row.elements.clear();
	
	/*----------------------------------------------*/
	//SIZE
	/*----------------------------------------------*/
	auto tell_totalsize = std::make_shared<TextComponent>(mWindow, "TOTAL DISK SIZE", Font::get(FONT_SIZE_MEDIUM), 0x777777FF);
	auto tell_totalsize_i = std::make_shared<TextComponent>(mWindow, "" + totalSizeInGb + " GB", Font::get(FONT_SIZE_MEDIUM, FONT_PATH_LIGHT), 0x777777FF);
	tell_totalsize_i->setHorizontalAlignment(ALIGN_RIGHT);
	row.addElement(tell_totalsize, true);
	row.addElement(tell_totalsize_i, true);
	mMenu.addRow(row);
	row.elements.clear();

	/*----------------------------------------------*/
	//FREE
	/*----------------------------------------------*/
	auto tell_free = std::make_shared<TextComponent>(mWindow, "TOTAL DISK FREE", Font::get(FONT_SIZE_MEDIUM), 0x777777FF);
	auto tell_free_i = std::make_shared<TextComponent>(mWindow, "" + totalAvailableInGb + " GB", Font::get(FONT_SIZE_MEDIUM, FONT_PATH_LIGHT), 0x777777FF);
	tell_totalsize_i->setHorizontalAlignment(ALIGN_RIGHT);
	row.addElement(tell_free, true);
	row.addElement(tell_free_i, true);
	mMenu.addRow(row);
	row.elements.clear();
	
	
	/*----------------------------------------------*/
	//PASSWORD
	/*----------------------------------------------*/
	auto title = std::make_shared<TextComponent>(mWindow, "PASSWORD", Font::get(FONT_SIZE_MEDIUM), 0x777777FF);
	auto spacer = std::make_shared<GuiComponent>(mWindow);
	spacer->setSize(Renderer::getScreenWidth() * 0.005f, 0);
	std::string smbKey = Settings::getInstance()->getString("SmbKey");
	for(int i = 0; i < smbKey.length(); ++i) smbKey[i] = '*';
	auto editPass = std::make_shared<TextComponent>(mWindow, smbKey, Font::get(FONT_SIZE_MEDIUM, FONT_PATH_LIGHT), 0x777777FF);
	editPass->setHorizontalAlignment(ALIGN_RIGHT);
	auto updatePass = [this,editPass](const std::string& newVal) {
		Settings::getInstance()->setString("SmbKey", newVal);
		std::string smbKey = newVal;
		for(int i = 0; i < smbKey.length(); ++i) smbKey[i] = '*';
		editPass->setText(smbKey);
		writeStorageSettings();
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
	
	/*----------------------------------------------*/
	//BUTTONS
	/*----------------------------------------------*/
	mMenu.addButton("BACK", "go back", [this] { delete this; });

	addChild(&mMenu);
	setSize(mMenu.getSize());
	setPosition((Renderer::getScreenWidth() - mSize.x()) / 2, Renderer::getScreenHeight() * 0.15f);
}


void GuiStorage::writeStorageSettings() {
	std::stringstream cmd;
	std::string smbKey = Settings::getInstance()->getString("SmbKey");
	cmd << "(echo \"" << smbKey << "\"; echo \"" << smbKey << "\") | smbpasswd -s worukami";
	system(cmd.str().c_str());
	system("sudo service smbd restart");
}