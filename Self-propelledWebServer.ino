#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <FS.h>

#include "config.h"

const char* host = "esp8266fs";

ESP8266WebServer Server(80);      //80番ポートを使用


enum {
  M1_l = 4,
  M1_r = 5,
  M2_l = 12,
  M2_r = 13,
  Pilot = 16,
  Stop,
  Open,
  Forw,
  Back,
  Right,
  Left
};

typedef struct {
  int _mode;
  int val;
} motor_T;

motor_T motor;

/**************************************************

  WEBサーバーに関する関数群

 **************************************************/
/**
  Show URI args
*/
void showUriArgs() {
  Serial.printf("\n---URI args---\n");
  for (int i = 0; i < Server.args(); i++) {
    Serial.printf("%s: %s \n", Server.argName(i).c_str(), Server.arg(i).c_str());
  }
}

/**
  ファイルの拡張子を調べてMIMEタイプを返す関数
*/
String getContentType(String filename) {
  if (Server.hasArg("download")) return "application/octet-stream";
  else if (filename.endsWith(".htm"))   return "text/html";
  else if (filename.endsWith(".html"))   return "text/html";
  else if (filename.endsWith(".css"))    return "text/css";
  else if (filename.endsWith(".js"))     return "application/javascript";
  else if (filename.endsWith(".png"))    return "image/png";
  else if (filename.endsWith(".gif"))    return "image/gif";
  else if (filename.endsWith(".jpg"))    return "image/jpeg";
  else if (filename.endsWith(".ico"))    return "image/x-icon";
  else if (filename.endsWith(".xml"))    return "text/xml";
  else if (filename.endsWith(".pdf"))    return "application/x-pdf";
  else if (filename.endsWith(".zip"))    return "application/x-zip";
  else if (filename.endsWith(".gz"))    return "application/x-gzip";
  else return "text/plain";
}
/**
  指定されたパスのファイルをクライアントに送信
*/
void handleSendRes(void) {
  showUriArgs();

  String path = Server.uri();

  if (path.equals("/motor.html")) {
    motor._mode = Open;
    if (Server.arg("motorMode").equals("forw")) {
      motor._mode = Forw;
    } else if (Server.arg("motorMode").equals("back")) {
      motor._mode = Back;
    } else if (Server.arg("motorMode").equals("right")) {
      motor._mode = Right;
    } else if (Server.arg("motorMode").equals("left")) {
      motor._mode = Left;
    }

    motor.val = atoi(Server.arg("motorVal").c_str());
  }

  Serial.println("");
  Serial.println("[handleSendRes]: trying to read " + path);

  // パス指定されたファイルがあればクライアントに送信する
  if (path.endsWith("/")) path += "index.html";

  String contentType = getContentType(path);

  if (SPIFFS.exists(path)) {
    Serial.println("[handleSendRes]: sending " + path);

    File file = SPIFFS.open(path, "r");
    Server.streamFile(file, contentType);
    file.close();

    Serial.println("[handleSendRes]: sent " + path);
  } else {
    Serial.println("[handleSendRes]: 404 not found");

    Server.send (404, "text/plain", "ESP: 404 not found");
  }
}


/**
  settings
*/
void setup() {
  motor._mode = Open;
  motor.val = 0;

  Serial.begin(74880);

  pinMode(Pilot, OUTPUT);
  digitalWrite(Pilot, HIGH);

  motorSetup(M1_l, M1_r, M2_l, M2_r);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  delay(100);

  Serial.println("");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    digitalWrite(Pilot, !digitalRead(Pilot));
    Serial.print(".");
  }
  digitalWrite(Pilot, HIGH);

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  MDNS.begin(host);

  SPIFFS.begin();
  {
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      Serial.printf("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
    }
    Serial.printf("\n");
  }

  //  ウェブサーバの設定
  Server.on("/list", HTTP_GET, handleFileList);
  //load editor
  Server.on("/edit", HTTP_GET, []() {
    if (!handleFileRead("/edit.htm")) {
      Server.send(404, "text/plain", "FileNotFound");
    }
  });
  //create file
  Server.on("/edit", HTTP_PUT, handleFileCreate);
  //delete file
  Server.on("/edit", HTTP_DELETE, handleFileDelete);
  //first callback is called after the request has ended with all parsed arguments
  //second callback handles file uploads at that location
  Server.on("/edit", HTTP_POST, []() {
    Server.send(200, "text/plain", "");
  }, handleFileUpload);

  //get heap status, analog input value and all GPIO statuses in one json call
  Server.on("/all", HTTP_GET, []() {
    String json = "{";
    json += "\"heap\":" + String(ESP.getFreeHeap());
    json += ", \"analog\":" + String(analogRead(A0));
    json += ", \"gpio\":" + String((uint32_t)(((GPI | GPO) & 0xFFFF) | ((GP16I & 0x01) << 16)));
    json += "}";
    Server.send(200, "text/json", json);
    json = String();
  });



  Server.onNotFound(handleSendRes);  ////called when handler is not assigned

  Server.begin();                     //  ウェブサーバ開始

}

/**
  main loop
*/
void loop() {
  Server.handleClient();
  MDNS.update();

  switch (motor._mode) {
    case Forw:
      goForward(motor.val);
      break;
    case Back:
      goBack(motor.val);
      break;
    case Right:
      turnRight(motor.val);
      break;
    case Left:
      turnLeft(motor.val);
      break;
    case Open:
    default:
      stopMotor();
  }
}
