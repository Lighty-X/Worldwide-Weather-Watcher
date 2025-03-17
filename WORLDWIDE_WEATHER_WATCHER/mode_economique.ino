void runEconMode() {
    unsigned long curTime = millis();
    if (curTime - lastLog >= conf.logInt*2000) {
        logData();
        lastLog = curTime;
    }
}