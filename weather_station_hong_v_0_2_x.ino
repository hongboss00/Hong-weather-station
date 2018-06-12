#include <Wire.h>
#include "ssd1306_i2c.h"
#include "images.h"
#include "WundergroundClient.h"
#include "TimeClient.h"

#include <ESP8266WiFi.h>
#include <JsonListener.h>

#define I2C 0x3c
#define BTN_1 D5
#define BTN_2 D1

//wunderground
const String WUNDERGROUND_API_KEY = "df1b9420db9c8d85";
const String  WUNDERGROUND_COUNTRY = "KO";
const String  WUNDERGROUND_CITY = "RKTF";
const String  WUNDERGRROUND_LANGUAGE = "EN";
const boolean IS_METRIC = true;
const boolean USE_PM = true;

//
const float UTC = 9;

// Initialize the oled display for address 0x3c
// 0x3D is the adafruit address....
SSD1306 display(I2C, D6, D7);
WundergroundClient wunderground(IS_METRIC);
TimeClient timeclient(UTC);
//Frames
void drawFrame1();
void drawFrame2();
void drawFrame3();
void drawFrame4();
void drawFrame5();

// this array keeps function pointers to all frames
// frames are the single views that slide from right to left
//void (*frameCallbacks[3])(int x, int y) = { drawFrame1, drawFrame4, drawFrame5};

// on frame is currently displayed

// your network SSID (name)
char pass[] = "";

// stop watch
int counter = 0;
int sec_F = 0;
int min_F = 0;
int hour_F = 0;
int stopcount = 0;


// flag changed in the ticker function every 10 minutes
bool readyForWeatherUpdate = true;
bool selecting = false;
bool scanning = false;
bool selected = false;
//bool initial = true;

int selecting_number = 0;

int action = 1;
int action_2 = 1;

int buttonstate = 0;
int lastbuttonstate = 1;

int buttonstate_2 = 0;
int lastbuttonstate_2 = 1;
String SSID_list[6];
char SSID0[100];
char SSID1[100];
char SSID2[100];
char SSID3[100];
char SSID4[100];
char SSID5[100];
char* SSID_LIST[6] = {SSID0,SSID1,SSID2,SSID3,SSID4,SSID5 };

//const char* moon_phase = "";

void setup() {
  delay(500);

  Serial.begin(9600);
  pinMode(BTN_1, INPUT_PULLUP);
  pinMode(BTN_2, INPUT_PULLUP);
  // initialize display
  display.init();
  display.clear();
  display.flipScreenVertically();
  // set the drawing functions

  display.clear();
  display.display();
 
  delay(500);

}

void loop() {
		
	if (readyForWeatherUpdate ) {
		if (WiFi.status() != WL_CONNECTED) {
			wificonnect();
		}
		if (WiFi.status() == WL_CONNECTED) {
			updateData();
		}
	}
	buttonaction(BTN_1);
	buttonaction_2(BTN_2);
	display.clear();
	currentFrame();
	Serial.println(SSID_LIST[2]);
}

void updateData() {
	display.clear();
	display.drawString(10,10,"Information...");
	display.display();
	wunderground.updateConditions(WUNDERGROUND_API_KEY, WUNDERGRROUND_LANGUAGE, WUNDERGROUND_COUNTRY, WUNDERGROUND_CITY);
	wunderground.updateForecast(WUNDERGROUND_API_KEY, WUNDERGRROUND_LANGUAGE,WUNDERGROUND_COUNTRY, WUNDERGROUND_CITY);
	//wunderground.updateAstronomy(WUNDERGROUND_API_KEY, WUNDERGRROUND_LANGUAGE, WUNDERGROUND_COUNTRY, WUNDERGROUND_CITY);
	timeclient.updateTime();
	delay(500);
	WiFi.disconnect();
	//String getMoon = wunderground.getMoonPhase();
	//moon_phase = getIconOfMoon(getMoon);

	/*if (getIconOfMoon(getMoon) == "Wan") {
		if (getMoon.substring(7, 10) == "Gib") {
			moon_phase = moon70_bits;
		}
		moon_phase = moon30_2_bits;
	}
	if (getIconOfMoon(getMoon) == "Wax") {
		if (getMoon.substring(7, 10) == "Gib") {
			moon_phase = moon70_2_bits;
		}
		moon_phase = moon30_bits;
	}
	
	*/
	readyForWeatherUpdate = false;
	display.clear();
	display.display();
	delay(500);
}


void setReadyForWeatherUpdate() {
  readyForWeatherUpdate = true;
}

void drawFrame1() {
	display.clear();
	display.setFontScale2x2(false);
	display.drawString(22 , 10 , wunderground.getDate().substring(0, 11));

	int hour_i = atoi(timeclient.getHours().c_str());
	int minute_i = atoi(timeclient.getMinutes().c_str());
	int second_i = atoi(timeclient.getSeconds().c_str());
	String M = "AM";
	String hour = String(hour_i);
	String minute = String(minute_i);
	String second = String(second_i);
	if (hour_i > 12) {
		hour_i -= 12;
		hour = String(hour_i);
		M = "PM";
	}
	if (hour_i < 10) {
		hour = "0" + hour;

	}

	if (minute_i < 10) {
		minute = "0" + minute;
	}
	if (second_i < 10) {
		second = "0" + second;
	}

	display.setFontScale2x2(true);
	display.drawString(18 , 23 , hour + ":" + minute);
	display.setFontScale2x2(false);
	display.drawString(99 , 20 , M);
	display.drawString(99 , 30 , second);
	display.display();
}



void drawFrame2() {
	display.clear();
	display.setFontScale2x2(false);
	display.drawString(65 , 0 , "Today");
	display.drawXbm(0, 0, 60, 60, xbmtemp);
	display.setFontScale2x2(true);
	String temp = wunderground.getCurrentTemp();
	display.drawString(64 , 14 , temp + "C");
	display.drawString(64 , 32 , wunderground.getHumidity());
	display.display();
}

void drawFrame3() {
	display.clear();
	display.setFontScale2x2(false);
	display.drawString(70, 0, "Today");
	display.drawXbm(10, 0, 50, 50, getIconFromString(wunderground.getTodayIcon()));
	display.setFontScale2x2(true);
	display.drawString(69, 14, wunderground.getCurrentTemp() + "C");
	display.setFontScale2x2(false);
	String high = wunderground.getForecastHighTemp(0);
	String low = wunderground.getForecastLowTemp(0);
	display.drawString(71, 38, high + "/" + low);
	display.display();
}

void drawFrame4() {
	display.clear();
	display.setFontScale2x2(false);
	display.drawString(65, 0, "Tomorrow");
	display.drawXbm(10, 0, 50, 50, getIconFromString(wunderground.getForecastIcon(2)));
	display.setFontScale2x2(true);
	String high = wunderground.getForecastHighTemp(2);
	String low = wunderground.getForecastLowTemp(2);
	display.drawString(71, 14, high + "C");
	display.drawString(71, 32, low + "C");
	display.display();
}

void drawFrame5() {
	display.clear();

	String sec_s = String(sec_F);
	String min_s = String(min_F);
	String hour_s = String(hour_F);

	if (stopcount == 1) {
		sec_F = millis() / (1000 - 60 * counter);


		if (sec_F == 60) {
			min_F++;
			counter++;
			sec_F = 0;
		}

		if (min_F == 60) {
			hour_F++;
			min_F = 0;
		}

		if (sec_F < 10) {
			sec_s = "0" + sec_s;
		}

		if (min_F < 10) {
			min_s = "0" + min_s;
		}

		if (hour_F < 10) {
			hour_s = "0" + hour_s;
		}

		display.setFontScale2x2(false);
		display.drawString(22, 15, "STOP_WATCH");
		display.setFontScale2x2(true);
		display.drawString(18, 28, hour_s + ":" + min_s);
		display.setFontScale2x2(false);
		display.drawString(99, 35, sec_s);
	}
	display.display();



}


	/*void drawFrame5() {
	display.setFontScale2x2(false);
	display.drawString(65 , 10, "Moon");
	display.drawXbm(0, 0, 50, 50, moon_phase);
	display.setFontScale2x2(true);
	display.drawString(64 , 24, wunderground.getMoonPctIlum() + "%");
	display.setFontScale2x2(false);
	String rise = wunderground.getMoonriseTime();
	String set = wunderground.getMoonsetTime();
	display.drawString(66 , 38 + y, rise);
	display.drawString(66 , 48 + y, set);

	}
	*/



const char* getIconFromString(String icon) {
  //"clear-day, clear-night, rain, snow, sleet, wind, fog, cloudy, partly-cloudy-day, or partly-cloudy-night"
  if (icon == "B") {
    return clear_day_bits;
  } else if (icon == "N") {
    return clear_night_bits;
  } else if (icon == "R") {
    return rain_bits;
  } else if (icon == "W") {
    return snow_bits;
  } else if (icon == "S") {
    return sleet_bits;
  } else if (icon == "D") {
    return wind_bits;
  } else if (icon == "M") {
    return fog_bits;
  } else if (icon == "Y") {
    return cloudy_bits;
  } else if (icon == "P") {
    return partly_cloudy_day_bits;
  } else if (icon == "PN") {
    return partly_cloudy_night_bits;
  }
  return cloudy_bits;
}

const char* getIconOfMoon(String icon_2) {
	String icon = icon_2.substring(0, 3);
	if (icon == "New") {
		return moon0_bits;
	}else if (icon == "Wax") {
		return "Waxing";
	}else if (icon == "Wan") {
		return "Waning";
	}else if (icon == "Fir") {
		return moon50_bits;
	}
	else if (icon == "Thi") {
		return moon50_2_bits;
	}else if (icon == "Ful") {
		return moon100_bits;
	}
}
/*const char* getIconOfMoon(String icon) {
	if (icon == "New") {
		return moon0_bits;
	}
	else if (icon == "Waxing Crescent") {
		if (atoi(wunderground.getMoonPctIlum().c_str()) >= 30) {
			return moon30_bits;
		}
		return moon10_bits;
	}
	else if (icon == "Waning Crescent") {
		if (atoi(wunderground.getMoonPctIlum().c_str()) >= 30) {
			return moon30_2_bits;
		}
		return moon10_2_bits;
		
	}
	else if (icon == "First Quater") {
		return moon50_bits;
	}
	else if (icon == "Third Quater") {
		return moon50_2_bits;
	}
	else if (icon == "Waning Gibbous") {
		moon70_bits;
	}
	else if (icon == "Waxing Gibbous") {
		return moon70_2_bits;
	}
	else if (icon == "Full") {
		return moon100_bits;
	}

}
*/

void wificonnect() {
	Wifiscan();
	while (!selecting) {
		Wifiselector();
	}
			WiFi.begin(SSID_LIST[selecting_number], pass);

			int counter = 0;
			while (WiFi.status() != WL_CONNECTED) {
				delay(500);

				display.clear();
				display.drawString(20, 10, "connecting...");
				drawSpinner(3, counter % 3);
				display.display();

				counter++;
			}
			delay(500);
}

void drawSpinner(int count, int active) {
  for (int i = 0; i < count; i++) {
    const char *xbm;
    if (active == i) {
      xbm = active_bits;
    } else {
      xbm = inactive_bits;
    }
    display.drawXbm(64 - (12 * count / 2) + 12 * i, 30, 8, 8, xbm);
  }
}

void buttonaction(int pin) {
	buttonstate = digitalRead(pin);
	if (action > 5) {
		action = 1;
	}

	if (buttonstate != lastbuttonstate) {
		if (buttonstate == 0) {

			action++;
			selected = true;
		}
		
	}
	lastbuttonstate = buttonstate;

}

void buttonaction_2(int pin) {
	buttonstate_2 = digitalRead(pin);
	if (action_2 > 6) {
		action_2 = 1;
	}
	if (stopcount > 3) {
		stopcount = 0;
	}

	if (buttonstate_2 != lastbuttonstate_2) {
		if (buttonstate_2 == 0) {

			action_2++;
			stopcount++;
		}

	}
	lastbuttonstate_2 = buttonstate_2;

}

void currentFrame() {
	switch (action) {
	case 1: drawFrame1();
		break;
	case 2: drawFrame2();
		break;
	case 3: drawFrame3();
		break;
	case 4: drawFrame4();
		break;
	case 5: drawFrame5();
		break;
	}

}

void Wifiscan() {
	display.clear();
	display.setFontScale2x2(false);
	display.drawString(0, 10, "Scanning WiFi...");
	display.display();
	int count = WiFi.scanNetworks();
	for (int i = 0; i < 6; i++) {
		SSID_list[i] = WiFi.SSID(i);
		strcpy(SSID_LIST[i], SSID_list[i].c_str());
	}

}

void Wifiselector() {
	if (selected) {
		selecting_number = action_2 - 1;
		action_2 = 1;
		action = 1;
		selecting = true;

	}
	buttonaction_2(BTN_2);
	buttonaction(BTN_1);
	display.clear();
	for (int i = 0; i < 6; i++) {
		display.drawString(18, 10 * i, SSID_list[i]);
	}
	display.drawString(0, 10 * (action_2 - 1), "->");
	display.display();

}