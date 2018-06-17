#include <Wire.h>
#include "ssd1306_i2c.h"
#include "images.h"
#include "WundergroundClient.h"
#include "TimeClient.h"

#include <ESP8266WiFi.h>
#include <JsonListener.h>

#define I2C 0x3c
#define SDA D6
#define SCL D7
#define BTN_1 D5
#define BTN_2 D1

//wunderground
const String WUNDERGROUND_API_KEY = "df1b9420db9c8d85";
const String  WUNDERGROUND_COUNTRY = "KO";
const String  WUNDERGROUND_CITY = "RKTF";
const String  WUNDERGRROUND_LANGUAGE = "EN";
const boolean IS_METRIC = true;
const boolean USE_PM = true;

//Timeclient
const float UTC = 9;

//Objects
// 0x3D is the adafruit address....
SSD1306 display(I2C, SDA, SCL);
WundergroundClient wunderground(IS_METRIC);
TimeClient timeclient(UTC);
//Frames
void drawFrame1(int,int);
void drawFrame2(int,int);
void drawFrame3(int,int);
void drawFrame4(int,int);
void drawFrame5(int,int);
void drawFrame6(int, int);

// your network SSID (name)
char pass[] = "";

// stop watch
long previous = 0;
long current = 0;
int sec_f = 0;
int min_f = 0;
int hour_f = 0;
bool ready = true;
String Second;
String Minute;
String Hour;

// flag changed in the ticker function every 10 minutes
bool readyForWeatherUpdate = true;
bool selecting = false;

//SSID_LIST
String SSID_list[6];
char SSID0[100];
char SSID1[100];
char SSID2[100];
char SSID3[100];
char SSID4[100];
char SSID5[100];
char* SSID_LIST[6] = {SSID0,SSID1,SSID2,SSID3,SSID4,SSID5 };
int selecting_number = 0;

//BTN struct
struct BTN
{
	int buttonstate = 0;
	int lastbuttonstate = 1;
	int count = 1;
	int stopwatch_state = 0;
	bool selected = false;
};

struct BTN BTN[2];


void setup() {
  delay(500);

  pinMode(BTN_1, INPUT_PULLUP);
  pinMode(BTN_2, INPUT_PULLUP);
  // initialize display and settings
  display.init();
  display.clear();
  display.flipScreenVertically();
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
	buttonaction(0);
	display.clear();
	currentFrame();
}

void updateData() {
	display.clear();
	display.drawString(10,10,"Information...");
	display.display();
	wunderground.updateConditions(WUNDERGROUND_API_KEY, WUNDERGRROUND_LANGUAGE, WUNDERGROUND_COUNTRY, WUNDERGROUND_CITY);
	wunderground.updateForecast(WUNDERGROUND_API_KEY, WUNDERGRROUND_LANGUAGE,WUNDERGROUND_COUNTRY, WUNDERGROUND_CITY);
	wunderground.updateAstronomy(WUNDERGROUND_API_KEY, WUNDERGRROUND_LANGUAGE, WUNDERGROUND_COUNTRY, WUNDERGROUND_CITY);
	timeclient.updateTime();
	delay(500);
	WiFi.disconnect();

	readyForWeatherUpdate = false;
	display.clear();
	display.display();
	delay(500);
}


void setReadyForWeatherUpdate() {
  readyForWeatherUpdate = true;
}

void drawFrame1(int x, int y) {
	display.clear();
	display.setFontScale2x2(false);
	display.drawString(22 + x , 10+ y , wunderground.getDate().substring(0, 11));

	int hour_i = atoi(timeclient.getHours().c_str());
	int minute_i = atoi(timeclient.getMinutes().c_str());
	int second_i = atoi(timeclient.getSeconds().c_str());
	String M = "AM";
	String hour = String(hour_i);
	String minute = String(minute_i);
	String second = String(second_i);
	if (hour_i >= 12) {
		M = "PM";
		if (hour_i > 12) {
			hour_i -= 12;
			hour = String(hour_i);
		}

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
	display.drawString(18+ x , 23+ y , hour + ":" + minute);
	display.setFontScale2x2(false);
	display.drawString(99+ x , 20+ y , M);
	display.drawString(99+x , 30+ y , second);
	display.display();
}



void drawFrame2(int x, int y) {
	display.clear();
	display.setFontScale2x2(false);
	display.drawString(65+ x , 0+ y , "Today");
	display.drawXbm(0+ x, 0+ y, 60, 60, xbmtemp);
	display.setFontScale2x2(true);
	String temp = wunderground.getCurrentTemp();
	display.drawString(64+ x , 14+ y , temp + "C");
	display.drawString(64+ x , 32+ y , wunderground.getHumidity());
	display.display();
}

void drawFrame3(int x, int y) {
	display.clear();
	display.setFontScale2x2(false);
	display.drawString(70 +x, 0+ y, "Today");
	display.drawXbm(10+ x, 0+ y, 50, 50, getIconFromString(wunderground.getTodayIcon()));
	display.setFontScale2x2(true);
	display.drawString(69+ x, 14+ y, wunderground.getCurrentTemp() + "C");
	display.setFontScale2x2(false);
	String high = wunderground.getForecastHighTemp(0);
	String low = wunderground.getForecastLowTemp(0);
	display.drawString(71+ x, 38+ y, high + "/" + low);
	display.display();
}

void drawFrame4(int x, int y) {
	display.clear();
	display.setFontScale2x2(false);
	display.drawString(65+ x, 0+ y, "Tomorrow");
	display.drawXbm(10+ x, 0+ y, 50, 50, getIconFromString(wunderground.getForecastIcon(2)));
	display.setFontScale2x2(true);
	String high = wunderground.getForecastHighTemp(2);
	String low = wunderground.getForecastLowTemp(2);
	display.drawString(71+ x, 14+ y, high + "C");
	display.drawString(71+ x, 32+ y, low + "C");
	display.display();
}

void drawFrame5(int x, int y) {
	display.clear();
	display.setFontScale2x2(false);
	display.drawString(65 + x, 0 + y, "Moon");
	display.drawXbm(5 + x, 0 + y, 50, 50, getIconOfMoon(wunderground.getMoonPhase()));
	display.setFontScale2x2(true);
	display.drawString(64 + x, 15 + y, wunderground.getMoonPctIlum() + "%");
	display.setFontScale2x2(false);
	String rise = wunderground.getMoonriseTime();
	String set = wunderground.getMoonsetTime();
	display.drawString(66 + x, 33 + y, rise);
	display.drawString(66 + x, 43 + y, set);
	display.display();
}

void drawFrame6(int x, int y) {
	display.clear();
		stop_watch();
		buttonaction(1);
	Second = String(sec_f);
	Minute = String(min_f);
	Hour = String(hour_f);
	if (sec_f < 10) {
		Second = "0" + Second;
	}
	if (min_f < 10) {
		Minute = "0" + Minute;
	}
	if (hour_f < 10) {
		Hour = "0" + Hour;
	}
	display.setFontScale2x2(false);
	display.drawString(22+ x, 15+ y, "STOP_WATCH");
	display.setFontScale2x2(true);
	display.drawString(18+ x, 28+ y, Hour + ":" + Minute);
	display.setFontScale2x2(false);
	display.drawString(99+ x, 35+ y, Second);
	display.display();

}

	
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

const char* getIconOfMoon(String icon) {
	if (icon == "New") {
		return moon0_bits;
	}
	else if (icon == "Waxing Crescent") {
			return moon30_bits;
	}
	else if (icon == "Waning Crescent") {
			return moon30_2_bits;
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

//BTN[0]  --> BTN_1 D5 -->stop_watch, list ->
//BTN[1]  --> BTN_2 D1 -->selecting
void buttonaction(int btn) {
		BTN[0].buttonstate = digitalRead(BTN_1);
		BTN[1].buttonstate = digitalRead(BTN_2);

	if (BTN[0].count > 6) {
		BTN[0].count = 1;
	}

	if (BTN[1].count > 6) {
		BTN[1].count = 1;
	}

	if (BTN[btn].stopwatch_state > 2) {
		BTN[btn].stopwatch_state = 0;
	}
	if (BTN[btn].buttonstate != BTN[btn].lastbuttonstate) {
		if (BTN[btn].buttonstate == 0) {
			BTN[btn].count++;
			BTN[btn].selected = true;
			BTN[btn].stopwatch_state++;
		}
	}
	BTN[btn].lastbuttonstate = BTN[btn].buttonstate;
}

void currentFrame() {
	switch (BTN[0].count) {
	case 1: drawFrame1(0,0);
		break;
	case 2: drawFrame2(0,0);
		break;
	case 3: drawFrame3(0,0);
		break;
	case 4: drawFrame4(0,0);
		break;
	case 5: drawFrame5(0,0);
		break;
	case 6: drawFrame6(0, -5);
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
	if (BTN[0].selected) {
		selecting_number = BTN[1].count - 1;
		BTN[1].count = 1;
		BTN[0].count = 1;
		selecting = true;
	}
	buttonaction(0);
	buttonaction(1);
	display.clear();
	for (int i = 0; i < 6; i++) {
		display.drawString(18, 10 * i, SSID_list[i]);
	}
	display.drawString(0, 10 * (BTN[1].count - 1), "->");
	display.display();

}

void stop_watch() {
	if (BTN[1].stopwatch_state == 0) {
		sec_f = 0;
		min_f = 0;
		hour_f = 0;
		previous = 0;
		current = 0;
		ready = true;
	}
	if (BTN[1].stopwatch_state == 1 && ready) {
		previous = millis();
		ready = false;
	}
	if (BTN[1].stopwatch_state == 1) {
		current = millis();
	}
	int t = current - previous;
	if (t > 1000) {
		sec_f += 1;
		previous = current;
	}
	if (sec_f >= 60) {
		min_f += 1;
		sec_f = 0;
	}
	if (min_f >= 60) {
		hour_f += 1;
		min_f = 0;
	}
}