#include<SoftwareSerial.h>
#include<LiquidCrystal.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

#define samples 10
#define minVal -3
#define MaxVal 3
#define led 11
#define buzzer 10

/* Assign a unique ID to this sensor at the same time */
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

SoftwareSerial gsm(2,3); //make RX arduino line is pin 2, make TX arduino line is pin 3.
SoftwareSerial wifi(12,13);//make RX arduino line is pin 12, make TX arduino line is pin 13.
LiquidCrystal lcd(4,5,6,7,8,9); // rs = 4,Contrast=GND, E = 5, 6 = D4, 7 = D5, 8 = D6, 9 = D7

int xsample=0;
int ysample=0;
int zsample=0;

//String mySSID = "Shivu";       // WiFi SSID
//String myPWD = "9661481381"; // WiFi Password

//String mySSID = "Riyanchhi";       // WiFi SSID
//String myPWD = "riya1054"; // WiFi Password

String mySSID = "Redmii";       // WiFi SSID
String myPWD = "9972047474"; // WiFi Password

//String mySSID = "AndroidAP";       // WiFi SSID
//String myPWD = "blht4975"; // WiFi Password

String myAPI = "80SI6JU4TQ0UFLH7";   // Write API Key
String myHOST = "api.thingspeak.com";
String myPORT = "80";
// Latitude and Longitude values
float latitude= 12.940084; 
float longitude= 77.564927; 
//Car Detail
String carNum = "1355"; //Car number  

void initModule(String cmd,String res, int t){
  char response[res.length()];
  res.toCharArray(response,res.length());
  while(1){
    Serial.println(cmd);
    gsm.println(cmd);
    delay(100);
    while(gsm.available()>0){
       if(gsm.find(response)){
          Serial.println(res);
          delay(t);
          return;
       }//end of if
       else
        Serial.println("Error");
    }//end of while
    delay(t);
  }//end of while
}//end of method

void espData(String command, const int timeout){
  Serial.print("AT Command ==> ");
  Serial.print(command);
  Serial.println("     ");
  
  String response = "";
  wifi.println(command);
  long int time = millis();
  while ( (time + timeout) > millis()){
    while (wifi.available()){
      char c = wifi.read();
      response += c;
    }//end of while
  }//end of while
  //Serial.print(response);
}//end of method

void setup() {
  pinMode(led,OUTPUT);
  pinMode(buzzer,OUTPUT);
  if(!accel.begin()){
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while(1);
  }//end of while
  wifi.begin(115200);
  gsm.begin(9600);
  Serial.begin(9600);
  lcd.begin(16,2);  
  lcd.print("Accident Alert");
  lcd.setCursor(0,1);
  lcd.print("     System     ");
  delay(2000);
  lcd.clear();
  Serial.println("Initializing....");
  lcd.setCursor(0,1);
  lcd.print("Please Wait...");
  delay(1000);
  // GSM AT commands
  initModule("AT","OK",1000);
  initModule("ATE1","OK",1000);
  initModule("AT+CPIN?","READY",1000);  
  initModule("AT+CMGF=1","OK",1000);     
  initModule("AT+CNMI=2,2,0,0,0","OK",1000);  
  Serial.println("Initialized GSM");
  lcd.clear();
  lcd.print("Initialized GSM");
  //Wifi AT Commands
  espData("AT+RST", 1000);                      //Reset the ESP8266 module
  espData("AT+CWMODE=1", 1000);                 //Set the ESP mode as station mode
  espData("AT+CWJAP=\""+ mySSID +"\",\""+ myPWD +"\"", 1000);   //Connect to WiFi network
  //while(!wifi.find("OK")) ;//Wait for connection

  Serial.println("Initialized Wifi");
  lcd.clear();
  lcd.print("Initialized Wifi");
  delay(1000);
  lcd.clear();
  lcd.print("Initialized");
  lcd.setCursor(0,1);
  lcd.print("Successfully");
  delay(2000);
  lcd.clear();
  lcd.print("Caliberating ");
  lcd.setCursor(0,1);
  lcd.print("Accelerometer");
  sensors_event_t event; 
  accel.getEvent(&event);
 
  for(int i=0;i<samples;i++){
    xsample+=event.acceleration.x;
    ysample+=event.acceleration.y;
    zsample+=event.acceleration.z;
  }//end of for

  xsample/=samples;
  ysample/=samples;
  zsample/=samples;

  Serial.println(xsample);
  Serial.println(ysample);
  Serial.println(zsample);
  delay(1000);
  
  lcd.clear();
}//end of setup

void loop(){
     /* Get a new sensor event */ 
    sensors_event_t event; 
    accel.getEvent(&event);
    int value1=event.acceleration.x;
    int value2=event.acceleration.y;
    int value3=event.acceleration.z;

    int xValue=xsample-value1;
    int yValue=ysample-value2;
    int zValue=zsample-value3;
    
    Serial.print("x=");
    Serial.println(xValue);
    Serial.print("y=");
    Serial.println(yValue);
    Serial.print("z=");
    Serial.println(zValue);

    if(xValue < minVal || xValue > MaxVal  || yValue < minVal || yValue > MaxVal  || zValue < minVal || zValue > MaxVal)
    {
      /*Accident has occurred*/
      digitalWrite(led,HIGH);
      digitalWrite(buzzer,HIGH);
      delay(2000);
      digitalWrite(led,LOW);
      digitalWrite(buzzer,LOW);
      lcd.clear();
      lcd.print("Accident!");
      delay(2000);
      show_coordinate();
      delay(2000);
      lcd.clear();
      lcd.print("Sending SMS ");
      Serial.println("Sending SMS");
      Send();
      Serial.println("SMS Sent");
      delay(2000);
      lcd.clear();
      lcd.print("Upload to Cloud");
      Upload();
      delay(2000);
      lcd.clear();
      lcd.print("Open Web Page");
      delay(5000);
    }//end of if
    else{
      lcd.clear();
      for(int i =0; i<16; i++){
        lcd.setCursor(i,0);
        lcd.print("Driving Safely:)");
        delay(1000);
        lcd.clear();
      }//end of for  
    }  //end of else
}//end of loop

void Upload(){
  String sendData = "GET /update?api_key="+ myAPI +"&field1="+String(latitude)+"&field2="+String(longitude)+"&field3="+carNum ;
    espData("AT+CIPMUX=1", 1000);       //Allow multiple connections
    espData("AT+CIPSTART=0,\"TCP\",\""+ myHOST +"\","+ myPORT, 1000);
    espData("AT+CIPSEND=0," +String(sendData.length()+4),1000);  
    wifi.find(">"); 
    wifi.println(sendData);
    Serial.print("Sent Latitude: ");
    Serial.println(latitude);
    Serial.print("Sent Longitude: ");
    Serial.println(longitude); 
    espData("AT+CIPCLOSE=0",1000);
    delay(2000);
    Serial.println("Upload Successful");
    lcd.clear();
    lcd.print("Upload done");
}//end of method

void show_coordinate(){
    lcd.clear();
    lcd.print("Lat:");
    lcd.print(latitude);
    lcd.setCursor(0,1);
    lcd.print("Log:");
    lcd.print(longitude);
    Serial.print("Latitude:");
    Serial.println(latitude);
    Serial.print("Longitude:");
    Serial.println(longitude);
    delay(2000);
    lcd.clear();
}//end of method

void Send(){ 
   gsm.println("AT");
   delay(500);
   serialPrint();
   gsm.println("AT+CMGF=1");
   delay(500);
   serialPrint();
   gsm.print("AT+CMGS=");
   gsm.print('"');
   gsm.print("9972047474");    //mobile no. for SMS alert
   gsm.println('"');
   delay(500);
   serialPrint();
   gsm.println("Accident Alert !!");
   gsm.println("Location of accident:");
   gsm.print("Latitude:");
   gsm.println(latitude);
   delay(500);
   serialPrint();
   gsm.print(" longitude:");
   gsm.println(longitude);
   delay(500);
   serialPrint();
   delay(500);
   serialPrint();
   gsm.print("http://maps.google.com/maps?&z=15&mrt=yp&t=k&q=");
   gsm.print(latitude,6);
   gsm.print("+");             
   gsm.print(longitude,6);
   gsm.write(26);
   delay(2000);
   serialPrint();
}//end of method

void serialPrint(){
  while(gsm.available()>0)
    Serial.print((char)gsm.read());
}//end of method
