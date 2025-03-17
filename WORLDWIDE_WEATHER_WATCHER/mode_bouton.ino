void loop() {
  if (btnPressed) {
    while (digitalRead(btnR) == LOW || digitalRead(btnG) == LOW);
    unsigned long relTime = millis();
    unsigned long pressDur = relTime - pressTime;
    btnPressed = false;

    switch (curMode) {
      case STD:
        if (btnID == 1 && pressDur >= 3000) {
          prevMode = curMode;  // Mémorise le mode actuel avant de changer
          curMode = MAINT;
          modeStart = millis();
          maintMode(); // Appel unique pour l'initialisation
        } else if (btnID == 2 && pressDur >= 3000) {
          prevMode = curMode;  // Mémorise le mode actuel avant de changer
          curMode = ECON;
          modeStart = millis();
          econMode(); // Appel unique pour l'initialisation
          
        }
        break;

      case CONF:
        if (millis() - modeStart >= conf.timeout*1000) {
          curMode = STD;
          modeStart = millis();
          stdMode(); // Appel unique pour l'initialisation
        }
        break;

      case MAINT:
        if (btnID == 1 && pressDur >= 3000) {
          curMode = prevMode;
          modeStart = millis();
          if (curMode == STD) {
            stdMode(); // Appel unique pour l'initialisation
          } else if (curMode == ECON) {
            econMode(); // Appel unique pour l'initialisation
          }
        }
        break;

      case ECON:
        if (btnID == 2 && pressDur >= 3000) {
          curMode = STD;
          modeStart = millis();
          stdMode(); // Appel unique pour l'initialisation
        }
        break;
    }
    btnID = 0;
  }

  // Gestion des actions continues des modes (Appel des fonctions dédiées)
  switch (curMode) {
    case STD:
      runStdMode();  // Exécution continue du mode standard
      break;

    case CONF:
      runConfMode(); // Exécution continue du mode configuration
      break;

    case MAINT:
      runMaintMode(); // Exécution continue du mode maintenance
      break;

    case ECON:
      runEconMode();  // Exécution continue du mode économique
      break;
  }

  delay(10); // Important: petit délai pour la stabilité
}

//Fonctions des modes (initialisation, appelées une seule fois)

void stdMode() {
    lcd.backlight();
    lcd.clear();
    lcd.print("STANDARD");
    ledV();
    runStdMode();
}

void confMode() {
  lcd.backlight();
  lcd.clear();
  lcd.print("CONFIG");
  ledJ();
  lastAct = millis(); // Initialisation pour le timeout
  loadConf();  // Recharge la configuration (au cas où elle aurait été modifiée)
  printConf(); // Affiche la configuration
  printTime(); // Affiche l'heure
  runConfMode();
}

void maintMode() {
  
  lcd.backlight();
  lcd.clear();
  lcd.print("MAINT");
  ledO();
  runMaintMode();
  // Ne pas définir curMode ici
}

void econMode() {
  runEconMode();
  lcd.backlight();
  lcd.clear();
  lcd.print("ECO");
  ledB();
  runEconMode();
  // Ne pas définir curMode ici
}

void checkStartMode() {
  if (digitalRead(btnR) == LOW) {
    curMode = CONF;
    modeStart = millis();
    confMode(); // Appel unique
    lastAct = millis(); // Initialise le timeout
  } else {
    curMode = STD;
    modeStart = millis();
    stdMode(); // Appel unique
  }
}