void runMaintMode() {
   static unsigned long prevMs = 0;
   const int intr = 1000;
   unsigned long curMs = millis();

   if (curMs - prevMs >= intr) {
       prevMs = curMs;

       SenData data;
       readSens(&data);
       
       //Serial.print(F(" Pres : ")); Serial.print(data.pres); Serial.println(F(" hPa"));
       Serial.print(F(" Temp : ")); Serial.print(data.temperature); Serial.println(F(" °C"));
       Serial.print(F(" Lumi : ")); Serial.print(data.light); Serial.println(F(" Lux"));
       Serial.print(F(" Hum : ")); Serial.print(data.humidity); Serial.println(F(" %"));
       lcd.setCursor(0, 1);
       lcd.print(F("Lumi : ")); lcd.print(data.light);lcd.print(F("Lux"));
   }
}