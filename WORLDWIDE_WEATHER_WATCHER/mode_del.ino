unsigned long prevMs = 0;
bool colSt = false;
unsigned long intr[] = {1000, 2000};
int intIdx = 0;

void errH() {
  unsigned long curMs = millis();
  if (curMs - prevMs >= intr[intIdx]) {
    prevMs = curMs;
    if (colSt) {
      ledR();
    } else {
      ledB();
    }
    colSt = !colSt;
  }
}

void errGPS() {
  unsigned long curMs = millis();
  if (curMs - prevMs >= intr[intIdx]) {
    prevMs = curMs;
    if (colSt) {
      ledR();
    } else {
      ledJ();
    }
    colSt = !colSt;
  }
}

void carteP() {
  unsigned long curMs = millis();
  if (curMs - prevMs >= intr[intIdx]) {
    prevMs = curMs;
    if (colSt) {
      ledR();
    } else {
      ledW();
    }
    colSt = !colSt;
  }
}

void errW() {
  unsigned long curMs = millis();
  if (curMs - prevMs >= intr[intIdx]) {
    prevMs = curMs;
    if (colSt) {
      ledR();
    } else {
      ledW();
    }
    colSt = !colSt;
    intIdx++;
    if (intIdx >= sizeof(intr) / sizeof(intr[0])) {
      intIdx = 0;
    }
  }
}

void errC() {
  unsigned long curMs = millis();
  if (curMs - prevMs >= intr[intIdx]) {
    prevMs = curMs;
    if (colSt) {
      ledR();
    } else {
      ledV();
    }
    colSt = !colSt;
  }
}

void dataI() {
  unsigned long curMs = millis();
  if (curMs - prevMs >= intr[intIdx]) {
    prevMs = curMs;
    if (colSt) {
      ledR();
    } else {
      ledV();
    }
    colSt = !colSt;
    intIdx++;
    if (intIdx >= sizeof(intr) / sizeof(intr[0])) {
      intIdx = 0;
    }
  }
}

void ledJ() {
  leds.setColorRGB(0, 255, 255, 0);
}

void ledR() {
  leds.setColorRGB(0, 255, 0, 0);
}

void ledW() {
  leds.setColorRGB(0, 255, 255, 255);
}

void ledV() {
  leds.setColorRGB(0, 0, 128, 0);
}

void ledB() {
  leds.setColorRGB(0, 0, 0, 255);
}

void ledO() {
  leds.setColorRGB(0, 255, 30, 0);
}