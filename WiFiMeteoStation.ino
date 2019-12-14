  
// To develop => @TODO placeholder

// ----------------------------------------------------------------------------------------

//Used Library
#include <FS.h>                               //this needs to be first, or it all crashes and burns...
#include <NTPClient.h>                        // Date/Time manager 
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>                          // Library to send and receive UDP messages
#include <WiFiClient.h>                       // driver for WiFi client
#include <ArduinoJson.h>                      // Arduino Json to parse reauest into JSON object. Installed version 5.13, last version is not compatible.
#include <Adafruit_ST7735.h>                  // LCD driver
#include <Adafruit_GFX.h>                     // LCD graphical driver
#include <WiFiManager.h>                      // https://github.com/tzapu/WiFiManager
#include <math.h>
#include <time.h>
#include <ESP8266HTTPClient.h>

// ----------------------------------------------------------------------------------------

// Color definitions
#define BLACK     0x0000
#define BLUE      0xF800
#define RED       0x001F
#define GREEN     0x07E0
#define CYAN      0xFFE0
#define MAGENTA   0xF81F
#define YELLOW    0x07FF
#define WHITE     0xFFFF
#define GREY      0xC618

// ----------------------------------------------------------------------------------------

// wifi sid and password (hardcoded for the moment)
// @TODO: MAKE ssid and password NOT hardcoded?

// ----------------------------------------------------------------------------------------

// APIKEY is a passtoken used to identify the request from data to "pool.ntp.org
String APIKEY = "a846fde251b5325d05cf645176fb2a3d"; // API Key 
// @TODO: Make CityID configurable
String CityID = "6541554"; // Montemurlo, ITA
// @TODO: Make Timezone configurable
int TimeZone = 1;// GMT +1
// @TODO: Make utcOffsetInSeconds configurable
const long utcOffsetInSeconds = 3600;
char daysOfTheWeek[7][12] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"}; 

// Define NTP Client to get time and date
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

WiFiClient client;    // WIFI Client
char servername[]="api.openweathermap.org";    // remote server we will connect to
String result;

// ----------------------------------------------------------------------------------------
// set of variables used into this sketch for different pourpose
boolean   night = false;

String    timeS = "";
String    day = "";
String    OLDtimeS = "";
String    OLDday = "";

int       weatherID = 0;
String    location = "";
String    temperature = "";
String    tempMin = "";
String    tempMax = "";
String    weather = "";
String    description = "";
String    idString = "";
String    umidityPer = "";
float     Fltemperature = 0;
int       InTemperature = 0;
int       IntTempMin = 0;
int       IntTempMax = 0;
float     FlTempMin = 0;
float     FlTempMax = 0; 
String    windS = "";

String    for1="";
String    for2="";
String    for3="";
String    for4="";
String    for5="";
String    for6="";
String    for7="";
String    for8="";
String    forDate1="";
String    forDate2="";
String    forDate3="";
String    forDate4="";
String    forDate5="";
String    forDate6="";
String    forDate7="";
String    forDate8="";

// ----------------------------------------------------------------------------------------
// images used
extern  unsigned char  cloud[]; 
extern  unsigned char  thunder[];
extern  unsigned char  wind[];
extern  unsigned char  thermometer[];
extern  unsigned char  umidity[];
extern  unsigned char  wifi[];

// ----------------------------------------------------------------------------------------
// LCD definition
// @TODO set the PIN as variable
Adafruit_ST7735 tft = Adafruit_ST7735(15, 12, 13, 14, 0);   

//flag for saving data
bool shouldSaveConfig = false;

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

//REFRESH DATA
boolean firstRun = true;
boolean firstRunForecast = true;
unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 30 * 60 * 1000; // mm * ss * millisecond

// WiFiManager
// Local intialization. Once its business is done, there is no need to keep it around
WiFiManager wifiManager;

// Variable to store the HTTP request
String header;

//pit to switch the screen
int switchScreenPIn = 2;
int valSwitchScreenPIn= 0;      

IPAddress ip;

// =======================================================================================
// S E T U P
// =======================================================================================
void setup() {
    
  //switchScreenPIn inizialization
  pinMode(switchScreenPIn, INPUT); 
  
  Serial.begin(9600);

  //start of couting of millisecond from start of sketch
  startMillis = millis();  //initial start time
  
  // LCD: initialize a ST7735S chip, green tab
  tft.initR(INITR_GREENTAB);    
  tft.fillScreen(BLACK);    
  
  // Uncomment and run it once, if you want to erase all the stored information
  //wifiManager.resetSettings();

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  // set custom ip for portal
  //wifiManager.setAPConfig(IPAddress(10,0,1,1));

  drawWifi();
  
  tft.setCursor(1,110);
  tft.println ("To provide credential");
  tft.println ("use accesspoint");
  tft.setTextColor(GREEN);
  tft.println ( "WiFiMeteoStationAP"); 
  
  // fetches ssid and pass from eeprom and tries to connect
  // if it does not connect it starts an access point with the specified name
  // here  "AutoConnectAP"
  // and goes into a blocking loop awaiting configuration
  wifiManager.autoConnect("WiFiMeteoStationAP");
  // or use this for auto generated name ESP + ChipID
  //wifiManager.autoConnect();
  
  //set minimu quality of signal so it ignores AP's under that quality
  //defaults to 8%
  //wifiManager.setMinimumSignalQuality();

  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep
  //in seconds
  //wifiManager.setTimeout(120);

  // if you get here you have connected to the WiFi
  // Serial.println("Connected.");
  
  ip = WiFi.localIP();
  Serial.println(ip);
    
  timeClient.begin();

}

// =======================================================================================
// L O O P
// =======================================================================================
void loop() {

  valSwitchScreenPIn = digitalRead(switchScreenPIn);
  
  // switch is 1, display screen 1 with current weather
  if (valSwitchScreenPIn == 1) {
    if (!firstRunForecast) {
      firstRunForecast = true;
    }
    currentMillis = millis(); 
    if ( (currentMillis - startMillis >= period) or (firstRun) ) {
      startMillis = currentMillis;
      firstRun = false;
      //get current time
      timeS = getTime();
      Serial.println ( "Getting current time: " + String(timeS) );
      //get current day
      day = getDay();
      Serial.println ( "Getting current day: " + String (day) );
      //to define if we are in night or day (to display moon or sun)
      nightOrDay (timeS);
      Serial.println ( "Getting data from openweathermap.org" );
      getWeatherData();
      Serial.println ( "Display Weather data " );
      displayHomePage();
      refreshDate();
      printDateTime ( getDay(), getTime(), 65, 25 );
    } else {
      //refreshing date and time from web
      refreshDate();
      //displayind date and time on LCD, out of general display function in order to udate date/time in real time.
      printDateTime ( getDay(), getTime(), 65, 25 );
    }
  }else{
    // switch is 0, display screen 2 with forecast
    if (!firstRun) {
      firstRun = true;
    }
    currentMillis = millis(); 
    if ( (currentMillis - startMillis >= period) or (firstRunForecast) ) {
      startMillis = currentMillis;
      firstRunForecast = false;
      //get current time
      timeS = getTime();
      Serial.println ( "Getting current time: " + String(timeS) );
      //get current day
      day = getDay();
      Serial.println ( "Getting current day: " + String (day) );
      //to define if we are in night or day (to display moon or sun)
      nightOrDay (timeS);
      Serial.println ( "Getting data from openweathermap.org" );
      getForecastWeatherData();
      Serial.println ( "Display Forecast data " );
      displayForecastData();
      refreshDate();
      printDateTime ( getDay(), getTime(), 80, 128 );
    }else{
      //refreshing date and time from web
      refreshDate();
      //displayind date and time on LCD, out of general display function in order to udate date/time in real time.
      printDateTime ( getDay(), getTime(), 60, 128 );
    }
  }
}
