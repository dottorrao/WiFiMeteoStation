
// To develop => @TODO placeholder

// ----------------------------------------------------------------------------------------

//Used Library
#include <NTPClient.h>                        // Date/Time manager 
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>                          // Library to send and receive UDP messages
#include <WiFiClient.h>                       // driver for WiFi client
#include <ArduinoJson.h>                      // Arduino Json to parse reauest into JSON object. Installed version 5.13, last version is not compatible.
#include <Adafruit_ST7735.h>                  // LCD driver
#include <Adafruit_GFX.h>                     // LCD graphical driver

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
#ifndef APSSID
#define APSSID "Saturno"
#define APPSK  "mamalorechia"
#endif
/* Set these to your desired credentials. */
char *ssid = APSSID;
char *password = APPSK;

// ----------------------------------------------------------------------------------------

// APIKEY is a passtoken used to identify the request from data to "pool.ntp.org
String APIKEY = "<APIKEYHERE>"; // API Key 
// @TODO: Make CityID configurable
String CityID = "6541554"; // Montemurlo, ITA
// @TODO: Make Timezone configurable
int TimeZone = 1;// GMT +1
// @TODO: Make utcOffsetInSeconds configurable
const long utcOffsetInSeconds = 7200;
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
int       weatherID = 0;
String    location = "";
String    temperature = "";
String    weather = "";
String    description = "";
String    idString = "";
String    umidityPer = "";
float     Fltemperature = 0;
int       counter = 30;
String    windS = "";


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

// =======================================================================================
// S E T U P
// =======================================================================================
void setup() {
  Serial.begin(9600);

  // LCD: initialize a ST7735S chip, green tab
  tft.initR(INITR_GREENTAB);    
  tft.fillScreen(BLACK);    
  // wifi connection
  drawWifi();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED ){
    delay(500);
    Serial.print(".");
    tft.setCursor(20,110);
    tft.print ( "Try connect to: " );
    tft.setCursor(20,120);
    tft.print ( ssid );
  }
  IPAddress myIP = WiFi.localIP();
  tft.setCursor(20,130);
  tft.print ( "Connected!" );
  tft.setCursor(20,140);
  tft.print ( myIP );
  // client to get date and time
  // @TODO: implement better this step with https://github.com/scanlime/esp8266-Arduino/blob/master/tests/Time/Time.ino
  timeClient.begin();
  delay (2000);
}

// =======================================================================================
// L O O P
// =======================================================================================
void loop() {
  
  if(counter == 30) {//Get new data every 30 cycles (1 cycles = 60 sec)
    counter = 0;
    getWeatherData();
  }else{
    displayData();
    counter++;
    Serial.println(counter); 
  } 
  
  //get current time
  timeS = getTime();
  //get current day
  day = getDay();
  //to define if we are in night or day (to display moon or sun)
  nightOrDay (timeS);
  
}
// =======================================================================================

// get current date
String getDate(){
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
  if (client.connect(servername, 80)) {  //starts client connection, checks for connection
    client.println("GET /data/2.5/weather?id="+CityID+"&APPID="+APIKEY);
    client.println("Host: api.openweathermap.org");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();
  } else {
    Serial.println("connection failed"); //error message if no client connect
    Serial.println();
  }

  // reading sent data
  while(client.connected() && !client.available()) delay(1); //waits for data
  Serial.println("Waiting for data");
  while (client.connected() || client.available()) { //connected or data available
    char c = client.read(); //gets byte from ethernet buffer
    result = result+c;
  }

  // replacing character '['
  client.stop(); //stop client
  result.replace('[', ' ');
  result.replace(']', ' ');
  
  Serial.println(result);

  // format received data into a jsonArray.
  // to make this code working it has been becessary to install version 
  char jsonArray [result.length()+1];
  result.toCharArray(jsonArray,sizeof(jsonArray));
  jsonArray[result.length() + 1] = '\0';
  StaticJsonBuffer<1024> json_buf;
  JsonObject &root = json_buf.parseObject(jsonArray);
  if (!root.success()){
    Serial.println("parseObject() failed");
  }
  
  //TODO : try to understand why this double assignement is necessary
  String temperatureLOC = root["main"]["temp"];
  String weatherLOC = root["weather"]["main"];
  String descriptionLOC = root["weather"]["description"];
  String idStringLOC = root["weather"]["id"];
  String umidityPerLOC = root["main"]["humidity"];
  String windLOC = root["wind"]["speed"];

  temperature = temperatureLOC;
  weather = weatherLOC;
  description = descriptionLOC;
  idString = idStringLOC;
  umidityPer = umidityPerLOC;
  windS = windLOC;

  int length = temperature.length();
  if(length==5){
    temperature.remove(length-3);
  }

  Fltemperature = temperature.toFloat();
  Fltemperature = Fltemperature - 273,15;
  
  weatherID = idString.toInt();
}

// =======================================================================================

//Display data on the LCD screen
//Une loop every 60 seconds

void displayData(){
  printGeneral("Montemurlo", timeS, day, weatherID, description, Fltemperature, umidityPer);
  delay (45000);
  //printWeather("Montemurlo", timeS, day, weatherID, description);
  //delay (pause);
  printTemperature("Montemurlo", timeS, day, Fltemperature);
  delay (5000);
  printUmidity("Montemurlo", timeS, day, umidityPer);
  delay (5000);
  printWind("Montemurlo", timeS, day, windS);
  delay (5000);
}

// =======================================================================================
// Print Home page with all details
void printGeneral(String city, String timeS, String day, int weatherID, String description, float temperature, String umidity){
  tft.fillScreen(BLACK);

  tft.setCursor(10,10);
  tft.setTextColor(RED);
  tft.setTextSize(1);
  tft.print(city);

  tft.setCursor(10,20);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print(timeS + ' ' + day);

  printWeatherIcon(weatherID);

  tft.setCursor(2,112);
  tft.setTextColor(GREEN);
  tft.setTextSize(1);
  tft.print(description);
  tft.setCursor(2,122);
  tft.setTextColor(WHITE);
  tft.print("Temperature:");
  tft.setTextColor(GREEN);
  tft.print(temperature);
  tft.print("'C");
  tft.setCursor(2,132);
  tft.setTextColor(WHITE);
  tft.print("Umidity:");
  tft.setTextColor(GREEN);
  tft.print(umidity);
  tft.print("%");
  tft.setCursor(2,142);
  tft.setTextColor(WHITE);
  tft.print("Wind:");
  tft.setTextColor(GREEN);
  tft.print(windS);
  tft.print("m/s");
}

// =======================================================================================
// Print Weather with icon
void printWeather(String city, String timeS, String day, int weatherID, String description) {
  tft.fillScreen(BLACK);

  tft.setCursor(10,10);
  tft.setTextColor(RED);
  tft.setTextSize(1);
  tft.print(city);

  tft.setCursor(10,20);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print(timeS + ' ' + day);

  printWeatherIcon(weatherID);

  tft.setCursor(1,122);
  tft.setTextColor(GREEN);
  tft.setTextSize(2);
  tft.print(description);
}

// =======================================================================================
// Print temperature display
void printTemperature(String city, String timeS, String day, float temperature){

  tft.fillScreen(BLACK);

  tft.setCursor(10,10);
  tft.setTextColor(RED);
  tft.setTextSize(1);
  tft.print(city);

  tft.setCursor(10,20);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print(timeS + ' ' + day);

  drawThermometer();

  tft.setCursor(30,135);
  tft.setTextColor(GREEN);
  tft.setTextSize(2);
  tft.print(temperature);
  tft.print("'C");
}

// =======================================================================================
// Print umidity display
void printUmidity(String city, String timeS, String day, String umidity){

  tft.fillScreen(BLACK);

  tft.setCursor(10,10);
  tft.setTextColor(RED);
  tft.setTextSize(1);
  tft.print(city);

  tft.setCursor(10,20);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print(timeS + ' ' + day);

  drawUmidity();

  tft.setCursor(47,132);
  tft.setTextColor(GREEN);
  tft.setTextSize(2);
  tft.print(umidity);
  tft.print("%");
}

// =======================================================================================
// Print wind display
void printWind(String city, String timeS, String day, String wind){

  tft.fillScreen(BLACK);

  tft.setCursor(10,10);
  tft.setTextColor(RED);
  tft.setTextSize(1);
  tft.print(city);

  tft.setCursor(10,20);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print(timeS + ' ' + day);

  drawWind();

  tft.setCursor(30,132);
  tft.setTextColor(GREEN);
  tft.setTextSize(2);
  tft.print(wind);
  tft.print("m/s");
}

// =======================================================================================
// Print WeatherIcon based on id
void printWeatherIcon(int id) {
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
  Serial.print ( "====" );
  Serial.print ( time );
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
  if(night){
    drawTheMoon();
  }else{
    drawTheSun();
  }
}

void drawFewClouds(){
  if(night){
    drawCloudAndTheMoon();
  }else{
    drawCloudWithSun();
  }
}

void drawTheSun(){
  tft.fillCircle(64,80,26,YELLOW);
}

void drawTheFullMoon(){
  tft.fillCircle(64,80,26,GREY);
}

void drawTheMoon(){
  tft.fillCircle(64,80,26,GREY);
  tft.fillCircle(75,73,26,BLACK);
}

void drawCloud(){
  tft.drawBitmap(0,35,cloud,128,90,GREY);
}

void drawThermometer(){
  tft.drawBitmap(0,40,thermometer,128,90,WHITE);
}

void drawUmidity(){
  tft.drawBitmap(0,40,umidity,128,90,BLUE);
}

void drawWifi(){
  tft.drawBitmap(0,20,wifi,128,90  ,BLUE);
}

void drawCloudWithSun(){
  tft.fillCircle(73,70,20,YELLOW);  
  tft.drawBitmap(0,36,cloud,128,90,BLACK);
  tft.drawBitmap(0,40,cloud,128,90,GREY);
}

void drawLightRainWithSunOrMoon(){
  if(night){  
    drawCloudTheMoonAndRain();
  }else{
    drawCloudSunAndRain();
  }
}

void drawLightRain(){
  tft.drawBitmap(0,35,cloud,128,90,GREY);
  tft.fillRoundRect(50, 105, 3, 13, 1, BLUE);
  tft.fillRoundRect(65, 105, 3, 13, 1, BLUE);
  tft.fillRoundRect(80, 105, 3, 13, 1, BLUE);
}

void drawModerateRain(){
  tft.drawBitmap(0,35,cloud,128,90,GREY);
  tft.fillRoundRect(50, 105, 3, 15, 1, BLUE);
  tft.fillRoundRect(57, 102, 3, 15, 1, BLUE);
  tft.fillRoundRect(65, 105, 3, 15, 1, BLUE);
  tft.fillRoundRect(72, 102, 3, 15, 1, BLUE);
  tft.fillRoundRect(80, 105, 3, 15, 1, BLUE);
}

void drawHeavyRain(){
  tft.drawBitmap(0,35,cloud,128,90,GREY);
  tft.fillRoundRect(43, 102, 3, 15, 1, BLUE);
  tft.fillRoundRect(50, 105, 3, 15, 1, BLUE);
  tft.fillRoundRect(57, 102, 3, 15, 1, BLUE);
  tft.fillRoundRect(65, 105, 3, 15, 1, BLUE);
  tft.fillRoundRect(72, 102, 3, 15, 1, BLUE);
  tft.fillRoundRect(80, 105, 3, 15, 1, BLUE);
  tft.fillRoundRect(87, 102, 3, 15, 1, BLUE);
}

void drawThunderstorm(){
  tft.drawBitmap(0,40,thunder,128,90,YELLOW);
  tft.drawBitmap(0,35,cloud,128,90,GREY);
  tft.fillRoundRect(48, 102, 3, 15, 1, BLUE);
  tft.fillRoundRect(55, 102, 3, 15, 1, BLUE);
  tft.fillRoundRect(74, 102, 3, 15, 1, BLUE);
  tft.fillRoundRect(82, 102, 3, 15, 1, BLUE);
}

void drawLightSnowfall(){
  tft.drawBitmap(0,30,cloud,128,90,GREY);
  tft.fillCircle(50, 100, 3, GREY);
  tft.fillCircle(65, 103, 3, GREY);
  tft.fillCircle(82, 100, 3, GREY);
}

void drawModerateSnowfall(){
  tft.drawBitmap(0,35,cloud,128,90,GREY);
  tft.fillCircle(50, 105, 3, GREY);
  tft.fillCircle(50, 115, 3, GREY);
  tft.fillCircle(65, 108, 3, GREY);
  tft.fillCircle(65, 118, 3, GREY);
  tft.fillCircle(82, 105, 3, GREY);
  tft.fillCircle(82, 115, 3, GREY);
}

void drawHeavySnowfall(){
  tft.drawBitmap(0,35,cloud,128,90,GREY);
  tft.fillCircle(40, 105, 3, GREY);
  tft.fillCircle(52, 105, 3, GREY);
  tft.fillCircle(52, 115, 3, GREY);
  tft.fillCircle(65, 108, 3, GREY);
  tft.fillCircle(65, 118, 3, GREY);
  tft.fillCircle(80, 105, 3, GREY);
  tft.fillCircle(80, 115, 3, GREY);
  tft.fillCircle(92, 105, 3, GREY);
}

void drawCloudSunAndRain(){
  tft.fillCircle(73,70,20,YELLOW);
  tft.drawBitmap(0,32,cloud,128,90,BLACK);
  tft.drawBitmap(0,35,cloud,128,90,GREY);
  tft.fillRoundRect(50, 105, 3, 13, 1, BLUE);
  tft.fillRoundRect(65, 105, 3, 13, 1, BLUE);
  tft.fillRoundRect(80, 105, 3, 13, 1, BLUE);
}

void drawCloudAndTheMoon(){
  tft.fillCircle(94,60,18,GREY);
  tft.fillCircle(105,53,18,BLACK);
  tft.drawBitmap(0,32,cloud,128,90,BLACK);
  tft.drawBitmap(0,35,cloud,128,90,GREY);
}

void drawCloudTheMoonAndRain(){
  tft.fillCircle(94,60,18,GREY);
  tft.fillCircle(105,53,18,BLACK);
  tft.drawBitmap(0,32,cloud,128,90,BLACK);
  tft.drawBitmap(0,35,cloud,128,90,GREY);
  tft.fillRoundRect(50, 105, 3, 11, 1, BLUE);
  tft.fillRoundRect(65, 105, 3, 11, 1, BLUE);
  tft.fillRoundRect(80, 105, 3, 11, 1, BLUE);
}

void drawWind(){  
  tft.drawBitmap(0,35,wind,128,90,GREY);   
}

void drawFog()  {
  tft.fillRoundRect(45, 60, 40, 4, 1, GREY);
  tft.fillRoundRect(40, 70, 50, 4, 1, GREY);
  tft.fillRoundRect(35, 80, 60, 4, 1, GREY);
  tft.fillRoundRect(40, 90, 50, 4, 1, GREY);
  tft.fillRoundRect(45, 100, 40, 4, 1, GREY);
}

void clearIcon(){
  tft.fillRect(0,40,128,100,BLACK);
}
