#include<WiFi.h>
#include<WebServer.h>
#include <Preferences.h>

WebServer  esp32_server(80);
Preferences prefs;

IPAddress AP_local_ip(192,168,4,1);          //IP地址
IPAddress AP_gateway(192,168,4,1);           //网关地址
IPAddress AP_subnet(255,255,255,0);       //子网掩码
const char *AP_SSID="ESP32";
const char *AP_Password="";
String ssid = "";
String password = "";
int wifiConnected=0;

String page_html = R"(
<!DOCTYPE html>
<html lang='en'>
  <head>
    <meta charset='UTF-8'>
   
    <meta name="viewport" content="width=device-width,initial-scale=1.0,maximum-scale=1.0,minimum-scale=1.0,user-scalable=no">
    <title>WIFI配置页面</title>
    <style type="text/css">
      * { margin: 0; padding: 0; }
       html { height: 100%; }
       h2 {text-align: center;color: #fff;line-height: 2.2;}
       body { height: 100%; background-color: #46A3FF; 50% 50% no-repeat; background-size: cover;}
       .dowebok { position: absolute; left: 50%; top: 30%; width: 380px; height: 500px; margin: -200px 0 0 -200px; border: 3px solid #fff; border-radius: 10px; overflow: hidden;}
       
       .form-item { position: relative; width: 360px; margin: 0 auto; padding-bottom: 20px;}
       .form-item input { width: 288px; height: 48px; padding-left: 10px; border: 1px solid #fff; border-radius: 25px; font-size: 18px; color: #fff; background-color: transparent; outline: none;}
       .send_button { width: 360px; height: 50px; border: 0; border-radius: 25px; font-size: 18px; color: #1f6f4a; outline: none; cursor: pointer; background-color: #fff; }
       
       .tip { display: none; position: absolute; left: 20px; top: 52px; font-size: 14px; color: #f50; }
       .reg-bar { width: 360px; margin: 20px auto 0; font-size: 14px; overflow: hidden;}
       .reg-bar a { color: #fff; text-decoration: none; }
       .reg-bar a:hover { text-decoration: underline; }
       .reg-bar .reg { float: left; }
       .reg-bar .forget { float: right; }
       .dowebok ::-webkit-input-placeholder { font-size: 18px; line-height: 1.4; color: #fff;}
       .dowebok :-moz-placeholder { font-size: 18px; line-height: 1.4; color: #fff;}
       .dowebok ::-moz-placeholder { font-size: 18px; line-height: 1.4; color: #fff;}
       .dowebok :-ms-input-placeholder { font-size: 18px; line-height: 1.4; color: #fff;}
        
       @media screen and (max-width: 500px) {
       * { box-sizing: border-box; }
       .dowebok { position: static; width: auto; height: auto; margin: 0 30px; border: 0; border-radius: 0; }
       .logo { margin: 50px auto; }
       .form-item { width: auto; }
       .form-item input, .form-item button, .reg-bar { width: 100%; }
       }
       
    </style>
  </head>
  
  <body>
    <div class="dowebok">
      <h2>参 数 配 置</h2>
      <form style='text-align: center;padding-top: 20px' name='input' action='/Connect' method='POST'>  
         <div class="form-item">
          <input id="username" type="text" name='ssid' autocomplete="off" placeholder="WiFi名称">
         </div>
         <div class="form-item">
          <input id="password" type="password" name='password' autocomplete="off" placeholder="WiFi密码">
         </div>
         <div class="form-item">
           <div id="">
            <input id="send_button" type='submit' value='保存并连接'>
           </div>
        </div>
      </form> 
     </div>
  </body>
</html>
)";

void setUpAp();
void setUpWifi(){
    prefs.begin("WIFI",false);
    String PrefSSID= prefs.getString("ssid", "none");
    String PrefPassword= prefs.getString("password", "none");
    String str;
    str="{\"ssid\":\"";
    str+=PrefSSID;
    str+="\",\"password\":\"";
    str+=PrefPassword;
    str+="\"}";
    prefs.end();
    if( PrefSSID == "none" ){
      setUpAp();
      return;
    }
    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);
    WiFi.begin(PrefSSID.c_str(), PrefPassword.c_str());
    while (WiFi.status() != WL_CONNECTED)  
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println(str);
    wifiConnected=1;
}

void clearWifi(){
  Serial.println("clear");
  delay(1000);
  prefs.begin("WIFI",false);
  prefs.clear();
  prefs.end();
  ESP.restart();
}

void handleRoot(){
  esp32_server.send(200, "text/html", page_html);
}

void handleConnect() {                          
  if (esp32_server.hasArg("ssid"))
    ssid=esp32_server.arg("ssid");
  if (esp32_server.hasArg("password"))
    password=esp32_server.arg("password");
  prefs.begin("WIFI",false);
  prefs.putString("ssid", ssid);
  prefs.putString("password", password);
  prefs.end();
  setUpWifi();       
}

void handleFound(){
  esp32_server.send(404,"text/plain","404:Not Found!");
}

void setUpAp(){
  WiFi.softAPConfig(AP_local_ip, AP_gateway, AP_subnet);
  WiFi.softAP(AP_SSID,AP_Password);  //设置AP模式热点的名称和密码，密码可不填则发出的热点为无密码热点

  esp32_server.begin();  //启动网络服务器
  esp32_server.on("/",HTTP_GET,handleRoot);
  esp32_server.on("/Connect", HTTP_POST, handleConnect);
  esp32_server.onNotFound(handleFound);
}

/* void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);  //初始化串口通信并设置波特率为115200
  setUpAp();
  
}

void loop() {
  if(wifiConnected==0)
    esp32_server.handleClient();
}
 */
