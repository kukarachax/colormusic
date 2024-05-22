void fftRead() {
    colorMusic[0] = 0;
    colorMusic[1] = 0;
    colorMusic[2] = 0;

    spectrFFT();

    if (DEBUG == true) {
        for (byte i=0;i<FFT_SIZE;i++) {
            Serial.print(int(spectr[i]));
        }
        Serial.println();
    }
    for (int i = 0 ; i < 32 ; i++) {
      if (spectr[i] < SPEKTR_LOW_PASS) spectr[i] = 0;
    }

    for (byte i = 2; i < 6; i++) {
      if (spectr[i] > colorMusic[0]) colorMusic[0] = spectr[i];
    }

    for (byte i = 6; i < 11; i++) {
      if (spectr[i] > colorMusic[1]) colorMusic[1] = spectr[i];
    }

    for (byte i = 11; i < 32; i++) {
      if (spectr[i] > colorMusic[2]) colorMusic[2] = spectr[i];
    }
    freq_max = 0;
    for (byte i = 0; i < 30; i++) {
      if (spectr[i + 2] > freq_max) freq_max = spectr[i + 2];
      if (freq_max < 5) freq_max = 5;
      if (freq_f[i] < spectr[i + 2]) freq_f[i] = spectr[i + 2];
      if (freq_f[i] > 0) freq_f[i] -= LIGHT_SMOOTH;
      else freq_f[i] = 0;
    }
    freq_max_f = freq_max * averK + freq_max_f * (1 - averK);
    for (byte i = 0; i < 3; i++) {
      colorMusic_aver[i] = colorMusic[i] * averK + colorMusic_aver[i] * (1 - averK); 
      colorMusic_f[i] = colorMusic[i] * SmoothFreq + colorMusic_f[i] * (1 - SmoothFreq); 
      if (colorMusic_f[i] > ((float)colorMusic_aver[i] * MaxCoefFreq)) {
        thisBright[i] = 255;
        colorMusicFlash[i] = true;
        running_flag[i] = true;
      } else colorMusicFlash[i] = false;
      if (thisBright[i] >= 0) thisBright[i] -= SMOOTH_STEP;
      if (thisBright[i] < EmptyBright) {
        thisBright[i] = EmptyBright;
        running_flag[i] = false;
      }
    }
    animation();
}

void volumeRead() {
  RsoundLevel = 0;
  LsoundLevel = 0;
  
  for (byte i = 0; i < 100; i ++) {                              
    RcurrentLevel = analogRead(SOUND_R);                         
    if (RsoundLevel < RcurrentLevel) RsoundLevel = RcurrentLevel;
  }

  RsoundLevel = map(RsoundLevel, LOW_PASS, 1023, 0, 500);
  RsoundLevel = constrain(RsoundLevel, 0, 500);
  RsoundLevel = pow(RsoundLevel, EXP);
  RsoundLevel_f = RsoundLevel * SMOOTH + RsoundLevel_f * (1 - SMOOTH);
  LsoundLevel_f = RsoundLevel_f;

  if (EmptyBright > 5) {
    for (int i = 0; i < NUM_LEDS; i++)
      leds[i] = CHSV(EMPTY_COLOR, 255, EmptyBright);
  }

  if (RsoundLevel_f > 15 && LsoundLevel_f > 15) {

    averageLevel = (float)(RsoundLevel_f + LsoundLevel_f) / 2 * averK + averageLevel * (1 - averK);

    maxLevel = (float)averageLevel * MAX_COEF;

    Rlenght = map(RsoundLevel_f, 0, maxLevel, 0, MAX_CH);
    Llenght = map(LsoundLevel_f, 0, maxLevel, 0, MAX_CH);

    Rlenght = constrain(Rlenght, 0, MAX_CH);
    Llenght = constrain(Llenght, 0, MAX_CH);

    animation();
  }
}

void lowPass() {
  fillStaticColor(HUE_GREEN+10, 255, 50); 

  int thisMax = 0;                          
  int thisLevel;
  for (byte i = 0; i < 200; i++) {
    thisLevel = analogRead(SOUND_R);        
    if (thisLevel > thisMax)                
      thisMax = thisLevel;                  
    delay(4);                               
  }
  LOW_PASS = thisMax + LOW_PASS_ADD;        

  thisMax = 0;
  spectrFFT();
  for (byte i = 0; i < 100; i++) {          
    for (byte j = 2; j < 32; j++) {         
      thisLevel = spectr[j];
      if (thisLevel > thisMax)              
        thisMax = thisLevel;                
    }
    delay(4);                               
  }
  SPEKTR_LOW_PASS = thisMax + LOW_PASS_FREQ_ADD;  
  eeprom_upd();

}

void spectrFFT() {
  for (int i = 0; i < FFT_SIZE; i++) raw[i] = analogRead(SOUND_R_FREQ);
  FFT(raw, spectr);
}

void lowpassCunstruct() {
  LOW_PASS += LOW_PASS_ADD;   
  SPEKTR_LOW_PASS += LOW_PASS_FREQ_ADD; 
}

