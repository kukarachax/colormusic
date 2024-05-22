
void animation() {
    if (Mode == 0) {
        count = 0;
        for (int i = (MAX_CH - 1); i > ((MAX_CH - 1) - Rlenght); i--) {
          leds[i] = ColorFromPalette(myPal, (count * indexCH));   // заливка по палитре " от зелёного к красному"
          count++;
        }
        count = 0;
        for (int i = (MAX_CH); i < (MAX_CH + Llenght); i++ ) {
          leds[i] = ColorFromPalette(myPal, (count * indexCH));   // заливка по палитре " от зелёного к красному"
          count++;
        }
        if (EmptyBright > 0) {
          CHSV this_dark = CHSV(EMPTY_COLOR, 255, EmptyBright);
          for (int i = ((MAX_CH - 1) - Rlenght); i > 0; i--)
            leds[i] = this_dark;
          for (int i = MAX_CH + Llenght; i < NUM_LEDS; i++)
            leds[i] = this_dark;
        }
    }
    else if (Mode == 1) {
        if (running_flag[2]) leds[STRIPE] = CHSV(HIGH_COLOR, 255, thisBright[2]);
        else if (running_flag[1]) leds[STRIPE] = CHSV(MID_COLOR, 255, thisBright[1]);
        else if (running_flag[0]) leds[STRIPE] = CHSV(LOW_COLOR, 255, thisBright[0]);
        else leds[STRIPE] = CHSV(EMPTY_COLOR, 255, EmptyBright);
        leds[STRIPE - 1] = leds[STRIPE];
        if (IntervalTimer(millis(), &running_timer, RunningFreqSpeed)) {
            for (int i = 0; i < STRIPE - 1; i++) {
              leds[i] = leds[i + 1];
              leds[NUM_LEDS - i - 1] = leds[i];
            }
        }
    }
    else if (Mode == 2) {   
        byte HUEindex = HUE_START;
        for (int i = 0; i < NUM_LEDS / 2; i++) {
            byte this_bright = map(freq_f[(int)floor((NUM_LEDS / 2 - i) / freq_to_stripe)], 0, freq_max_f, 0, 255);
            this_bright = constrain(this_bright, 0, 255);
            leds[i] = CHSV(HUEindex, 255, this_bright);
            leds[NUM_LEDS - i - 1] = leds[i];
            HUEindex += HUE_STEP;
            if (HUEindex > 255) HUEindex = 0;
        }
    }
    else if (Mode == 3) {
        for (int i = 0; i < NUM_LEDS; i++) {
            if (i < NUM_LEDS / 5)          leds[i] = CHSV(HIGH_COLOR, 255, thisBright[2]);
            else if (i < NUM_LEDS / 5 * 2) leds[i] = CHSV(MID_COLOR, 255, thisBright[1]);
            else if (i < NUM_LEDS / 5 * 3) leds[i] = CHSV(LOW_COLOR, 255, thisBright[0]);
            else if (i < NUM_LEDS / 5 * 4) leds[i] = CHSV(MID_COLOR, 255, thisBright[1]);
            else if (i < NUM_LEDS / 5 * 5) leds[i] = CHSV(HIGH_COLOR, 255, thisBright[2]);
        }
    }
    else if (Mode == 4) {
        fillStaticColor(Shue, Ssat, Bright);
    }
    else if (Mode == 5) {
        if (IntervalTimer(millis(), &rainbow_timer, 30)) {
            this_color += RainbowPeriod;
            if (this_color > 255) this_color = 0;
            if (this_color < 0) this_color = 255;
        }
        rainbow_steps = this_color;
        for (int i = 0; i < NUM_LEDS; i++) {
            delay(0);
            leds[i] = CHSV((int)floor(rainbow_steps), 255, 255);
            rainbow_steps += RainbowLoopStep;
            if (rainbow_steps > 255) rainbow_steps = 0;
            if (rainbow_steps < 0) rainbow_steps = 255;
        }
    }
    else if (Mode == 6) {
        if (IntervalTimer(millis(), &fade_timer, RainbowFadeDelay)) {
            ihue++;
            if (ihue > 255) ihue = 0;
            for (int idex = 0; idex < NUM_LEDS; idex++) {
                leds[idex] = CHSV(ihue, Fsat, 255);
            }
        }
    }
    else if (Mode == 7) {
          if (!IntervalTimer(millis(), &EmLightsLast, EmLightsDelay)) return;
          idexEM++;
          if (idexEM >= NUM_LEDS) {
            idexEM = 0;
          }
          int idexR = idexEM;
          int idexB = antipodal_index(idexR);
          int thathue = (thishue + 160) % 255;
          leds[idexR] = CHSV(thishue, thissat, 255);
          leds[idexB] = CHSV(thathue, thissat, 255);
    }
    else if (Mode == 8) {
        if (IntervalTimer(millis(), &strobe_timer, StrPeriod)) {
            if (strFlag) fillStaticColor(StrHue,StrSat,Bright);
            else fillStaticColor(StrHue,StrSat,0);
            strFlag = !strFlag;

        }
    }
    else if (Mode == 9) {
        if (!IntervalTimer(millis(), &pulseTimerLast, pulseDelay)) return;
        pulse_one_color_all();
    }
    else if (Mode == 10) {
        if (!IntervalTimer(millis(), &randomMarchLast, randomMarchDelay)) return;
        random_march();
    }

    if (Mode != 4 && Mode != 8) FastLED.show();
    
}

void fillStaticColor(byte hueT_, byte satT_, byte brght) {
    for (int i=0;i<NUM_LEDS;i++) {
        leds[i] = CHSV(hueT_, satT_, brght);
    }
    FastLED.show();
}

void strobeColor(byte hueT_, byte satT_, byte brght, int per, byte repeats) {
    for (byte i=0; i<repeats; i++) {
        fillStaticColor(hueT_, satT_, brght);
        delay(per);
        fillStaticColor(0,0,0);
        if (repeats >> 1) delay(per);
    }
}

void pulse_one_color_all() {
  if (bouncedirection == 0) {
    ibright++;
    if (ibright >= Bright) {
      bouncedirection = 1;
    }
  }
  if (bouncedirection == 1) {
    ibright = ibright - 1;
    if (ibright <= 1) {
      bouncedirection = 0;
    }
  }
  for (int idex = 0 ; idex < NUM_LEDS; idex++ ) {
    leds[idex] = CHSV(pulseHue, 255, ibright);
  }
}

void random_march() {
  copy_led_array();
  int iCCW;
  leds[0] = CHSV(random(0, 255), 255, 255);
  for (int idex = 1; idex < NUM_LEDS ; idex++ ) {
    iCCW = adjacent_ccw(idex);
    leds[idex].r = ledsX[iCCW][0];
    leds[idex].g = ledsX[iCCW][1];
    leds[idex].b = ledsX[iCCW][2];
  }
}

void copy_led_array() {
  for (int i = 0; i < NUM_LEDS; i++ ) {
    ledsX[i][0] = leds[i].r;
    ledsX[i][1] = leds[i].g;
    ledsX[i][2] = leds[i].b;
  }
}

int adjacent_ccw(int i) {
  int r;
  if (i > 0) {
    r = i - 1;
  }
  else {
    r = NUM_LEDS - 1;
  }
  return r;
}

int antipodal_index(int i) {
  int iN = i + TOP_INDEX;
  if (i >= TOP_INDEX) {
    iN = ( i + TOP_INDEX ) % NUM_LEDS;
  }
  return iN;
}

void FadeOff() {
    for (byte i = Bright; i>0; i-=5) {
        if (i % 5 > 0) i -= i % 5;
        FastLED.setBrightness(i);
        if (Mode == 0) volumeRead();
        else if (Mode == 1 || Mode == 2 || Mode == 3) fftRead();
        else animation();
        FastLED.show();
    }
}

void FadeOn() {
    for (byte i = 0; i<Bright; i+=5) {
        FastLED.setBrightness(i);
        if (Mode == 0) volumeRead();
        else if (Mode == 1 || Mode == 2 || Mode == 3) fftRead();
        else animation();
        FastLED.show();
    }
}

void BrightFade(byte newBright, bool dontAnimate) {
    if (!OnOff) return;
    if (Bright < newBright) {
        for (;Bright<newBright; Bright++) {
            FastLED.setBrightness(Bright);
            FastLED.show();
            if (!dontAnimate) {
                if (Mode == 0) volumeRead();
                else if (Mode == 1 || Mode == 2 || Mode == 3) fftRead();
                else animation();
            }
            
        }
    }
    else {
        for (;Bright>newBright; Bright--) {
            FastLED.setBrightness(Bright);
            FastLED.show();
            if (!dontAnimate) {
                if (Mode == 0) volumeRead();
                else if (Mode == 1 || Mode == 2 || Mode == 3) fftRead();
                else animation();
            }
        }
    }
}
