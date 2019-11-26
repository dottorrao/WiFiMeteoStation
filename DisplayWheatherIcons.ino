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
  //tft.print(":");
  //tft.setCursor(2,20);
  //tft.setTextColor(GREEN);
  //tft.setTextSize(2);
  //tft.print(description);
  printWeatherIcon(weatherID);
  tft.setCursor(2,25);
  tft.setTextColor(GREEN);
  tft.print("Min:");
  tft.setTextColor(WHITE);
  tft.print(tempMin);
  tft.print("'C");
  tft.setCursor(2,35);
  tft.setTextSize(1);
  tft.setTextColor(GREEN);
  tft.print("Max:");
  tft.setTextColor(WHITE);
  tft.print(tempMax);
  tft.print("'C");
  tft.setCursor(2,45);
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
  tft.setCursor(80,128);
  tft.setTextSize(2);
  tft.setTextColor(BLUE);
  tft.print(umidity);
  tft.print("%");
  tft.setTextSize(1);
  //tft.setCursor(13,150);
  //tft.print(ip);
}

void printForecast(){
  Serial.println ("...Displaying Forecast Page on LCD..." );
  tft.fillScreen(BLACK);
  tft.setCursor(2,10);
  tft.setTextColor(RED);
  tft.setTextSize(1);
  tft.print("Montemurlo");
  //tft.print(":");
  tft.setTextColor(GREEN);
  tft.setCursor(2,30);
  tft.setTextSize(1);
  tft.print(forDate1 + ": " + for1);
  tft.setCursor(2,40);
  tft.print(forDate2 + ": " + for2);
  tft.setCursor(2,50);
  tft.print(forDate3 + ": " + for3);
  tft.setCursor(2,60);
  tft.print(forDate4 + ": " + for4);
  tft.setCursor(2,70);
  tft.print(forDate5 + ": " + for5);
  tft.setCursor(2,80);
  tft.print(forDate6 + ": " + for6);
  tft.setCursor(2,90);
  tft.print(forDate7 + ": " + for7);
  tft.setCursor(2,100);
  tft.print(forDate8 + ": " + for8);
}

void printDateTime(String day, String timeS, int x, int y){
  if ( (timeS != OLDtimeS) ){
    tft.setTextSize(2);
    //tft.setCursor(77,32);
    //tft.setTextColor(BLACK);
    //tft.print(day);
    //tft.setCursor(77,32);
    //tft.setTextColor(WHITE);
    //tft.print(day); 
    tft.setCursor(x,y);
    tft.setTextColor(BLACK);
    tft.print(OLDtimeS);
    tft.setCursor(x,y);
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
