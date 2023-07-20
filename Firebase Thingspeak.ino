#include <FirebaseESP32.h>
#include <DHT.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <Wire.h>
#include <BH1750.h>
#include <ThingSpeak.h>
#include <WiFiClientSecure.h>
BH1750 lightMeter;
#define FIREBASE_HOST "https://smartmelonfarm-92fda-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "EZQcSQ0JysZTsB40Fy2EYcJdQUetx6NQzgeM3pGj"
#define WIFI_SSID "Meow" // Tên wifi của bạn
#define WIFI_PASSWORD "khang@20" // Password wifi của bạn
#define SECRET_CH_ID 1751278
#define SECRET_WRITE_APIKEY "FPIOPXS107GQZQPZ"
#define DHTPIN 15   
#define DHTTYPE DHT11 
#define roleden 12 
#define rolequat 14
#define rolebom 2 
int sensor_pin = 34;
int doamdat;
DHT dht(DHTPIN, DHTTYPE);
FirebaseData fbdo;
WiFiClient  client;
String canhbao1;
String canhbao2;
String canhbao3;
void setup() {

  Serial.begin(115200);
  pinMode(roleden, OUTPUT);
  pinMode(rolequat, OUTPUT);
  pinMode(rolebom, OUTPUT);
  delay(1000);
  WiFi.begin (WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Dang ket noi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    Wire.begin();
    lightMeter.begin();//Khởi tạo cảm biến
    Serial.println(F("BH1750 Test begin"));
  }

  dht.begin();
  Serial.println ("");
  Serial.println ("Da ket noi WiFi!");
  Serial.println(WiFi.localIP());
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  ThingSpeak.begin(client);
}

void loop() {

  float h = dht.readHumidity();
  float t = dht.readTemperature();  // Đọc nhiệt độ theo độ C

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;

  }

  Serial.print("Nhiet do: ");
  Serial.print(t);
  Serial.print("*C  ");
  Serial.print("Do am: ");
  Serial.print(h);
  Serial.println("%  ");

  Firebase.setFloat( fbdo,"nhietdo", t);

  Firebase.setFloat ( fbdo,"doam", h);

  int x =  ThingSpeak.setField(1, t); //setField(field, value)
  int y =  ThingSpeak.setField(2, h); //setField(field, value)
 
 //cam bien do am dat
  doamdat= analogRead(sensor_pin);
  doamdat= map(doamdat,4095,0,0,100);
  Serial.print("Do am dat : ");
  Serial.print(doamdat);
  Serial.println("%");
  Firebase.setFloat ( fbdo,"doamdat", doamdat);
  int f =  ThingSpeak.setField(3, doamdat); //setField(field, value)
//cam bien cuong do anh sang
  float lux = lightMeter.readLightLevel();
  Serial.print("Cuong do anh sang: ");
  Serial.print(lux);
  Serial.println(" lx");
  delay(2000);
  Firebase.setFloat ( fbdo,"cuongdoanhsang", lux);
  int e =  ThingSpeak.setField(4, lux); //setField(field, value)
   int z = ThingSpeak.writeFields(SECRET_CH_ID,SECRET_WRITE_APIKEY);
  delay(200);
//dieu khien den
  if(Firebase.getString(fbdo, "dieukhien/den/button"))
  {
    String den = fbdo.stringData();
    Serial.println("Download success: " + String(fbdo.stringData()));
    if(fbdo.stringData()=="ON")
    digitalWrite(roleden, HIGH);
    else
    digitalWrite(roleden, LOW);
  }else {
    Serial.println("Download success: " + String(fbdo.stringData()));
}
//dieu khien den quat
if(Firebase.getString(fbdo, "dieukhien/quat/button"))
  {
    String quat = fbdo.stringData();
    Serial.println("Download success: " + String(fbdo.stringData()));
    if(fbdo.stringData()=="ON")
    digitalWrite(rolequat, HIGH);
    else
    digitalWrite(rolequat, LOW);
  }else {
    Serial.println("Download success: " + String(fbdo.stringData()));
}
  delay(1000);
 // canh bao nhiet do
 if(t<25) canhbao1 = "Thấp";
 else if(t>30) canhbao1="Cao";
 else canhbao1= "Bình thường";
 Firebase.setString ( fbdo,"canhbao/nhietdo", canhbao1);
  // canh bao do am khong khi
 if(h<85) canhbao2 = "Thấp";
 else if(h>85) canhbao2="Cao";
 else canhbao2= "Bình thường";
 Firebase.setString ( fbdo,"canhbao/doamkhongkhi", canhbao2);
   // canh bao do am dat
 if(doamdat<70) canhbao3 = "Thấp";
 else if(doamdat>70) canhbao3="Cao";
 else canhbao3= "Bình thường";
 Firebase.setString ( fbdo,"canhbao/doamdat", canhbao3);
 //bom tu dong
 if(doamdat < 70){digitalWrite(rolebom,HIGH);}//bat role
  else digitalWrite(rolebom,LOW); //tat role
 if(Firebase.getString(fbdo, "dieukhien/bom/button"))
  {
    String quat = fbdo.stringData();
    Serial.println("Download success: " + String(fbdo.stringData()));
    if(fbdo.stringData()=="ON")
    digitalWrite(rolebom, HIGH);
    else
    digitalWrite(rolebom, LOW);
  }else {
    Serial.println("Download success: " + String(fbdo.stringData()));
}
}
