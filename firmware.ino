#include "config.h"
#include "Adafruit_FONA.h"
#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>

// -------------------------
// ----- Dislay setup ------
// -------------------------
#ifdef DISP
    #include <Adafruit_GFX.h>    // Core graphics library
    #include <Adafruit_ST7735.h> // Hardware-specific library
    #include <SPI.h>
    // use hardware SPI
    Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);
    #define DEBUG_FONT_SIZE(x) tft.setTextSize(x);
    #define DEBUG_FONT_COLOR(x) tft.setTextColor(x);
    #define DEBUG_CLEAR() {tft.fillScreen(ST7735_BLACK); tft.setCursor(0, 0);}
    #define DEBUG_SET_CURSOR(x, y) tft.setCursor(x, y);
    #define DEBUG_PRINT(x) tft.print(x);
    #define DEBUG_PRINTLN(x) tft.println(x);
#else
    #define DEBUG_FONT_SIZE(x) ;
    #define DEBUG_FONT_COLOR(x) ;
    #define DEBUG_CLEAR() ;
    #define DEBUG_SET_CURSOR(x, y) ;
    #define DEBUG_PRINT(x) Serial.print(x);
    #define DEBUG_PRINTLN(x) Serial.println(x);
#endif

// -----------------------
// ----- Global Vars -----
// -----------------------
// FONA
SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SoftwareSerial *fonaSerial = &fonaSS;
/*HardwareSerial *fonaSerial = &Serial;*/
Adafruit_FONA fona = Adafruit_FONA(FONA_RST);
// 3 addressable LEDs
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, NEO_PIN, NEO_GRB + NEO_KHZ800);
/*const char* id = "build1";*/

int raw[5];     // global raw variable to store readings
/*String to_send;*/
char to_send[300];

void setup_gsm() {
    // main connection page
    DEBUG_FONT_SIZE(2)
    DEBUG_FONT_COLOR(ST7735_GREEN)
    DEBUG_PRINTLN(F("Initializing"))

    DEBUG_FONT_COLOR(ST7735_YELLOW)
    DEBUG_PRINTLN(F("GSM:"))
    // INFO: FONA 808 V2
    fonaSerial->begin(4800);
    if (! fona.begin(*fonaSerial)) {
        DEBUG_FONT_COLOR(ST7735_RED)
        DEBUG_PRINTLN(F("ERROR"));
        while (1);
    }
    DEBUG_FONT_COLOR(ST7735_GREEN)
    DEBUG_PRINTLN(F("Connected!"))
    delay(5000);
    uint8_t n = fona.getNetworkStatus();
    if (n == 0) {
        DEBUG_FONT_COLOR(ST7735_RED)
        DEBUG_PRINTLN(F("Not\nregistered"));
        while(1);
    }
    if (n == 1) {
        DEBUG_FONT_COLOR(ST7735_GREEN)
        DEBUG_PRINTLN(F("Registered\n(home)"));
    }
    if (n == 2) {
        DEBUG_FONT_COLOR(ST7735_YELLOW)
        DEBUG_PRINTLN(F("Not\nregistered\n(searching)"));
        DEBUG_FONT_COLOR(ST7735_RED)
        DEBUG_PRINTLN(F("Retrying..."))
        delay(3000);
        if (fona.getNetworkStatus() == 1) {
            DEBUG_FONT_COLOR(ST7735_GREEN)
            DEBUG_PRINTLN(F("Regsitered!"))
        } else {
            DEBUG_PRINTLN(F("Error!"))
        }
    }
    if (n == 3) {
        DEBUG_FONT_COLOR(ST7735_RED)
        DEBUG_PRINTLN(F("Denied"));
        while(1);
    }
    if (n == 4) {
        DEBUG_FONT_COLOR(ST7735_RED)
        DEBUG_PRINTLN(F("Unknown"));
        while(1);
    }
    if (n == 5) {
        DEBUG_FONT_COLOR(ST7735_RED)
        DEBUG_PRINTLN(F("Registered\nroaming"));
        while(1);
    }
    delay(5000);

    // RSSI
    DEBUG_CLEAR()
    DEBUG_FONT_COLOR(ST7735_GREEN)
    DEBUG_PRINTLN(F("Initializing"))
    n = fona.getRSSI();
    int8_t r;
    DEBUG_FONT_COLOR(ST7735_YELLOW)
    DEBUG_PRINTLN(F("RSSI:"));
    DEBUG_FONT_COLOR(ST7735_RED)
    DEBUG_PRINT(n); DEBUG_PRINT(": ");
    if (n == 0) r = -115;
    if (n == 1) r = -111;
    if (n == 31) r = -52;
    if ((n >= 2) && (n <= 30)) {
      r = map(n, 2, 30, -110, -54);
    }
    DEBUG_PRINT(r); DEBUG_PRINTLN(F(" dBm"));

    // GPRS (same page as RSSI)
    DEBUG_FONT_COLOR(ST7735_YELLOW)
    DEBUG_PRINTLN(F("GPRS:"));
    fona.setGPRSNetworkSettings(F("internet.com"), F("wapuser1"), F("wap"));
    delay(1000);
    for (int i = 0; i < 3; i++) {
        if (!fona.enableGPRS(true)) {
            DEBUG_FONT_COLOR(ST7735_RED)
            if (i == 2) {
                DEBUG_PRINTLN(F("Failed!"));
                while(1);
            } else {
                DEBUG_PRINTLN(F("Retrying..."));
            }
            delay(2000);
        } else { break; }
    }
    DEBUG_FONT_COLOR(ST7735_GREEN)
    DEBUG_PRINTLN(F("Enabled!"))
    delay(5000);

    // time
    DEBUG_CLEAR()
    DEBUG_FONT_COLOR(ST7735_GREEN)
    DEBUG_PRINTLN(F("Initializing"))
    DEBUG_FONT_COLOR(ST7735_YELLOW)
    DEBUG_PRINTLN(F("Time:"));
    if (!fona.enableNTPTimeSync(true, F("pool.ntp.org"))) {
        DEBUG_FONT_COLOR(ST7735_RED)
        DEBUG_PRINTLN(F("Error!"));
    }
    DEBUG_FONT_COLOR(ST7735_GREEN)
    DEBUG_PRINTLN(F("Synced!"))
    // read the time
    char buffer[23];
    fona.getTime(buffer, 23);  // make sure replybuffer is at least 23 bytes!
    DEBUG_FONT_COLOR(ST7735_RED)
    DEBUG_FONT_SIZE(1)
    DEBUG_PRINTLN(buffer);
    delay(3000);

    // gps or GSMLOC
    /*DEBUG_CLEAR()*/
    DEBUG_FONT_SIZE(2)
    /*DEBUG_FONT_COLOR(ST7735_GREEN)*/
    /*DEBUG_PRINTLN(F("Initializing"))*/
    DEBUG_FONT_COLOR(ST7735_YELLOW)
    DEBUG_PRINTLN(F("GMSLOC:"));
    /*if (!fona.enableGPS(true)) {
        DEBUG_FONT_COLOR(ST7735_RED)
        DEBUG_PRINTLN(F("Error!"));
    }
    DEBUG_FONT_COLOR(ST7735_GREEN)
    DEBUG_PRINTLN(F("Enabled!"))
    int8_t stat = fona.GPSstatus();
    DEBUG_PRINTLN(stat)
    while(1) {
        int8_t stat = fona.GPSstatus();
        DEBUG_PRINTLN(stat)
        delay(5000);
    }*/
    float lat, lon;
    if (!fona.getGSMLoc(&lat, &lon)) {
        DEBUG_FONT_COLOR(ST7735_RED)
        DEBUG_PRINTLN(F("Error!"));
    } else {
        DEBUG_FONT_COLOR(ST7735_GREEN)
        DEBUG_PRINTLN(F("Enabled!"))
        DEBUG_FONT_COLOR(ST7735_RED)
        DEBUG_FONT_SIZE(1)
        DEBUG_PRINTLN(lat);
        DEBUG_PRINTLN(lon);
    }
    delay(3000);

}

void heatup_routine() {
    DEBUG_CLEAR()
    DEBUG_FONT_SIZE(2)
    DEBUG_FONT_COLOR(ST7735_GREEN)
    DEBUG_PRINTLN(F("Initializing"))
    DEBUG_FONT_COLOR(ST7735_YELLOW)
    DEBUG_PRINTLN(F("Heating"));
    DEBUG_PRINTLN(F("sensors..."));
    DEBUG_FONT_COLOR(ST7735_RED)
    uint16_t t =millis();
    uint16_t diff = 0;
    while (diff < 30000) {
        DEBUG_SET_CURSOR(0, 49)
        diff = (millis() - t);
        if ((diff % 1000) == 0) {
#ifdef DISP
            tft.fillRect(0, 49, 50, 15, ST7735_BLACK);
#endif
            DEBUG_PRINT(30-int(diff/1000))
            DEBUG_PRINTLN('s')
        }
    }
}

void setup() {

#ifdef DISP
    // INFO: 16 pixels with 1 pixel in between
    // initialize a ST7735S chip, black tab
    tft.initR(INITR_BLACKTAB);
    // rotate to horizontal view
    tft.setRotation(1);
    tft.setTextWrap(false);
    tft.fillScreen(ST7735_BLACK);
    tft.setCursor(0, 0);
#else
    Serial.begin(115200);
#endif

    pixels.begin(); // This initializes the NeoPixel library.
    pixels.setPixelColor(0, pixels.Color(0,0,200)); // Moderately bright green color.
    pixels.setPixelColor(1, pixels.Color(0,100,100)); // Moderately bright green color.
    pixels.setPixelColor(2, pixels.Color(0,200,0)); // Moderately bright green color.
    pixels.show();

    /*Serial.println(FONA_RX);*/
    /*Serial.println("Serial Initialized");*/
    setup_gsm();

#ifdef HEAT
    heatup_routine();
#endif

    DEBUG_FONT_SIZE(2)
    DEBUG_FONT_COLOR(ST7735_BLUE)
    DEBUG_PRINTLN(F("Sensors ready!"))
    delay(3000);

    //DEBUG_PRINT4(F("Sensors ready!"), NULL, 2, ST7735_BLUE)

      /*Serial.begin(115200);*/
      /*Serial.println(FONA_RX);*/
    DEBUG_FONT_SIZE(2)
    DEBUG_FONT_COLOR(ST7735_WHITE)
    DEBUG_CLEAR()

}



void loop() {
    raw[CO] = analogRead(CO);
    raw[NO2] = analogRead(NO2);
    raw[O3] = analogRead(O3);
    /*raw[PM25] = analogRead(PM25);*/
    /*raw[PM10] = analogRead(PM10);*/
    DEBUG_CLEAR()
    DEBUG_FONT_COLOR(ST7735_GREEN)
    DEBUG_PRINTLN(F("Sensors:"))
    DEBUG_FONT_COLOR(ST7735_WHITE)
    DEBUG_PRINT(F("CO:    "))
    DEBUG_PRINTLN(raw[CO])
    DEBUG_PRINT("NO2:   ")
    DEBUG_PRINTLN(raw[NO2])
    DEBUG_PRINT("O3:    ")
    DEBUG_PRINTLN(raw[O3])
    read_pm();
    DEBUG_PRINT("PM2.5: ")
    DEBUG_PRINTLN(raw[PM25])
    DEBUG_PRINT("PM10:  ")
    DEBUG_PRINTLN(raw[PM10])

    // TODO: remove quatation marks from reply buffer
    //char t_buff[23];
    //fona.getTime(t_buff, 23);  // make sure replybuffer is at least 23 bytes!

    float lat, lon;
    !fona.getGSMLoc(&lat, &lon);

    uint16_t vbat;
    fona.getBattVoltage(&vbat);

    sprintf(to_send, "http://airsense.azurewebsites.net/newdataget?sub=%s&d_id=%s&type=demo&lat=%ld&lng=%ld&o3=%d&co=%d&no2=%d&pm25=%d&pm10=%d&volt=%u", subid, id, long(lat*10000), long(lon*10000), raw[O3], raw[CO], raw[NO2], raw[PM25], raw[PM10], vbat);
    Serial.println(to_send);


    uint16_t statuscode;
    int16_t length;
    /*"http://airsense.azurewebsites.net/newdataget?device_id=a1234_232&device_type=FONA1&gmt_time=03/09/2016/23:09:40&gps_lat=48.8582&gps_long=2.2945&o3_raw=0.001&co_raw=0.002&no2_raw=0.003&pm_small_raw=0.004&pm_large_raw=0.005&temp_raw=17.3&humidity_raw=55&batt_voltage=4.19"*/
    if (!fona.HTTP_GET_start(to_send, &statuscode, (uint16_t *)&length)) {
      /*Serial.println("Failed!");*/
      DEBUG_FONT_COLOR(ST7735_RED)
      DEBUG_PRINTLN("FAILED")
    } else {
        DEBUG_FONT_COLOR(ST7735_GREEN)
        DEBUG_PRINTLN(F("SENT"))
    }
    fona.flush();
    fona.HTTP_GET_end();
    delay(20000);
}

/*void tft_clear() {
    tft.fillScreen(ST7735_BLACK);
    tft.setCursor(0, 0);
}*/

/*void tft_println(const char c[], int size = 1, uint16_t col = ST7735_WHITE) {
    tft.setTextSize(size);
    tft.setTextColor(col);
    tft.println(c);
}*/

/*void tft_print(const __FlashStringHelper *ifsh, int *val, int size, uint16_t col) {
    tft.setTextSize(size);
    tft.setTextColor(col);
    if (ifsh) tft.print(ifsh);
    if (val) tft.print(*val);
}*/

void read_pm() {
    uint16_t t = millis();
    uint16_t count = 0;
    int raw_pm25 = 0,
        raw_pm10 = 0;

    while ((millis() - t) < PM_MEAS_TIME) {
        if (analogRead(PM25) < PM_25_HRESHOLD) raw_pm25++;
        if (analogRead(PM10) < PM_10_HRESHOLD) raw_pm10++;
    }

    raw[PM25] = int(raw_pm25/PM_MEAS_TIME*100);
    raw[PM10] = int(raw_pm10/PM_MEAS_TIME*100);
}
