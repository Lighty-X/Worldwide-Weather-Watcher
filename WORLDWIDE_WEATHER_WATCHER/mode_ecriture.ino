void readSens(SenData *data) {
    // Lecture de la température et de l'humidité depuis AHT20
    data->temperature = aht.getTemperature();
    data->humidity = aht.getHumidity();
    // Lecture de la pression depuis BMP280 (en hPa)
    data->pres = bmp.readPressure() / 100.0F;
    // Lecture de la luminosité depuis un capteur analogique
    data->light = analogRead(LIGHT_SENSOR_PIN);
}

void logData() {
    if (!logFile) {
        String fname = "data" + String(fileRev) + ".csv";
        logFile = SD.open(fname.c_str(), FILE_WRITE);
        if (!logFile) {
            return;
        }
        logFile.println(F("Date,Time,Temp,Pres,Light,Humidity,Lat,Lon"));
    }

    if (logFile.size() >= conf.fileMax) {
        logFile.close();
        fileRev++;
        String newFname = "data" + String(fileRev) + ".csv";
        logFile = SD.open(newFname.c_str(), FILE_WRITE);
        if (!logFile) {
            return;
        }
        logFile.println(F("Date,Time,Temp,Pres,Light,Humidity,Lat,Lon"));
    }

    SenData data;
    readSens(&data);
    readTime();  // Populate the global variables with the current time

    logFile.print(yr);
    logFile.print('/');
    logFile.print(mon);
    logFile.print('/');
    logFile.print(dom);
    logFile.print(',');
    logFile.print(hr);
    logFile.print(':');
    logFile.print(mins);
    logFile.print(':');
    logFile.print(sec);
    logFile.print(',');

    logFile.print(data.temperature);
    logFile.print(',');
    logFile.print(data.pres);
    logFile.print(',');
    logFile.print(data.light);
    logFile.print(',');
    logFile.print(data.humidity);  // Ajouter l'humidité
    logFile.print(',');

    // Enregistrement des coordonnées GPS si disponibles
    if (isnan(data.lat)) {
        logFile.print("nan");
    } else {
        logFile.print(data.lat, 6);
    }
    logFile.print(',');

    if (isnan(data.lon)) {
        logFile.print("nan");
    } else {
        logFile.print(data.lon, 6);
    }
    logFile.println();

    logFile.flush();
}