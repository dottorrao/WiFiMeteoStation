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
