#include "guis/GuiGeneralScreensaverOptions.h"

#include "components/OptionListComponent.h"
#include "components/SliderComponent.h"
#include "components/SwitchComponent.h"
#include "guis/GuiMsgBox.h"
#include "guis/GuiSlideshowScreensaverOptions.h"
#include "guis/GuiVideoScreensaverOptions.h"
#include "Settings.h"

GuiGeneralScreensaverOptions::GuiGeneralScreensaverOptions(Window* window, const char* title) : GuiScreensaverOptions(window, title)
{
	// screensaver behavior
	auto screensaver_behavior = std::make_shared< OptionListComponent<std::string> >(mWindow, "SCREENSAVER STYLE", false);
	std::vector<std::string> screensavers;
	screensavers.push_back("dim");
	screensavers.push_back("game art");
	screensavers.push_back("slideshow");
	for(auto it = screensavers.cbegin(); it != screensavers.cend(); it++)
		screensaver_behavior->add(*it, *it, Settings::getInstance()->getString("ScreenSaverBehavior") == *it);
	addWithLabel("SCREENSAVER STYLE", screensaver_behavior);
	addSaveFunc([this, screensaver_behavior] {
		Settings::getInstance()->setString("ScreenSaverBehavior", screensaver_behavior->getSelected());
		PowerSaver::updateTimeouts();
	});
	
	
	// screensaver time
	auto screensaver_time = std::make_shared<SliderComponent>(mWindow, 0.f, 30.f, 1.f, "m");
	screensaver_time->setValue((float)(Settings::getInstance()->getInt("ScreenSaverTime") / (1000 * 60)));
	addWithLabel("SCREENSAVER AFTER", screensaver_time);
	addSaveFunc([screensaver_time] {
	    Settings::getInstance()->setInt("ScreenSaverTime", (int)Math::round(screensaver_time->getValue()) * (1000 * 60));
	    PowerSaver::updateTimeouts();
	});
	
	// Allow ScreenSaver Controls - ScreenSaverControls
	/*
	auto ss_controls = std::make_shared<SwitchComponent>(mWindow);
	ss_controls->setState(Settings::getInstance()->getBool("ScreenSaverControls"));
	addWithLabel("SCREENSAVER CONTROLS", ss_controls);
	addSaveFunc([ss_controls] { Settings::getInstance()->setBool("ScreenSaverControls", ss_controls->getState()); });
	*/

	ComponentListRow row;
	
	// image duration (seconds)
	row.elements.clear();
	row.addElement(std::make_shared<TextComponent>(mWindow, "SWAP IMAGE AFTER (SECS)", Font::get(FONT_SIZE_MEDIUM), 0x777777FF), true);
	auto sss_image_sec = std::make_shared<SliderComponent>(mWindow, 1.f, 60.f, 1.f, "s");
	sss_image_sec->setValue((float)(Settings::getInstance()->getInt("ScreenSaverSwapImageTimeout") / (1000)));
	row.addElement(sss_image_sec, false, true);
	addSaveFunc([sss_image_sec] {
		int playNextTimeout = (int)Math::round(sss_image_sec->getValue()) * (1000);
		Settings::getInstance()->setInt("ScreenSaverSwapImageTimeout", playNextTimeout);
		PowerSaver::updateTimeouts();
	});
	addRow(row);
	
	// image source
	/*
	row.elements.clear();
	row.addElement(std::make_shared<TextComponent>(mWindow, "USE CUSTOM IMAGES", Font::get(FONT_SIZE_MEDIUM), 0x777777FF), true);
	auto sss_custom_source = std::make_shared<SwitchComponent>(mWindow);
	sss_custom_source->setState(Settings::getInstance()->getBool("SlideshowScreenSaverCustomImageSource"));
	row.addElement(sss_custom_source, false, true);
	addSaveFunc([sss_custom_source] { Settings::getInstance()->setBool("SlideshowScreenSaverCustomImageSource", sss_custom_source->getState()); });
	addRow(row);
	*/

	// show filtered menu
	/*
	row.elements.clear();
	row.addElement(std::make_shared<TextComponent>(mWindow, "VIDEO SCREENSAVER SETTINGS", Font::get(FONT_SIZE_MEDIUM), 0x777777FF), true);
	row.addElement(makeArrow(mWindow), false);
	row.makeAcceptInputHandler(std::bind(&GuiGeneralScreensaverOptions::openVideoScreensaverOptions, this));
	addRow(row);
	*/
	/*
	row.elements.clear();
	row.addElement(std::make_shared<TextComponent>(mWindow, "SLIDESHOW SCREENSAVER SETTINGS", Font::get(FONT_SIZE_MEDIUM), 0x777777FF), true);
	row.addElement(makeArrow(mWindow), false);
	row.makeAcceptInputHandler(std::bind(&GuiGeneralScreensaverOptions::openSlideshowScreensaverOptions, this));
	addRow(row);
	*/
}

GuiGeneralScreensaverOptions::~GuiGeneralScreensaverOptions()
{
}

void GuiGeneralScreensaverOptions::openVideoScreensaverOptions() {
	mWindow->pushGui(new GuiVideoScreensaverOptions(mWindow, "VIDEO SCREENSAVER"));
}

void GuiGeneralScreensaverOptions::openSlideshowScreensaverOptions() {
    mWindow->pushGui(new GuiSlideshowScreensaverOptions(mWindow, "SLIDESHOW SCREENSAVER"));
}

