#include <ESP8266WiFi.h>      // 本程序使用ESP8266WiFi库
#include <ESP8266WiFiMulti.h> // 本程序使用ESP8266WiFiMulti库
#include <ESP8266WebServer.h> // 本程序使用ESP8266WebServer库
#include <FS.h>               // 本程序使用SPIFFS库
#include <Servo.h> 
Servo myservo;    

ESP8266WiFiMulti wifiMulti;     // 建立ESP8266WiFiMulti对象,对象名称是'wifiMulti'
 
ESP8266WebServer esp8266_server(80);    // 建立网络服务器对象，该对象用于响应HTTP请求。监听端口（80）
                                    
void setup(){
  myservo.attach(D1);  
  Serial.begin(9600);        
  Serial.println("");
  
  pinMode(LED_BUILTIN, OUTPUT);      // 初始化NodeMCU控制板载LED引脚为OUTPUT
 
  //通过addAp函数存储  WiFi名称       WiFi密码
  wifiMulti.addAP("forest's HUAWEI Mate 30 5G", "Hello World"); // 将需要连接的一系列WiFi ID和密码输入这里
  wifiMulti.addAP("301", "30130100"); // ESP8266-NodeMCU再启动后会扫描当前网络
  wifiMulti.addAP("607落泪", "01020202"); // 环境查找是否有这里列出的WiFi ID。如果有
  Serial.println("Connecting ...");                            // 则尝试使用此处存储的密码进行连接。
 
  int i = 0;                                 
  while (wifiMulti.run() != WL_CONNECTED) {  // 在当前环境中搜索addAP函数所存储的WiFi
    delay(1000);                             // 如果搜到多个存储的WiFi那么NodeMCU
    Serial.print(i++); Serial.print('.');                       // 将会连接信号最强的那一个WiFi信号。
  }                                          
                                         
  // WiFi连接成功后将通过串口监视器输出连接成功信息 
  Serial.println('\n');                     // WiFi连接成功后
  Serial.print("Connected to ");            // NodeMCU将通过串口监视器输出。
  Serial.println(WiFi.SSID());              // 连接的WiFI名称
  Serial.print("IP address:\t");            // 以及
  Serial.println(WiFi.localIP());           // NodeMCU的IP地址

  if(SPIFFS.begin()){                       // 启动闪存文件系统
    Serial.println("SPIFFS Started.");
  } else {
    Serial.println("SPIFFS Failed to Start.");
  }
  
  esp8266_server.on("/LED-Control", handleLEDControl); // 告知系统如何处理/LED-Control请求     
  esp8266_server.onNotFound(handleUserRequest);        // 告知系统如何处理其它用户请求     
  
  esp8266_server.begin();                   // 启动网站服务                                  
  Serial.println("HTTP server started");    
}

void loop(){
  esp8266_server.handleClient();  //处理用户请求
}                                

// 处理/LED-Control请求  
void handleLEDControl(){                                  
  myservo.write(20);               
  delay(3000);                               
  myservo.write(90);              
  bool ledStatus = digitalRead(LED_BUILTIN);     // 此变量用于储存LED状态     
  ledStatus == HIGH ? digitalWrite(LED_BUILTIN, LOW) : digitalWrite(LED_BUILTIN, HIGH);  // 点亮或者熄灭LED  
     
  esp8266_server.sendHeader("Location", "/LED.html");       
   esp8266_server.send(303);  
}
                                                                     
// 处理用户浏览器的HTTP访问
void handleUserRequest() {         
     
  // 获取用户请求资源(Request Resource）
  String reqResource = esp8266_server.uri();
  Serial.print("reqResource: ");
  Serial.println(reqResource);
  
  // 通过handleFileRead函数处处理用户请求资源
  bool fileReadOK = handleFileRead(reqResource);

  // 如果在SPIFFS无法找到用户访问的资源，则回复404 (Not Found)
  if (!fileReadOK){                                                 
    esp8266_server.send(404, "text/plain", "404 Not Found"); 
  }
}

bool handleFileRead(String resource) {            //处理浏览器HTTP访问

  if (resource.endsWith("/")) {                   // 如果访问地址以"/"为结尾
    resource = "/index.html";                     // 则将访问地址修改为/index.html便于SPIFFS访问
  } 
  
  String contentType = getContentType(resource);  // 获取文件类型
  
  if (SPIFFS.exists(resource)) {                     // 如果访问的文件可以在SPIFFS中找到
    File file = SPIFFS.open(resource, "r");          // 则尝试打开该文件
    esp8266_server.streamFile(file, contentType);// 并且将该文件返回给浏览器
    file.close();                                // 并且关闭文件
    return true;                                 // 返回true
  }
  return false;                                  // 如果文件未找到，则返回false
}

// 获取文件类型
String getContentType(String filename){
  if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}
