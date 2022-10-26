// Includes

#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>
#include <WiFi.h>
#include "time.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <NTPClient.h>

// Defines

#define TFT_BLACK 0x0000 // black

// Constants and Variables

const char* ssid       = "XXX"; // Wireless Network name
const char* password   = "XXX"; // Wireless Network password

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0; // Set to your time zone offset. 0 is used for UTC
const int   daylightOffset_sec = 0; // If you are on daylight saving

float prices[2];
float mt[2];

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

StaticJsonDocument<4096> jsonBuffer;
String currency[2] = {"USDT"};
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Setup

void setup(void) {
  tft.init();
  tft.setRotation(1);

  Serial.begin(115200);

// Connect to WiFi

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println("CONNECTED");

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printOnScreen();

//  WiFi.disconnect(true);
//  WiFi.mode(WIFI_OFF);

}

// Prints on the screen

void printOnScreen()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }

  HTTPClient http;

      String priceString = "https://api.binance.com/api/v1/ticker/price?symbol=BTCUSDT";
      http.begin(priceString);
      http.GET();
      String payload = http.getString();
      deserializeJson(jsonBuffer, payload);
      prices[0] = jsonBuffer["price"];
      mt[1] = (100000000/(prices[0]));
      http.end();

      String changeString = "https://api.binance.com/api/v3/ticker/24hr?symbol=BTCUSDT";
      http.begin(changeString);
      http.GET();
      payload = http.getString();
      deserializeJson(jsonBuffer, payload);
      prices[1] = jsonBuffer["priceChangePercent"];
      http.end();

      //String BTCprice = "BTC price (" + currency[0] + "): " + String(prices[0]);
      String BTCprice = "$" + String(prices[0]);
      String BTCchange = String(prices[1]) + "%";
      String MoscowTime = String(mt[1]);
      String temp = "_"; // This temp "_" serves to hide the decimal point of the MoscowTime until I find how to shrink the string to 4 symbols only

  tft.fillScreen(TFT_BLACK);
  tft.setCursor(75, 5, 2);
  tft.setTextColor(TFT_ORANGE,TFT_BLACK);
  tft.setTextSize(1);
  tft.println("satsHIVE clock");

  tft.setCursor(0, 120, 2);
  tft.setTextColor(TFT_WHITE,TFT_BLACK);
  tft.setTextSize(1);
  tft.println(&timeinfo,"%a, %d %b. %Y");

  tft.setCursor(200, 120, 2);
  tft.setTextColor(TFT_WHITE,TFT_BLACK);
  tft.setTextSize(1);
  tft.println(&timeinfo,"%H:%M");

  tft.drawString(BTCprice, 0, 100, 2);
    if(prices[1] >= 0)
    {
      tft.setTextColor(TFT_GREEN, TFT_BLACK);
    }
    else if(prices [1] < 0)
    {
      tft.setTextColor(TFT_RED, TFT_BLACK);
    }
  tft.drawString(BTCchange, 195, 100, 2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(10);
  tft.drawString(MoscowTime, 40, 40, 1);
  tft.drawString(temp, 0, 40, 1);
  tft.drawString(temp, 0, 0, 1);
  tft.drawString(temp, 210, 40, 1);
  tft.drawString(temp, 210, 0, 1);

} 

// Loop

void loop() {
delay(60000); //refresh rate in miliseconds
printOnScreen();
}
