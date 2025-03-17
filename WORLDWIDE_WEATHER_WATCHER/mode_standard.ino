void runStdMode() {
    unsigned long curTime = millis();
    if (curTime - lastLog >= conf.logInt*1000) {
        logData();
        lastLog = curTime;
    }
}