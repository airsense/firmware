#define CO    0
#define NO2   1
#define O3    2
#define PM25  3
#define PM10  4

#define PM_25_HRESHOLD 10
#define PM_10_HRESHOLD 10
#define PM_MEAS_TIME 30000

int raw[5];

void setup() {

  Serial.begin(115200);
  /*Serial.println(FONA_RX);*/
  Serial.println("Serial Initialized");
#ifdef HEAT
  Serial.println("Heating sensors...");
  delay(10000);
  Serial.println("10s");
  delay(10000);
  Serial.println("20s");
  delay(10000);
  Serial.println("30s");
  Serial.println("Sensors ready!");
#endif
}

void loop() {
  // put your main code here, to run repeatedly:

  /*raw[CO] = analogRead(CO);    // read the input pin*/
  /*raw[NO2] = analogRead(NO2);*/
  /*raw[O3] = analogRead(O3);*/
  raw[PM25] = analogRead(PM25);
  raw[PM10] = analogRead(PM10);

  Serial.println("---------------");
  /*Serial.print("CO:\t");
  Serial.println(raw[CO]);
  Serial.print("NO2:\t");
  Serial.println(raw[NO2]);
  Serial.print("O3:\t");
  Serial.println(raw[O3]);*/
  Serial.print("PM2.5:\t");
  Serial.println(raw[PM25]);
  Serial.print("PM10:\t");
  Serial.println(raw[PM10]);
  Serial.println("---------------");
  Serial.println();
  /*Serial.println();*/
  delay(1);


  /*delay(5000);*/

}

void read_pm() {
    uint16_t time = millis();
    uint16_t count = 0;
    int raw_pm25 = 0,
        raw_pm10 = 0;
    // Run for 30s
    while ((millis() - time) < PM_MEAS_TIME) {
        if (analogRead(PM25) < PM_25_HRESHOLD) raw_pm25++;
        if (analogRead(PM10) < PM_10_HRESHOLD) raw_pm10++;
    }

    raw[PM25] = int(raw_pm25/PM_MEAS_TIME*100);
    raw[PM10] = int(raw_pm10/PM_MEAS_TIME*100);
}
