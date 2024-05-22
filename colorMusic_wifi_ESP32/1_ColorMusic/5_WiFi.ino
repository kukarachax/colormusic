/*----------------------------------------INIT----------------------------------------*/
void wifiConnect() {
    byte trys = 0;

    fillStaticColor(HUE_YELLOW, 255, 255);

    EepromGetWiFiSettings();

    Serial.println(ssid);
    Serial.println(password);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        trys++;
        delay(500);
        if (trys == 15) { 
            strobeColor(HUE_RED,255,255,450,3); 
            Serial.println("Не удалось подключиться.\n\rПереход в режим точки доступа.");
            APactivate();
        }
    }

    strobeColor(HUE_GREEN, 255, 255, 450, 3);

    Serial.print("Инициализация и подключение к WiFi выполнены успешно.\n\rЗайдите в приложение 'Colormusic WiFi',\n\rДалее в 'Сервис' -> 'Ключ' и введите в это поле это число: ");
    Serial.println(WiFi.localIP());

    server.begin();
    //timeClient.begin();
    otaInit();
}

void otaInit() {
    ArduinoOTA.setHostname("Colormusic-ESP32");
    ArduinoOTA.onStart([]() {Serial.println("Start");} );
    ArduinoOTA.onEnd([]() {Serial.println("\nEnd"); ESP.restart();} );
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {Serial.printf("Progress: %u%%\r", (progress / (total / 100)));} );
    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
    ArduinoOTA.begin();
}


/*----------------------------------------WIFI_LISTEN----------------------------------------*/
void WiFiRead() {
    bool fl=true, flGet=false;

    WiFiClient client = server.available();
    if (!client) return;
    while (!client.available()) {}

    String req = client.readStringUntil('\r');
    if (req.indexOf("/LedStrip/OnOff;") != -1) { 
        OnOff = cutString(req);
        isWakeUpOn = false;
        if (OnOff) FadeOn();
        else FadeOff();
    }
    else if (req.indexOf("/LedStrip/Mode;") != -1) {Mode = cutString(req); FastLED.clear(); FastLED.show(); }
    /*
    else if (req.indexOf("/LedStrip/Bright;") != -1) BrightFade(cutString(req));
    else if (req.indexOf("/LedStrip/EmptyBright;") != -1) EmptyBright = cutString(req);
    else if (req.indexOf("/LedStrip/RunningFreqSpeed;") != -1) RunningFreqSpeed = cutString(req);
    else if (req.indexOf("/LedStrip/MaxCoefFreq;") != -1) MaxCoefFreq = cutStringFloat(req);
    else if (req.indexOf("/LedStrip/HUE_START;") != -1) HUE_START = cutString(req);
    else if (req.indexOf("/LedStrip/HUE_STEP;") != -1) HUE_STEP = cutString(req);
    else if (req.indexOf("/LedStrip/SmoothFreq;") != -1) SmoothFreq = cutStringFloat(req);
    else if (req.indexOf("/LedStrip/Shue;") != -1) Shue = cutString(req);
    else if (req.indexOf("/LedStrip/Ssat;") != -1) Ssat = cutString(req);
    else if (req.indexOf("/LedStrip/RainbowPeriod;") != -1) RainbowPeriod = cutString(req);
    else if (req.indexOf("/LedStrip/RainbowLoopStep;") != -1) RainbowLoopStep = cutStringFloat(req);
    else if (req.indexOf("/LedStrip/Fsat;") != -1) Fsat = cutString(req);
    else if (req.indexOf("/LedStrip/EmLightsDelay;") != -1) EmLightsDelay = cutString(req);
    else if (req.indexOf("/LedStrip/StrPeriod;") != -1) StrPeriod = cutString(req);
    else if (req.indexOf("/LedStrip/StrHue;") != -1) StrHue = cutString(req);
    else if (req.indexOf("/LedStrip/StrSat;") != -1) StrSat = cutString(req);
    else if (req.indexOf("/LedStrip/SMOOTH;") != -1) SMOOTH = cutStringFloat(req);
    else if (req.indexOf("/LedStrip/RainbowFadeDelay;") != -1) RainbowFadeDelay = cutString(req);
    */
    else if (req.indexOf("/LedStrip/LOW_PASS_ADD;") != -1) { 
        LOW_PASS_ADD = cutString(req); 
        lowpassCunstruct(); 
    }
    else if (req.indexOf("/LedStrip/LOW_PASS_FREQ_ADD;") != -1) { 
        LOW_PASS_FREQ_ADD = cutString(req); 
        lowpassCunstruct(); 
    }
    else if (req.indexOf("/LedStrip/SleepTime;") != -1) { 
        sleepTime = cutString(req) * 60000; 
        startSleep(); 
    }
    else if (req.indexOf("/LedStrip/SleepStop") != -1) { 
        sleepTime = 0; 
    }
    else if (req.indexOf("/LedStrip/Restart") != -1) {
        ESP.restart();
    }
    else if (req.indexOf("/LedStrip/lowpass") != -1) {
        lowPass();
    }
    else if (req.indexOf("/LedStrip/AllData@") != -1) {
        descryptData(req);
    }
    /*
    else if (req.indexOf("/LedStrip/WakeUp@") != -1) {
        descryptWakeUp(req);
    }
    */
    else if (req.indexOf("/LedStrip/GetDataStruct") != -1) { 
        flGet = true; 
        client.println("HTTP/1.1 200 OK"); 
        client.println("Content-Type: text/html");
        client.println();
        client.println(StructDebug()); 
    }
    else if (req.indexOf("/LedStrip/GetData") != -1) { 
        flGet = true; 
        client.println("HTTP/1.1 200 OK"); 
        client.println("Content-Type: text/html");
        client.println();
        client.println(GetData()); 
    }
    /*
    else if (req.indexOf("/LedStrip/WakeUpOff") != -1) {
        isWakeUpOn = false;
        BrightFade(0, true);
    }
    else if (req.indexOf("/LedStrip/GetTimeAndWU") != -1) {
        flGet = true;
        client.println("HTTP/1.1 200 OK"); 
        client.println("Content-Type: text/html");
        client.println();
        client.println(timeClient.getFormattedTime());
        client.println();
        client.println("isWakeUpOn: " + String(isWakeUpOn));
        client.println("IsWakeUpDay: " + String(IsWakeUpDay[nowDay]));
        client.println("WakeUpHourDays: " + String(WakeUpHourDays[nowDay]));
        client.println("WakeUpMinuteDays: " + String(WakeUpMinuteDays[nowDay]));
    }
    */
    else fl = false;


    if (!flGet) {
        if (fl == true) {
            client.println("HTTP/1.1 200 OK"); 
            client.println("Content-Type: text/html");
            client.println();
            client.print("OK!");
            client.println();
            eeprom_upd();
        }
        else {
            client.println("HTTP/1.1 200 OK"); 
            client.println("Content-Type: text/html");
            client.println();
            client.print("Error!");
            client.println();
        }
    }
    client.stop();
}


/*----------------------------------------REQUEST_CUT----------------------------------------*/
int cutString(String inAdd) {
  inAdd.remove(0, inAdd.indexOf(";")+1);
  inAdd.remove(inAdd.indexOf("HTTP/1.1"), inAdd.length());
  int result = inAdd.toInt();
  return result;
}

float cutStringFloat(String inAdd) {
    inAdd.remove(0, inAdd.indexOf(";")+1);
    inAdd.remove(inAdd.indexOf("HTTP/1.1"), inAdd.length());
    float result = inAdd.toFloat();
    return result;
}

/*----------------------------------------AP_WIFI----------------------------------------*/
const char* PARAM_INPUT_1 = "input1";
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html lang='ru-RU'>
    <head>
        <title>Подключить к WIFI</title>
        <meta name="viewport" content="width=device-width, initial-scale=1, charset='UTF-8'">
    </head>

    <body>
        <h1>Цветомузыка</h1>
        <p>Пример: [SSID],[Password]</p>
        <p></p>
        <form action="/get">
        <input type="text" name="input1">
        <input type="submit" value="Submit">
        </form><br>
    </body>
</html>)rawliteral";

void APactivate() {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssidAP, passwordAP);
    IPAddress IP = WiFi.softAPIP();
    Serial.println(WiFi.softAPIP());

    Aserver.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", index_html);
    });

    Aserver.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
        String inputRAM;
        String inputSSID;
        String inputPASS;

        if (request->hasParam(PARAM_INPUT_1)) {
            inputRAM = request->getParam(PARAM_INPUT_1)->value();

            inputSSID = CutSSID(inputRAM);
            inputPASS = CutPASS(inputRAM);

            EepromPutWiFiSettings(inputSSID, inputPASS);
            request->send(200, "text/html", "<h1>OK</h1>");
            ESP.restart();
            return;
        }
    });
    Aserver.begin();
}

/*----------------------------------------GET_RESPONSE----------------------------------------*/
String StructDebug() {
  String str;
  str += "<!DOCTYPE HTML>";
  str += "<html lang='ru-RU'><head><meta charset='UTF-8'/>\n\r";
  str += "</head>\n\r<body>\n\r<h1>Цветомузыка WEB</h1>";
  str += "<br/>----------------------------------------------------";
  str += "<br/>OnOff [Вкл/Выкл]: "; str += OnOff;
  str += "<br/>Bright [Яркость]: "; str += Bright;
  str += "<br/>EmptyBright [Яркость пустых]: "; str += EmptyBright;
  str += "<br/>Mode [Режим]: "; str += Mode;
  str += "<br/>--------------------------";
  str += "<br/>Mode 0 [Громкость]:";
  str += "<br/>LOW_PASS [Нижний порог]: "; str += LOW_PASS;
  str += "<br/>SMOOTH [Плавность анимации]: "; str += SMOOTH;
  str += "<br/>--------------------------";
  str += "<br/>Mode 1 [Бегущие частоты]:";
  str += "<br/>RunningFreqSpeed [Скорость]: "; str += RunningFreqSpeed;
  str += "<br/>MaxCoefFreq [Порог]: "; str += MaxCoefFreq;
  str += "<br/>--------------------------";
  str += "<br/>Mode 2 [Спектр]:"; 
  str += "<br/>HUE_START [Цветовая палитра]: "; str += HUE_START;
  str += "<br/>HUE_STEP [Шаг цвета]: "; str += HUE_STEP;
  str += "<br/>--------------------------";
  str += "<br/>Mode 3 [5 Частот]:";
  str += "<br/>MaxCoefFreq [Порог вспышки]: "; str += MaxCoefFreq;
  str += "<br/>SmoothFreq [Плавность]: "; str += SmoothFreq;
  str += "<br/>--------------------------";
  str += "<br/>Mode 4 [Цвет (0-255)]:"; 
  str += "<br/>Shue [HUE]: "; str += Shue;
  str += "<br/>Ssat [SAT]: "; str += Ssat;
  str += "<br/>--------------------------";
  str += "<br/>Mode 5 [Бегущая радуга]: ";
  str += "<br/>RainbowPeriod [Скорость]: "; str += RainbowPeriod;
  str += "<br/>RainbowLoopStep [Шаг радуги]: "; str += RainbowLoopStep;
  str += "<br/>--------------------------";
  str += "<br/>Mode 6 [Смена цвета]: ";
  str += "<br/>Fsat [Насыщенность]: "; str += Fsat;
  str += "<br/>RainbowFadeDelay [Скорость]: "; str += RainbowFadeDelay;
  str += "<br/>----------------------------------------------------";
  str += "</body> \n\r</html>";
  return str;
}

String GetData() {
    String str;
    str += OnOff; str += " ";
    str += Mode; str += " ";
    str += Bright; str += " ";
    str += EmptyBright; str += " ";
    str += RunningFreqSpeed; str += " ";
    str += MaxCoefFreq; str += " ";
    str += HUE_START; str += " ";
    str += HUE_STEP; str += " ";
    str += SmoothFreq; str += " ";
    str += Shue; str += " ";
    str += Ssat; str += " ";
    str += RainbowPeriod; str += " ";
    str += RainbowLoopStep; str += " ";
    str += Fsat; str += " ";
    str += EmLightsDelay; str += " ";
    str += StrPeriod; str += " ";
    str += StrHue; str += " ";
    str += StrSat; str += " ";
    str += RainbowFadeDelay; str += " ";
    str += LOW_PASS_ADD; str += " ";
    str += LOW_PASS_FREQ_ADD; str += " ";
    str += LOW_PASS; str += " ";
    str += SPEKTR_LOW_PASS; str += " ";
    str += SMOOTH; str += " ";
    if (sleepTime > 0 and sleepTime > 60000) str += int(round((sleepTime - (millis() - sleepTimeLast)) / 60000));
    else if ((sleepTime - (millis() - sleepTimeLast)) < 60000 and sleepTime > 0) str += "-1";
    else str += "0";
    str += " "; 
    str += pulseDelay; str += " ";
    str += pulseHue; str += " ";
    str += randomMarchDelay;
    return str;
}

/*----------------------------------------REQUEST_DESCR----------------------------------------*/
void descryptData(String data) {
    data.remove(0, data.indexOf("@")+1);
    data.remove(data.indexOf("HTTP/1.1"), data.length());
    int i=0;
    bool OnOffLast = OnOff;
    while (true) {
        int position = 0;
        position = data.indexOf(';');
        if (position == -1) position = data.indexOf('&');
        if (position == -1) return;
        String strSub = data.substring(0, position);
        data.remove(0, position+1);

        switch (i) {
            case 0: 
                OnOff = StrToBool(strSub);
                isWakeUpOn = false;
                if (OnOffLast != OnOff) {
                    if (OnOff) FadeOn();
                    else FadeOff();
                }
            break;
            case 1: Mode = strSub.toInt();
            break;
            case 2: 
                if (!OnOff) BrightFade(strSub.toInt(), true);
                else BrightFade(strSub.toInt());
            break;
            case 3: EmptyBright = strSub.toInt();
            break;
            case 4: RunningFreqSpeed = strSub.toInt();
            break;
            case 5: MaxCoefFreq = strSub.toFloat();
            break;
            case 6: HUE_START = strSub.toInt();
            break;
            case 7: HUE_STEP = strSub.toInt();
            break;
            case 8: SmoothFreq = strSub.toFloat();
            break;
            case 9: Shue = strSub.toInt();
            break;
            case 10: Ssat = strSub.toInt();
            break;
            case 11: RainbowPeriod = strSub.toInt();
            break;
            case 12: RainbowLoopStep = strSub.toFloat();
            break;
            case 13: Fsat = strSub.toInt();
            break;
            case 14: EmLightsDelay = strSub.toInt();
            break;
            case 15: StrPeriod = strSub.toInt();
            break;
            case 16: StrHue = strSub.toInt();
            break;
            case 17: StrSat = strSub.toInt();
            break;
            case 18: RainbowFadeDelay = strSub.toInt();
            break;
            case 19: SMOOTH = strSub.toFloat();
            break;
            case 20: pulseDelay = strSub.toInt();
            break;
            case 21: pulseHue = strSub.toInt();
            break;
            case 22: randomMarchDelay = strSub.toInt();
            break;
        }
        i++;
    }
}

/*
void descryptWakeUp(String data) {
    //Req = "...@ state_sunday, h, m; state_tuesday, h, m; state_wednesday, h, m; state_thursday, h, m; state_friday, h, m; state_saturday, h, m; state_monday, h, m &"
    data.remove(0, data.indexOf("@")+1);
    data.remove(data.indexOf("HTTP/1.1"), data.length());
    int i=0;

    while (true) {
        int position = 0;
        position = data.indexOf(';');
        if (position == -1) position = data.indexOf('&');
        if (position == -1) return;
        String strSub = data.substring(0, position);
        data.remove(0, position+1);
        
        bool whileTr = false;
        byte counterWhile = 0;

        while (whileTr) {
            int posSub = strSub.indexOf(",");
            if (position == -1) position = data.indexOf(';');
            if (position == -1) whileTr = !whileTr;
            String strSubSub = strSub.substring(0, posSub);
            strSub.remove(0, posSub+1);
            
            switch (counterWhile) {
            case 0: 
                if (StrToBool(strSubSub)) IsWakeUpDay[i] = true;
                else IsWakeUpDay[i] = false;
                break;
            case 1:
                if (IsWakeUpDay[i]) WakeUpHourDays[i] = strSubSub.toInt();
                else WakeUpHourDays[i] = 0;
                break;
            case 2:
                if (IsWakeUpDay[i]) WakeUpMinuteDays[i] = strSubSub.toInt();
                else WakeUpMinuteDays[i] = 0;
                break;
            }
            counterWhile++;
        }
        i++;
    }
}
*/

bool StrToBool(String str) {
    bool out;
    int i = str.toInt();
    if (i == 1) out=true;
    else out=false;
    return out;
}