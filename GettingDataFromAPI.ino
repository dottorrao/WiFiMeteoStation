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
    //weatherID = 801;
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

    String for1LOC = root["list"][0]["weather"][0]["main"];
    for1 = for1LOC;
    String for2LOC = root["list"][1]["weather"][0]["main"];
    for2 = for2LOC;
    String for3LOC = root["list"][2]["weather"][0]["main"];
    for3 = for3LOC;
    String for4LOC = root["list"][3]["weather"][0]["main"];
    for4 = for4LOC;
    String for5LOC = root["list"][4]["weather"][0]["main"];
    for5 = for5LOC;
    String for6LOC = root["list"][5]["weather"][0]["main"];
    for6 = for6LOC;
    String for7LOC = root["list"][6]["weather"][0]["main"];
    for7 = for7LOC;
    String for8LOC = root["list"][7]["weather"][0]["main"];
    for8 = for8LOC;

    String forDate1LOC = root["list"][0]["dt_txt"];
    forDate1 = forDate1LOC.substring(5,16);
    String forDate2LOC = root["list"][1]["dt_txt"];
    forDate2 = forDate2LOC.substring(5,16);
    String forDate3LOC = root["list"][2]["dt_txt"];
    forDate3 = forDate3LOC.substring(5,16);
    String forDate4LOC = root["list"][3]["dt_txt"];
    forDate4 = forDate4LOC.substring(5,16);
    String forDate5LOC = root["list"][4]["dt_txt"];
    forDate5 = forDate5LOC.substring(5,16);
    String forDate6LOC = root["list"][5]["dt_txt"];
    forDate6 = forDate6LOC.substring(5,16);
    String forDate7LOC = root["list"][6]["dt_txt"];
    forDate7 = forDate7LOC.substring(5,16);
    String forDate8LOC = root["list"][7]["dt_txt"];
    forDate8 = forDate8LOC.substring(5,16);
    
  } else {
    Serial.print ( "Error on geting forecast data: " + String(httpCode) );
  }
  ESP.wdtEnable(1000); //enabling watchdog
}
