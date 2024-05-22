void EepromGetWiFiSettings() {
    uint8_t SSIDlength, PASSlength;

    EEPROM.get(100, SSIDlength);
    EEPROM.get(104, PASSlength);

    ssid = "";
    password = "";

    for (int i=0;i<SSIDlength;i++) {
      ssid += char(EEPROM.read(108+i));
    }

    for (int i=0;i<PASSlength;i++) {
      password += char(EEPROM.read(150+i));
    }
}

void EepromPutWiFiSettings(String inSSID, String inPASS) {
    EEPROM.put(100, inSSID.length());
    EEPROM.put(104, inPASS.length());

    for (int i=0;i<inSSID.length();i++) {
        EEPROM.put(108+i, inSSID[i]);
    }

    for (int i=0;i<inPASS.length();i++) {
        EEPROM.put(150+i, inPASS[i]);
    }

    EEPROM.commit();
}

String CutPASS(String passNew) {
    passNew.remove(0, passNew.indexOf(",")+1);
    return passNew;
}

String CutSSID(String ssidNew) {
    ssidNew.remove(ssidNew.indexOf(","), ssidNew.length());
    return ssidNew;
}

void eeprom_upd() {
  EEPROM.put(200,1);
  EEPROM.put(0, OnOff);
  EEPROM.put(4, Bright);
  EEPROM.put(8, EmptyBright);
  EEPROM.put(12, Mode);
  EEPROM.put(16, LOW_PASS);
  EEPROM.put(20, SMOOTH);
  EEPROM.put(24, RunningFreqSpeed);
  EEPROM.put(28, HUE_START);
  EEPROM.put(32, HUE_STEP);
  EEPROM.put(36, Shue);
  EEPROM.put(40, Ssat);
  EEPROM.put(44, RainbowPeriod);
  EEPROM.put(48, Fsat);
  EEPROM.put(52, RainbowFadeDelay);
  EEPROM.put(56, SPEKTR_LOW_PASS);
  EEPROM.put(58, SmoothFreq);
  EEPROM.put(68, MaxCoefFreq);
  EEPROM.put(78, RainbowLoopStep);
  EEPROM.put(88, LOW_PASS_ADD);
  EEPROM.put(92, LOW_PASS_FREQ_ADD);
  EEPROM.put(204, pulseDelay);
  EEPROM.put(208, pulseHue);
  EEPROM.put(212, randomMarchDelay);
  EEPROM.commit();
}

void eeprom_load() {
  EEPROM.get(0, OnOff);
  EEPROM.get(4, Bright);
  EEPROM.get(8, EmptyBright);
  EEPROM.get(12, Mode);
  EEPROM.get(16, LOW_PASS);
  EEPROM.get(20, SMOOTH);
  EEPROM.get(24, RunningFreqSpeed);
  EEPROM.get(28, HUE_START);
  EEPROM.get(32, HUE_STEP);
  EEPROM.get(36, Shue);
  EEPROM.get(40, Ssat);
  EEPROM.get(44, RainbowPeriod);
  EEPROM.get(48, Fsat);
  EEPROM.get(52, RainbowFadeDelay);
  EEPROM.get(56, SPEKTR_LOW_PASS);
  EEPROM.get(58, SmoothFreq);
  EEPROM.get(68, MaxCoefFreq);
  EEPROM.get(78, RainbowLoopStep);
  EEPROM.get(88, LOW_PASS_ADD);
  EEPROM.get(92, LOW_PASS_FREQ_ADD);
  EEPROM.get(204, pulseDelay);
  EEPROM.get(208, pulseHue);
  EEPROM.get(212, randomMarchDelay);
}