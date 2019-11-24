#include<SoftwareSerial.h>
SoftwareSerial wifi(12,13);
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(500);
  wifi.begin(115200);
  Serial.print("Wifi connected");
  delay(500);
}
//void loop(){}

void loop() { // run over and over
  if(Serial.available()){
    wifi.write(Serial.read());
  }
  if (wifi.available()) {
    Serial.write(wifi.read());
  }
}

//GET /update?api_key=80SI6JU4TQ0UFLH7&field1=12.940084&field2=77.564927&field3=1355
// length = 90

//GET /update?key=ZNVYA6O9D6DL1EMF&field1=5&headers=false HTTP/1.1CrLf(13,10)Host: api.thingspeak.comCrLf(13,10)Connection: closeCrLf(13,10)Accept: */*CrLf(13,10)CrLf(13,10)

//http://api.thingspeak.com/channels/39433/fields/2/last
