/*
unsigned long UTCtimeLast;

bool getUTC() {
    if (IntervalTimer(millis(), &UTCtimeLast, (UPDATE_TIME * 1000))) {
        timeClient.update();
        nowDay = timeClient.getDay();
        nowHour = timeClient.getHours();
        nowMinute = timeClient.getMinutes();
        nowSecond = timeClient.getSeconds();
        return true;
    }
    else return false;
}
*/

void SleepTick() {
  if (sleepTime != 0) {
    if (TimeoutTimer(millis(), &sleepTimeLast, &sleepTime)) {
      OnOff = !OnOff;
      FadeOff();
    }
    else return;
  }
}

void startSleep() {
  sleepTimeLast = millis();
}

bool IntervalTimer(unsigned long uptime, unsigned long *TimeOutLast, long TimeOut) {
    if (uptime - *TimeOutLast > TimeOut) {
        *TimeOutLast = uptime;
        return true;
    }
    else return false;
}

bool TimeoutTimer(unsigned long uptime, unsigned long *TimeOutLast, long *TimeOut) {
    if (*TimeOut == 0) return false;

    if (uptime - *TimeOutLast > *TimeOut) {
        *TimeOut = 0;
        return true;
    }
    else return false;
}
/*
void WakeUpTick() {
    int wakeDay = nowDay; 
    int wakeHour = WakeUpHourDays[wakeDay];
    int wakeMinute = WakeUpMinuteDays[wakeDay];

    if (wakeMinute - START_WAKE_UP_MIN < 0) {
        wakeHour--;
        if (wakeHour < 0) {
            wakeDay--;
            if (wakeDay < 0) {
                wakeDay = 6;
            }
            wakeHour++;
        }
        wakeMinute = (wakeMinute + 60) - START_WAKE_UP_MIN;
    }
    
    if (IsWakeUpDay[wakeDay] && wakeHour == nowHour && wakeMinute == nowMinute) {
        OnOff = false;
        isWakeUpOn = true;
        wakeUpProcess(true, nowDay, nowMinute, nowHour);
    }
}

void wakeUpProcess(bool IsChange, int DayStart, int HourStart, int MinuteStart) {
    static int DayStarted, HourStarted, MinuteStarted;
    static unsigned long TimeLast, finishLastMillis;
    static int i = 0;

    if (IsChange) {
        DayStarted = DayStart;
        HourStarted = HourStart;
        MinuteStarted = MinuteStart;
        TimeLast = 0;
        finishLastMillis = 0;
        i = 0;
    }

    if (IntervalTimer(millis(), &TimeLast, (WAKE_UP_FADE_TIME * 60000) / (255 / 5)) && i != 255) {
        i+=5;
        leds[i] = CHSV(35, 190, i);
        FastLED.setBrightness(i);
        FastLED.show();
        if (i == 255) finishLastMillis = millis();
        return;
    }

    if (i == 255) {
        wakeUpFinish(finishLastMillis);
    }
}

void wakeUpFinish(unsigned long finishLastMillis) {
    long finishWakeUpTime = FINISH_WAKE_UP_MIN * 60000;

    if (TimeoutTimer(millis(), &finishLastMillis, &finishWakeUpTime)) {
        isWakeUpOn = false;
        BrightFade(0, true);
    }
}
*/