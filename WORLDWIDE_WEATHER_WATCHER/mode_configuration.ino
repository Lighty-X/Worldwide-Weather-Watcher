void saveConf() {
    EEPROM.put(0, conf);
}

void loadConf() {
    EEPROM.get(0, conf);
}

void resetConf() {
    conf = defConf;
    saveConf();
    printConf();
}

void printConf() {
    Serial.println(F("Conf"));
    Serial.print(F("LOG_INTERVAL: ")); Serial.println(conf.logInt);
    Serial.print(F("FILE_MAX: ")); Serial.println(conf.fileMax);
    Serial.print(F("TIMEOUT: ")); Serial.println(conf.timeout);
    Serial.print(F("LUMIN: ")); Serial.println(conf.lumin);
    Serial.print(F("LUM_LOW: ")); Serial.println(conf.lumLow);
    Serial.print(F("LUM_HI: ")); Serial.println(conf.lumHigh);
    Serial.print(F("TEMP_AIR: ")); Serial.println(conf.tempAir);
    Serial.print(F("MIN_TEMP: ")); Serial.println(conf.minTemp);
    Serial.print(F("MAX_TEMP: ")); Serial.println(conf.maxTemp);
    Serial.print(F("HYGR: ")); Serial.println(conf.hygr);
    Serial.print(F("HYGR_MIN: ")); Serial.println(conf.hygrMin);
    Serial.print(F("HYGR_MAX: ")); Serial.println(conf.hygrMax);
    Serial.print(F("PRESSURE: ")); Serial.println(conf.pres);
    Serial.print(F("PRES_MIN: ")); Serial.println(conf.presMin);
    Serial.print(F("PRES_MAX: ")); Serial.println(conf.presMax);
}

void setClock(const char *timeStr) {
  if (sscanf(timeStr, "%d:%d:%d", &hr, &mins, &sec) == 3) {
    if (hr >= 0 && hr <= 23 && mins >= 0 && mins <= 59 && sec >= 0 && sec <= 59) {
        readTime();
        setTime(sec, mins, hr, dow, dom, mon, yr);
        printTime();
    }
  }
}

void setDate(const char *dateStr) {
  int y, m, d;
  if (sscanf(dateStr, "%d,%d,%d", &m, &d, &y) == 3) {
      if (y >= 2000 && y <= 2099 && m >= 1 && m <= 12 && d >= 1 && d <= 31) {
            readTime();
            int yrAdj = y - (14 - m) / 12;
            int mAdj = m + 12 * ((14 - m) / 12) - 2;
            int dow = (d + yrAdj + yrAdj/4 - yrAdj/100 + yrAdj/400 + (31*mAdj)/12) % 7;
            if (dow == 0) dow = 7;
            setTime(sec, mins, hr, dow, d, m, y);
            printTime();
        }
  }
}

void setDay(const char *dayStr) {
    const char *days[] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
    char upDayStr[4];
    for (int i = 0; i < 3 && dayStr[i] != '\0'; i++) {
        upDayStr[i] = toupper(dayStr[i]);
        upDayStr[i+1] = '\0';
    }
    for (int i = 0; i < 7; i++) {
        if (strcmp(upDayStr, days[i]) == 0) {
            readTime();
            setTime(sec, mins, hr, i + 1, dom, mon, yr);
            printTime();
            return;
        }
    }
}

void procCmd(char* key, char* val) {
    if (strcmp(key, "LOG_INTERVAL") == 0) conf.logInt = atoi(val);
    else if (strcmp(key, "FILE_MAX") == 0) conf.fileMax = atoi(val);
    else if (strcmp(key, "TIMEOUT") == 0) conf.timeout = atoi(val);
    else if (strcmp(key, "LUMIN") == 0) conf.lumin = atoi(val);
    else if (strcmp(key, "LUM_LOW") == 0) conf.lumLow = atoi(val);
    else if (strcmp(key, "LUM_HI") == 0) conf.lumHigh = atoi(val);
    else if (strcmp(key, "TEMP_AIR") == 0) conf.tempAir = atoi(val);
    else if (strcmp(key, "MIN_TEMP") == 0) conf.minTemp = atoi(val);
    else if (strcmp(key, "MAX_TEMP") == 0) conf.maxTemp = atoi(val);
    else if (strcmp(key, "HYGR") == 0) conf.hygr = atoi(val);
    else if (strcmp(key, "HYGR_MIN") == 0) conf.hygrMin = atoi(val);
    else if (strcmp(key, "HYGR_MAX") == 0) conf.hygrMax = atoi(val);
    else if (strcmp(key, "PRESSURE") == 0) conf.pres = atoi(val);
    else if (strcmp(key, "PRES_MIN") == 0) conf.presMin = atoi(val);
    else if (strcmp(key, "PRES_MAX") == 0) conf.presMax = atoi(val);
    saveConf();
    printConf();
}

void runConfMode() {
     if (Serial.available()) {
        char inBuf[64];
        byte bytesRead = Serial.readBytesUntil('\n', inBuf, sizeof(inBuf) - 1);
        inBuf[bytesRead] = '\0';

        char *start = inBuf;
        while (*start && isspace(*start)) start++;
        char *end = start + strlen(start) - 1;
        while (end > start && isspace(*end)) end--;
        *(end + 1) = '\0';

        if (strcmp(start, "RESET") == 0) {
            resetConf();
            lastAct = millis(); // Mettre à jour lastAct
            return;
        }
        if (strcmp(start, "VERSION") == 0) {
            Serial.println(F("#### Worldwide Weather Watcher - Pau Gab Imd Bas ####"));
            Serial.println(F("Version: 47.3.21 "));
            lastAct = millis(); // Mettre à jour lastAct
            return;
        }
        if (strcmp(start, "GETCLOCK") == 0) {
            printTime();
            lastAct = millis(); // Mettre à jour lastAct
            return;
        }

        if (strncmp(start, "CLOCK", 5) == 0) {
            setClock(start + 6);
            lastAct = millis(); // Mettre à jour lastAct
            return;
        }
        if (strncmp(start, "DATE", 4) == 0) {
            setDate(start + 5);
            lastAct = millis(); // Mettre à jour lastAct
            return;
        }
        if (strncmp(start, "DAY", 3) == 0) {
            setDay(start + 4);
            lastAct = millis(); // Mettre à jour lastAct
            return;
        }

        char *sep = strchr(start, '=');
        if (sep != NULL) {
            *sep = '\0';
            char *key = start;
            char *val = sep + 1;

            char *key_end = key + strlen(key) - 1;
            while (key_end > key && isspace(*key_end)) key_end--;
            *(key_end + 1) = '\0';

            while (*val && isspace(*val)) val++;

            procCmd(key, val);
            lastAct = millis(); // Mettre à jour lastAct
        }
    }
    // Vérification du timeout (en dehors de la gestion de l'entrée série)
    if (millis() - lastAct >= conf.timeout*1000) {
        stdMode(); // Retour au mode standard
    }
}