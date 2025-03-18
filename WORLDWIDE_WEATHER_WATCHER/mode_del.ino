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

void errH() {
  while (true) {  // Boucle infinie
    ledR();
    delay(1000);  // Attendre 1 seconde
    ledB();
    delay(1000);  // Attendre 1 seconde
  }
}
/*
void errGPS() {
  while (true) {  // Boucle infinie
    ledR();
    delay(1000);  // Attendre 1 seconde
    ledJ();
    delay(1000);  // Attendre 1 seconde
  }
}
*/
void carteP() {
while (true) {  // Boucle infinie
    ledR();
    delay(1000);  // Attendre 1 seconde
    ledW();
    delay(1000);  // Attendre 1 seconde
  }
}

void errC() {
  while (true) {  // Boucle infinie
    ledR();
    delay(1000);  // Attendre 1 seconde
    ledV();
    delay(1000);  // Attendre 1 seconde
  }
}

