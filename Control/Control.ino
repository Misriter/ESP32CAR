#include <Arduino.h>
#include "PubSubClient.h"
#include <U8g2lib.h>
#include <ArduinoJson.h>
#include <mp3_player_module_wire.h>
#include "WIFI.h"

//const char *ssid = "LG U+ Router_0523AC";                 //wifi名
//const char *password = "05336812";         //wifi密码
//const char *ssid = "iPhone";                 //wifi名
//const char *password = "785243100";         //wifi密码
const char *mqtt_server = "183.230.40.39"; //onenet 的 IP地址

#define mqtt_devid "973637421"  //设备ID
#define mqtt_pubid "535946"     //产品ID
#define mqtt_password "misriter" //鉴权信息

#define LEFT1 13
#define LEFT2 12
#define RIGHT1 14
#define RIGHT2 27
#define LED 2
#define SOUND 5
int resetKey=23;//重置WIFI按钮

#define SCL 15
#define SDA  4

U8G2_SSD1306_128X64_NONAME_1_SW_I2C u8g2(U8G2_R0,SCL,SDA,U8X8_PIN_NONE);
Mp3PlayerModuleWire player(SOUND);

WiFiClient espClient;
PubSubClient client(espClient);

/* void setUpWifi()
{
  delay(10);
  Serial.println("连接WIFI");
  WiFi.begin(ssid, password);
  while (!WiFi.isConnected())
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("OK");
  Serial.println("Wifi连接成功");
} */

int left=-1,up=-1,right=-1,down=-1,led=1,volumn=30;
String text="";
void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.println("message rev:");
  Serial.println(topic);
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  StaticJsonDocument<64> doc;
  DeserializationError error = deserializeJson(doc, payload);
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }
  
  String id = doc["id"];
  String state = doc["state"];
  Serial.println(id);
  Serial.println(state);
  if(id=="move"){
    if(state=="up")
      up=1;
    else if(state=="down")
      down=1;
    else if(state=="left")
      left=1;
    else if(state=="right")
      right=1;
    else if(state=="stop")
      left=-1,up=-1,right=-1,down=-1;
  }
  if(id=="button"){
    if(state=="light"){
      led*=-1;
    }
  }
  if(id=="send"){
    u8g2.firstPage();
    text=state;
  }
  if(id=="mp3"){
    int mp3=atoi(state.c_str());
    Serial.println("mp3:"+mp3);
    player.set_track_index(mp3);
    player.play();
  }
}

void clientReconnect()
{
  while (!client.connected()) //再重连客户端
  {
    Serial.println("reconnect MQTT...");
    if (client.connect(mqtt_devid, mqtt_pubid, mqtt_password))
    {
      Serial.println("connected");
    }
    else
    {
      Serial.println("failed");
      Serial.println(client.state());
      Serial.println("try again in 5 sec");
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(115200);
  delay(3000);
  pinMode(LEFT1,OUTPUT);
  pinMode(LEFT2,OUTPUT);
  pinMode(RIGHT1,OUTPUT);
  pinMode(RIGHT2,OUTPUT);
  pinMode(LED,OUTPUT);
  pinMode(resetKey,INPUT);

  digitalWrite(LED,HIGH);

  setUpWifi();
  client.setServer(mqtt_server, 6002);
  client.connect(mqtt_devid, mqtt_pubid, mqtt_password);
  client.setCallback(callback);

  u8g2.begin();
  u8g2.enableUTF8Print();
  u8g2.setFont(u8g2_font_wqy12_t_gb2312a);
  u8g2.setFontMode(1);
  u8g2.setFontDirection(0);
  u8g2.firstPage();

  player.set_volume(volumn);
  
}

void loop()
{
  if(wifiConnected==0)
      esp32_server.handleClient();
  else{
    if (!WiFi.isConnected()){
      left=-1,up=-1,right=-1,down=-1,led=1;
        setUpWifi();
    }
    if(digitalRead(resetKey) == 1)
        clearWifi();

    if(led==1)
      digitalWrite(LED,HIGH);
    else
      digitalWrite(LED,LOW);
    
    if (!client.connected())
    {
      left=-1,up=-1,right=-1,down=-1,led=1;
      clientReconnect();
    }
    if(up==1){
      digitalWrite(LEFT1,HIGH);digitalWrite(RIGHT1,HIGH);
    }
    else if(down==1){
      digitalWrite(LEFT2,HIGH);digitalWrite(RIGHT2,HIGH);
    }
    else if(left==1){
      digitalWrite(RIGHT1,HIGH);
    }
    else if(right==1){
      digitalWrite(LEFT1,HIGH);
    }
    else{
      digitalWrite(LEFT1,LOW);digitalWrite(LEFT2,LOW);digitalWrite(RIGHT1,LOW);digitalWrite(RIGHT2,LOW);
    }
    if(text!=""){
      u8g2.setCursor(0,15);
      u8g2.print(text);
      u8g2.nextPage();
    }
    client.loop();
  }
  
}
