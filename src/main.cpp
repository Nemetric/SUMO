/*
ESP32 specific lib for LEDs - https://github.com/MartyMacGyver/ESP32-Digital-RGB-LED-Drivers
http://esp32.net/images/Heltec/WIFI-Kit-32/
https://espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf
ADC Input Ranges - https://www.esp32.com/viewtopic.php?t=1045

*/
#include <Wifi.h>
#include "aREST.h"
#include "Motor.h"
#include <OLED.h>

#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <SPIFFS.h>
#include <ArduinoOTA.h>
#include <HTTPFileServer.h>
#include <LedStrip.h>
//#include "FastLED.h"

HTTPFileServer fs1;

Motor motorLeft;
Motor motorRight;

aREST rest = aREST();

OLED disp = OLED();

//CRGB leds[5];
//Adafruit_NeoPixel pixels = Adafruit_NeoPixel(5, 35, NEO_GRB + NEO_KHZ800);
LedStrip ls;

const char *ssid = "ROBOT_SUMO";
const char *password = "";
const char *host = "robot";

String HostName = "http://";

void ConnectToWIFI();
void SetHostName();

#define LISTEN_PORT 8080

WiFiServer wfserver(LISTEN_PORT);

int newxy(String command);
int auth(String command);

unsigned long lastActivity;

void setup()
{
	pinMode(25, OUTPUT);
	digitalWrite(25, HIGH);


	
	disp.init();
	disp.writeNEMETRIC();


	Serial.begin(115200);

	ls.Init();
	
	//FastLED.addLeds<WS2812B, 0, RGB>(leds, 5);
	//FastLED.addLeds<NEOPIXEL, 0>(leds, 5);

	// Turn the LED on, then pause
	//leds[0] = CRGB::Pink;
	//leds[1] = CRGB::Pink;
	//leds[2] = CRGB::DeepPink;
	//leds[3] = CRGB::HotPink;
	//leds[4] = CRGB::Blue;
	//FastLED.show();


	// Init motor shield
	motorLeft.setInputs(21, 22, 1, 2); // Pin One , Pin Two and PWM pin
	motorRight.setInputs(18, 23, 3, 4);
	pinMode(19, OUTPUT);
	digitalWrite(19, HIGH);

	// REST Functions
	rest.function("newxy", newxy);
	rest.function("auth", auth);

	rest.set_id("123456");
	rest.set_name("robot2");

	ConnectToWIFI();
	SetHostName();

	// Start the server
	wfserver.begin();
	Serial.println("Server started");

	// Print the IP address
	Serial.println(WiFi.localIP());

	MDNS.begin(host);

	fs1.Setup();

	disp.writeYETIBOTICS();
	//disp.WriteString("Connect: ", 0, -90);
	disp.WriteString(HostName, 0, -100);

	lastActivity = millis();

	
}

void loop()
{
	//ls.Task();
	//sleep after 5 minutes of no activity
	if((lastActivity + (60000 * 1)) < millis())
	{
		//SLEEP
		ls.LightsOff();
		ls.LightColor(2, 10, 10, 10);
		digitalWrite(19, LOW);
		esp_deep_sleep_start();
	}


	ArduinoOTA.handle();
	fs1.Task();

	// Handle REST calls
	WiFiClient client = wfserver.available();
	if (!client)
	{
		return;
	}
	while (!client.available())
	{
		delay(1);
	}

	rest.handle(client);

}
int authToken = 0;
int auth(String command)
{

	if (authToken == 0)
	{
		authToken = random(1, 99999);
		disp.WriteString("Device Locked!       ", 0, -100);
		ls.HeadlightsON();
		return authToken;
	}

	return 0;
}

String getValue(String data, char separator, int index)
{
	int found = 0;
	int strIndex[] = {0, -1};
	int maxIndex = data.length() - 1;

	for (int i = 0; i <= maxIndex && found <= index; i++)
	{
		if (data.charAt(i) == separator || i == maxIndex)
		{
			found++;
			strIndex[0] = strIndex[1] + 1;
			strIndex[1] = (i == maxIndex) ? i + 1 : i;
		}
	}
	return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

int newxy(String command)
{
	//Serial.println(command);
	lastActivity = millis();

	int xval = getValue(command, ',', 0).toInt();
	int yval = getValue(command, ',', 1).toInt();
	int tkn = getValue(command, ',', 2).toInt();

	if (tkn == authToken)
	{
		int leftSpeed = map(yval + xval, -100, 100, -255, 255);
		int rightSpeed = map(yval - xval, -100, 100, -255, 255);

		if (leftSpeed > 255)
			leftSpeed = 255;
		else if (leftSpeed < -255)
			leftSpeed = -255;

		if (rightSpeed > 255)
			rightSpeed = 255;
		else if (rightSpeed < -255)
			rightSpeed = -255;

		if (leftSpeed == 0)
			motorLeft.stop();
		else if (leftSpeed > 0)
			motorLeft.goForward(leftSpeed);
		else
			motorLeft.goBackward(leftSpeed * -1);

		if (rightSpeed == 0)
			motorRight.stop();
		else if (rightSpeed > 0)
			motorRight.goForward(rightSpeed);
		else
			motorRight.goBackward(rightSpeed * -1);

		Serial.print("NewXY: ");
		Serial.print(leftSpeed);
		Serial.print("::");
		Serial.println(rightSpeed);
	}
	return 1;
}

void ConnectToWIFI()
{

	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.println("WiFi connected");

	ArduinoOTA
		.onStart([]() {
			String type;
			if (ArduinoOTA.getCommand() == U_FLASH)
				type = "sketch";
			else // U_SPIFFS
				type = "filesystem";

			// NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
			Serial.println("Start updating " + type);
		})
		.onEnd([]() {
			Serial.println("\nEnd");
		})
		.onProgress([](unsigned int progress, unsigned int total) {
			Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
		})
		.onError([](ota_error_t error) {
			Serial.printf("Error[%u]: ", error);
			if (error == OTA_AUTH_ERROR)
				Serial.println("Auth Failed");
			else if (error == OTA_BEGIN_ERROR)
				Serial.println("Begin Failed");
			else if (error == OTA_CONNECT_ERROR)
				Serial.println("Connect Failed");
			else if (error == OTA_RECEIVE_ERROR)
				Serial.println("Receive Failed");
			else if (error == OTA_END_ERROR)
				Serial.println("End Failed");
		});

	ArduinoOTA.begin();
}

void SetHostName()
{
	String mac = WiFi.macAddress();
	if (mac == "30:AE:A4:55:E9:98")
		HostName += "r1.su.mo";
}