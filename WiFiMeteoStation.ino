  
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

String    for0003="";
String    for0306="";
String    for0609="";
String    for0912="";
String    for1215="";
String    for1518="";
String    for1821="";
String    for2100="";
String    forDate="";

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
boolean comingFromSwitch = false;
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
  
  IPAddress ip = WiFi.localIP();
  Serial.print(">>");
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
      printDateTime ( getDay(), getTime() );
    } else {
      //refreshing date and time from web
      refreshDate();
      //displayind date and time on LCD, out of general display function in order to udate date/time in real time.
      printDateTime ( getDay(), getTime() );
    }
  }else{
    // switch is 0, display screen 2 with forecast
    if (!firstRun) {
      firstRun = true;
    }
    currentMillis = millis(); 
    if ( (currentMillis - startMillis >= period) or (firstRunForecast) ) {
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
      printDateTime ( getDay(), getTime() );
    }else{
      //refreshing date and time from web
      refreshDate();
      //displayind date and time on LCD, out of general display function in order to udate date/time in real time.
      printDateTime ( getDay(), getTime() );
    }
  }
}

// =======================================================================================

// get current date
String refreshDate(){
  timeClient.update(); 
}

// =======================================================================================

// get current time
String getTime(){
  timeClient.update();
  String timeS = timeClient.getFormattedTime();
  int length = timeS.length();
  return timeS.substring(length-8,length-3);
}

// =======================================================================================

// get current day
String getDay(){
  timeClient.update();
  return daysOfTheWeek[timeClient.getDay()];
}

// =======================================================================================

// get Weather data from openweathermap.org
// sent request for data
void getWeatherData(){ //client function to send/receive GET request data. 
  
  //ESP.wdtDisable(); //disabling watchdog

  Serial.println ( "Getting Weather from openweathermap.org" );
  HTTPClient http; //Object of class HTTPClient
  http.begin("http://api.openweathermap.org/data/2.5/weather?id="+CityID+"&APPID="+APIKEY);
  int httpCode = http.GET();
  if (httpCode > 0) {
    
    const size_t bufferSize = JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(8) + 370;
    DynamicJsonBuffer jsonBuffer(bufferSize);
    JsonObject& root = jsonBuffer.parseObject(http.getString());
    JsonObject& weatherJ = root["weather"][0];
    String temperatureLOC = root["main"]["temp"];
    String weatherLOC = weatherJ["main"];   
    String descriptionLOC = weatherJ ["description"];
    String idStringLOC = weatherJ["id"];
    String umidityPerLOC = root["main"]["humidity"];
    String windLOC = root["wind"]["speed"];
    String tempMinLOC = root["main"]["temp_min"];
    String tempMaxLOC = root["main"]["temp_max"];
   
    //if the length of description is up than 10 char, truncate to 9 + "."
    if ( descriptionLOC.length() > 9 ){
      descriptionLOC = descriptionLOC.substring (0,9);
      descriptionLOC += ".";
    }
        
    temperature = temperatureLOC;
    weather = weatherLOC;
    description = descriptionLOC;
    idString = idStringLOC;
    umidityPer = umidityPerLOC;
    windS = windLOC;
    tempMin = tempMinLOC;
    tempMax = tempMaxLOC;
    
    Serial.println ("Temperature: " + String(temperature) );
    Serial.println ("Weather: " + String(weather) );
    Serial.println ("Description: " + String(description) );
    Serial.println ("idString: " + String(idString) );
    Serial.println ("Umdity: " + String(umidityPer) );
    Serial.println ("Wind: " + String(windS) );
    Serial.println ("Temp Min: " + String(tempMin) );
    Serial.println ("Temp Max: " + String(tempMax) );

    int length = temperature.length();
    if(length==5){
      temperature.remove(length-3);
    }

    //adjusting temperature
    Fltemperature = temperature.toFloat();
    Fltemperature = Fltemperature - 273,15;
    InTemperature = round(Fltemperature);
  
    FlTempMin = tempMin.toFloat();
    FlTempMin = FlTempMin - 273,15;
    IntTempMin = round(FlTempMin);
  
    FlTempMax = tempMax.toFloat();
    FlTempMax = FlTempMax - 273,15;
    IntTempMax = round(FlTempMax);
  
    weatherID = idString.toInt();
  }
  http.end();
  //ESP.wdtEnable(1000); //enabling watchdog
  
}

// =======================================================================================
// get forecast data from openweathermap.org
// sent request for data

void getForecastWeatherData(){
  ESP.wdtDisable(); //disabling watchdog
  Serial.println ( "Getting Weather from openweathermap.org" );
  HTTPClient http; //Object of class HTTPClient
  http.begin("http://api.openweathermap.org/data/2.5/forecast?id="+CityID+"&APPID="+APIKEY+"&cnt=16");
  int httpCode = http.GET();
  if (httpCode > 0) { 
    const size_t bufferSize = JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(8) + 370;
    DynamicJsonBuffer jsonBuffer(bufferSize);
    JsonObject& root = jsonBuffer.parseObject(http.getString());

    String for0003LOC = root["list"][0]["weather"][0]["main"];
    for0003 = for0003LOC;
    String for0306LOC = root["list"][1]["weather"][0]["main"];
    for0306 = for0306LOC;
    String for0609LOC = root["list"][2]["weather"][0]["main"];
    for0609 = for0609LOC;
    String for0912LOC = root["list"][3]["weather"][0]["main"];
    for0912 = for0912LOC;
    String for1215LOC = root["list"][4]["weather"][0]["main"];
    for1215 = for1215LOC;
    String for1518LOC = root["list"][5]["weather"][0]["main"];
    for1518 = for1518LOC;
    String for1821LOC = root["list"][6]["weather"][0]["main"];
    for1821 = for1821LOC;
    String for2100LOC = root["list"][7]["weather"][0]["main"];
    for2100 = for1821LOC;

    String forDateLOC = root["list"][0]["dt_txt"];
    forDate = forDateLOC;
    
  } else {
    Serial.print ( "Error on geting forecast data: " + String(httpCode) );
  }
  ESP.wdtEnable(1000); //enabling watchdog
}

// =======================================================================================

//Display data on the LCD screen
//Une loop every 60 seconds

void displayHomePage(){
  Serial.println ("...Displaying weather data on LCD..." );
  printGeneral("Montemurlo", timeS, day, weatherID, description, InTemperature, umidityPer, IntTempMin, IntTempMax, windS);
}

void displayForecastData(){
  Serial.println ("...Displaying forecaste data on LCD..." );
  printForecast();
}

// =======================================================================================
// Print Home page with all details
void printGeneral(String city, String timeS, String day, int weatherID, String description, int temperature, String umidity, int tempMin, int tempMax, String wind){
  Serial.println ("...Displaying Home Page on LCD..." );
  tft.fillScreen(BLACK);
  tft.setCursor(2,10);
  tft.setTextColor(RED);
  tft.setTextSize(1);
  tft.print(city);
  tft.print(":");
  //tft.setCursor(2,20);
  tft.setTextColor(GREEN);
  //tft.setTextSize(2);
  tft.print(description);
  printWeatherIcon(weatherID);
  tft.setCursor(2,32);
  tft.setTextColor(GREEN);
  tft.print("Min:");
  tft.setTextColor(WHITE);
  tft.print(tempMin);
  tft.print("'C");
  tft.setCursor(2,42);
  tft.setTextSize(1);
  tft.setTextColor(GREEN);
  tft.print("Max:");
  tft.setTextColor(WHITE);
  tft.print(tempMax);
  tft.print("'C");
  tft.setCursor(2,52);
  tft.setTextSize(1);
  tft.setTextColor(GREEN);
  tft.print("Hum:");
  tft.setTextColor(WHITE);
  tft.print(umidity);
  tft.print("%");
  tft.setCursor(2,62);
  tft.setTextColor(GREEN);
  tft.print("Wnd:");
  tft.setTextColor(WHITE);
  tft.print(wind);
  tft.print("ms");
  tft.setCursor(13,128);
  tft.setTextSize(2);
  tft.setTextColor(BLUE);
  tft.print(temperature);
  tft.print("'C");
  //tft.setCursor(77,128);
  //tft.print(umidity);
  //tft.print("%");
}

void printForecast(){
  Serial.println ("...Displaying Forecast Page on LCD..." );
  tft.fillScreen(BLACK);
  tft.setCursor(2,10);
  tft.setTextColor(RED);
  tft.setTextSize(1);
  tft.print("Montemurlo");
  tft.print(":");
  tft.setTextColor(GREEN);
  tft.print("Forecast");
  tft.setCursor(2,30);
  tft.print(forDate);
  tft.setCursor(2,50);
  tft.setTextSize(1);
  tft.print("00-03: " + for0003);
  tft.setCursor(2,60);
  tft.print("03-06: " + for0306);
  tft.setCursor(2,70);
  tft.print("06-09: " + for0609);
  tft.setCursor(2,80);
  tft.print("09-12: " + for0912);
  tft.setCursor(2,90);
  tft.print("12-15: " + for1215);
  tft.setCursor(2,100);
  tft.print("15-18: " + for1518);
  tft.setCursor(2,110);
  tft.print("18-21: " + for1821);
    tft.setCursor(2,120);
  tft.print("21-00: " + for2100);
}

void printDateTime(String day, String timeS){
  if ( timeS != OLDtimeS ){
    tft.setTextSize(1);
    tft.setCursor(77,128);
    tft.setTextColor(BLACK);
    tft.print(day);
    tft.setCursor(77,128);
    tft.setTextColor(WHITE);
    tft.print(day); 
    tft.setCursor(77,138);
    tft.setTextColor(BLACK);
    tft.print(OLDtimeS);
    tft.setCursor(77,138);
    tft.setTextColor(WHITE);
    tft.print(timeS);
    OLDtimeS = timeS;
    OLDday = day;
  }
  delay(100);
}

// =======================================================================================
// Print WeatherIcon based on id
void printWeatherIcon(int id) {
 Serial.println ("...Printing WeatherIcon on LCD with id: " + String(id) );
 switch(id) {
  case 800: drawClearWeather(); break;
  case 801: drawFewClouds(); break;
  case 802: drawFewClouds(); break;
  case 803: drawCloud(); break;
  case 804: drawCloud(); break;
  
  case 200: drawThunderstorm(); break;
  case 201: drawThunderstorm(); break;
  case 202: drawThunderstorm(); break;
  case 210: drawThunderstorm(); break;
  case 211: drawThunderstorm(); break;
  case 212: drawThunderstorm(); break;
  case 221: drawThunderstorm(); break;
  case 230: drawThunderstorm(); break;
  case 231: drawThunderstorm(); break;
  case 232: drawThunderstorm(); break;

  case 300: drawLightRain(); break;
  case 301: drawLightRain(); break;
  case 302: drawLightRain(); break;
  case 310: drawLightRain(); break;
  case 311: drawLightRain(); break;
  case 312: drawLightRain(); break;
  case 313: drawLightRain(); break;
  case 314: drawLightRain(); break;
  case 321: drawLightRain(); break;

  case 500: drawLightRainWithSunOrMoon(); break;
  case 501: drawLightRainWithSunOrMoon(); break;
  case 502: drawLightRainWithSunOrMoon(); break;
  case 503: drawLightRainWithSunOrMoon(); break;
  case 504: drawLightRainWithSunOrMoon(); break;
  case 511: drawLightRain(); break;
  case 520: drawModerateRain(); break;
  case 521: drawModerateRain(); break;
  case 522: drawHeavyRain(); break;
  case 531: drawHeavyRain(); break;

  case 600: drawLightSnowfall(); break;
  case 601: drawModerateSnowfall(); break;
  case 602: drawHeavySnowfall(); break;
  case 611: drawLightSnowfall(); break;
  case 612: drawLightSnowfall(); break;
  case 615: drawLightSnowfall(); break;
  case 616: drawLightSnowfall(); break;
  case 620: drawLightSnowfall(); break;
  case 621: drawModerateSnowfall(); break;
  case 622: drawHeavySnowfall(); break;

  case 701: drawFog(); break;
  case 711: drawFog(); break;
  case 721: drawFog(); break;
  case 731: drawFog(); break;
  case 741: drawFog(); break;
  case 751: drawFog(); break;
  case 761: drawFog(); break;
  case 762: drawFog(); break;
  case 771: drawFog(); break;
  case 781: drawFog(); break;
  default:break; 
  }
}

// =======================================================================================
// To switch between day/night icon
void nightOrDay(String timeS) {
  timeS = timeS.substring(0,2);
  int time = timeS.toInt();
  if(time > 21 ||  time<7) {
 night = true;
  }else {
 night = false;
  }
}

// =======================================================================================
// Clear the screen

void clearScreen() {
    tft.fillScreen(BLACK);
}

void drawClearWeather(){
  Serial.println ("...Printing ClearWeather on LCD");
  if(night){
    drawTheMoon();
  }else{
    drawTheSun();
  }
}

void drawFewClouds(){
  Serial.println ("...Printing FewClouds on LCD");
  if(night){
    drawCloudAndTheMoon();
  }else{
    drawCloudWithSun();
  }
}

void drawTheSun(){
  Serial.println ("...Printing TheSun on LCD");
  tft.fillCircle(90,80,26,YELLOW);
}

void drawTheFullMoon(){
  Serial.println ("...Printing FullMoon on LCD");
  tft.fillCircle(90,80,26,GREY);
}

void drawTheMoon(){
  Serial.println ("...Printing TheMoon on LCD");
  tft.fillCircle(90,80,26,YELLOW);
  tft.fillCircle(101,73,26,BLACK);
}

void drawCloud(){
  Serial.println ("...Printing Cloud on LCD");
  tft.drawBitmap(20,35,cloud,128,90,WHITE);
}

void drawThermometer(){
  Serial.println ("...Printing Thermometer on LCD");
  tft.drawBitmap(0,40,thermometer,128,90,WHITE);
}

void drawUmidity(){
  Serial.println ("...Printing Humidityon LCD");
  tft.drawBitmap(0,40,umidity,128,90,BLUE);
}

void drawWifi(){
  Serial.println ("...Printing Wifi LCD");
  tft.drawBitmap(0,20,wifi,128,90  ,BLUE);
}

void drawCloudWithSun(){
  tft.fillCircle(90,70,20,YELLOW);  
  tft.drawBitmap(17,36,cloud,128,90,BLACK);
  tft.drawBitmap(17,40,cloud,128,90,WHITE);
}

void drawLightRainWithSunOrMoon(){
  Serial.println ("...Printing RainWithSunOrMoon on LCD");
  if(night){  
    drawCloudTheMoonAndRain();
  }else{
    drawCloudSunAndRain();
  }
}

void drawLightRain(){
  Serial.println ("...Printing LightRain on LCD");
  tft.drawBitmap(20,35,cloud,128,90,WHITE);
  tft.fillRoundRect(70, 105, 3, 13, 1, BLUE);
  tft.fillRoundRect(85, 105, 3, 13, 1, BLUE);
  tft.fillRoundRect(100, 105, 3, 13, 1, BLUE);
}

void drawModerateRain(){
  Serial.println ("...Printing ModerateRain on LCD");
  tft.drawBitmap(20,35,cloud,128,90,0xC618);
  tft.fillRoundRect(70, 105, 3, 15, 1, BLUE);
  tft.fillRoundRect(77, 102, 3, 15, 1, BLUE);
  tft.fillRoundRect(85, 105, 3, 15, 1, BLUE);
  tft.fillRoundRect(92, 102, 3, 15, 1, BLUE);
  tft.fillRoundRect(100, 105, 3, 15, 1, BLUE);
}

void drawHeavyRain(){
  Serial.println ("...Printing HeavyRain on LCD");
  tft.drawBitmap(20,35,cloud,128,90,WHITE);
  tft.fillRoundRect(63, 102, 3, 15, 1, BLUE);
  tft.fillRoundRect(70, 105, 3, 15, 1, BLUE);
  tft.fillRoundRect(77, 102, 3, 15, 1, BLUE);
  tft.fillRoundRect(85, 105, 3, 15, 1, BLUE);
  tft.fillRoundRect(92, 102, 3, 15, 1, BLUE);
  tft.fillRoundRect(100, 105, 3, 15, 1, BLUE);
  tft.fillRoundRect(107, 102, 3, 15, 1, BLUE);
}

void drawThunderstorm(){
  Serial.println ("...Printing Thunderstorm on LCD");
  tft.drawBitmap(20,40,thunder,128,90,YELLOW);
  tft.drawBitmap(20,35,cloud,128,90,WHITE);
  tft.fillRoundRect(68, 102, 3, 15, 1, BLUE);
  tft.fillRoundRect(75, 102, 3, 15, 1, BLUE);
  tft.fillRoundRect(94, 102, 3, 15, 1, BLUE);
  tft.fillRoundRect(102, 102, 3, 15, 1, BLUE);
}

void drawLightSnowfall(){
  Serial.println ("...Printing LightSnowfall on LCD");
  tft.drawBitmap(20,30,cloud,128,90,WHITE);
  tft.fillCircle(70, 100, 3, GREY);
  tft.fillCircle(85, 103, 3, GREY);
  tft.fillCircle(102, 100, 3, GREY);
}

void drawModerateSnowfall(){
  Serial.println ("...Printing ModerateSnowfall on LCD");
  tft.drawBitmap(20,35,cloud,128,90,WHITE);
  tft.fillCircle(70, 105, 3, GREY);
  tft.fillCircle(75, 115, 3, GREY);
  tft.fillCircle(85, 108, 3, GREY);
  tft.fillCircle(90, 118, 3, GREY);
  tft.fillCircle(102, 105, 3, GREY);
  tft.fillCircle(107, 115, 3, GREY);
}

void drawHeavySnowfall(){
  Serial.println ("...Printing HeavySnowfall on LCD");
  tft.drawBitmap(20,35,cloud,128,90,WHITE);
  tft.fillCircle(60, 105, 3, GREY);
  tft.fillCircle(72, 105, 3, GREY);
  tft.fillCircle(77, 115, 3, GREY);
  tft.fillCircle(85, 108, 3, GREY);
  tft.fillCircle(90, 118, 3, GREY);
  tft.fillCircle(100, 105, 3, GREY);
  tft.fillCircle(105, 115, 3, GREY);
  tft.fillCircle(112, 105, 3, GREY);
}

void drawCloudSunAndRain(){
  Serial.println ("...Printing CloudSunAndRain on LCD");
  tft.fillCircle(93,70,20,YELLOW);
  tft.drawBitmap(20,32,cloud,128,90,BLACK);
  tft.drawBitmap(20,35,cloud,128,90,WHITE);
  tft.fillRoundRect(70, 105, 3, 13, 1, BLUE);
  tft.fillRoundRect(85, 105, 3, 13, 1, BLUE);
  tft.fillRoundRect(100, 105, 3, 13, 1, BLUE);
}

void drawCloudAndTheMoon(){
  Serial.println ("...Printing CloudAndTheMoon on LCD");
  tft.fillCircle(110,60,18,YELLOW);
  tft.fillCircle(121,53,18,BLACK);
  tft.drawBitmap(16,32,cloud,128,90,BLACK);
  tft.drawBitmap(16,35,cloud,128,90,WHITE);
}

void drawCloudTheMoonAndRain(){
  Serial.println ("...Printing CloudTheMoonAndRain on LCD");
  tft.fillCircle(110,60,18,YELLOW);
  tft.fillCircle(121,53,18,BLACK);
  tft.drawBitmap(16,32,cloud,128,90,BLACK);
  tft.drawBitmap(16,35,cloud,128,90,WHITE);
  tft.fillRoundRect(65, 105, 3, 11, 1, BLUE);
  tft.fillRoundRect(80, 105, 3, 11, 1, BLUE);
  tft.fillRoundRect(95, 105, 3, 11, 1, BLUE);
}

void drawWind(){  
  Serial.println ("...Printing Wind on LCD");
  tft.drawBitmap(0,35,wind,128,90,GREY);   
}

void drawFog()  {
  Serial.println ("...Printing Fog on LCD");
  tft.fillRoundRect(65, 60, 40, 4, 1, GREY);
  tft.fillRoundRect(60, 70, 50, 4, 1, GREY);
  tft.fillRoundRect(55, 80, 60, 4, 1, GREY);
  tft.fillRoundRect(60, 90, 50, 4, 1, GREY);
  tft.fillRoundRect(65, 100, 40, 4, 1, GREY);
}

void clearIcon(){
  Serial.println ("...clearIcon for LCD");
  tft.fillRect(0,40,128,100,BLACK);
}
