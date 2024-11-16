#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "LittleFS.h"
#include <Arduino_JSON.h>
// #include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>

// Replace with your network credentials
// const char* ssid = "IoT";
// const char* password = "12244896";

const char* ssid = "SSID";
const char* password = "PASSWORD";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create a WebSocket object
AsyncWebSocket ws("/ws");

// Json Variable to Hold Sensor Readings
JSONVar readings;

// Timer variables 
unsigned long lastTime = 0;
unsigned long timerDelay = 1000;

int humidade = 0;
int mapeado = 0;

// bool enchendo = false;

bool automaticoZona1 = false;
bool enchendoZona1 = false;
float consumoZona1 = 0.0;

bool automaticoZona2 = false;
bool enchendoZona2 = false;
float consumoZona2 = 0.0;

bool automaticoZona3 = false;
bool enchendoZona3 = false;
float consumoZona3 = 0.0;

bool automaticoZona4 = false;
bool enchendoZona4 = false;
float consumoZona4 = 0.0;

// #define ledPin 10
// #define releBomba 12
// #define relepin 13

#define bomba1 18
#define bomba2 19
#define bomba3 21
#define bomba4 22

#define sensor1 32
int valorSensor1 = 0;

#define sensor2 33
int valorSensor2 = 0;

#define sensor3 34
int valorSensor3 = 0;

#define sensor4 35
int valorSensor4 = 0;


String messageZona1[2];
int countIndexZona1 = 0;
// Get Sensor Readings and return JSON object
String getSensorReadings(){

  // mapeado = map(analogRead(humidadePin), 1090, 2500, 100, 0);
  // mapeado = mapeado < 0 ? 0 : mapeado > 100 ? 100 : mapeado;

  // if(enchendo)
  // {
  //   if(mapeado <= 50)
  //   {
  //     digitalWrite(releBomba, HIGH);
  //     enchendo = false;
  //   }
  // }
  
  // else
  // {
  //   if(mapeado >= 10)
  //   { 
  //     enchendo = true;
  //     digitalWrite(releBomba, LOW);
  //   }
  // }
  valorSensor1 = analogRead(sensor1);
  valorSensor1 = map(valorSensor1, 2078, 4095, 100, 0);
  valorSensor1 = valorSensor1 < 0 ? 0 : valorSensor1 >100 ? 100 : valorSensor1;

  valorSensor2 = analogRead(sensor2);
  valorSensor2 = map(valorSensor2, 2044, 4095, 100, 0);
  valorSensor2 = valorSensor2 < 0 ? 0 : valorSensor2 >100 ? 100 : valorSensor2;

  valorSensor3 = analogRead(sensor3);
  valorSensor3 = map(valorSensor3, 1792, 4095, 100, 0);
  valorSensor3 = valorSensor3 < 0 ? 0 : valorSensor3 >100 ? 100 : valorSensor3;

  valorSensor4 = analogRead(sensor4);
  valorSensor4 = map(valorSensor4, 1783, 4095, 100, 0);
  valorSensor4 = valorSensor4 < 0 ? 0 : valorSensor4 >100 ? 100 : valorSensor4;

  // Serial.println((String) "Z1 Automatico -> "+automaticoZona1+" Z2 Automatico -> "+automaticoZona2+ " Z3 Automatico -> "+automaticoZona3+" Z4 Automatico -> "+automaticoZona4);
  checkAutomatico(valorSensor1, valorSensor2, valorSensor3, valorSensor4);
  checkConsumo();

  readings["humidadeZona1"] =  String(valorSensor1);
  readings["consumoZona1"] =  String(consumoZona1);

  readings["humidadeZona2"] =  String(valorSensor2);
  readings["consumoZona2"] =  String(consumoZona2);

  readings["humidadeZona3"] =  String(valorSensor3);
  readings["consumoZona3"] =  String(consumoZona3);

  readings["humidadeZona4"] =  String(valorSensor4);
  readings["consumoZona4"] =  String(consumoZona4);

  // readings["pressure"] = String("120");
  // readings["led"] = String(digitalRead(relepin) == 1 ? "OFF" : "ON");

  String jsonString = JSON.stringify(readings);
  Serial.println(jsonString);
  return jsonString;
}

// Initialize LittleFS
void initLittleFS() {
  if (!LittleFS.begin(true)) { 
    Serial.println("An error has occurred while mounting LittleFS");
  }
  Serial.println("LittleFS mounted successfully");
}

// Initialize WiFi
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void notifyClients(String sensorReadings) {
  ws.textAll(sensorReadings);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    String message = (char*)data;
    
    
    // if(strcmp((char*)data, "toggle") == 0)
    // {
    //   digitalWrite(relepin, digitalRead(relepin) == 0 ? HIGH : LOW);
    // }

    if(message.indexOf('/') != -1)
    {
      int indexZona = message.indexOf('/');
      String zona = message.substring(0, indexZona);

      if(zona.equals("zona1"))
      {
        String zona1Mess = message.substring(indexZona+1);
        Serial.println("zona 1");
        Serial.println(zona1Mess);

        if(zona1Mess.equals("on"))
        {
          digitalWrite(bomba1, LOW);
        }

        if(zona1Mess.equals("off"))
        {
          digitalWrite(bomba1, HIGH);
        }

        if(zona1Mess.equals("manual"))
        {
          digitalWrite(bomba1, HIGH);
          Serial.println("Zona 1 em modo manual");
        }

        automaticoZona1 = zona1Mess.equals("automatico");
        if(automaticoZona1)
        {
          configurarAutomatico(bomba1);
          Serial.println("Zona 1 em modo automatico");
        }
      }

      if(zona.equals("zona2"))
      {
        String zona2Mess = message.substring(indexZona+1);
        Serial.println("zona 2");
        Serial.println(zona2Mess);

        if(zona2Mess.equals("on"))
        {
          digitalWrite(bomba2, LOW);
        }

        if(zona2Mess.equals("off"))
        {
          digitalWrite(bomba2, HIGH);
        }

        if(zona2Mess.equals("manual"))
        {
          digitalWrite(bomba2, HIGH);
          Serial.println("Zona 2 em modo manual");
        }

        automaticoZona2 = zona2Mess.equals("automatico");
        if(automaticoZona2)
        {
          configurarAutomatico(bomba2);
          Serial.println("Zona 2 em modo automatico");
        }
      }

      if(zona.equals("zona3"))
      {
        String zona3Mess = message.substring(indexZona+1);
        Serial.println("zona 3");
        Serial.println(zona3Mess);

        if(zona3Mess.equals("on"))
        {
          digitalWrite(bomba3, LOW);
        }

        if(zona3Mess.equals("off"))
        {
          digitalWrite(bomba3, HIGH);
        }

        if(zona3Mess.equals("manual"))
        {
          digitalWrite(bomba3, HIGH);
          Serial.println("Zona 3 em modo manual");
        }

        automaticoZona3 = zona3Mess.equals("automatico");
        if(automaticoZona3)
        {
          configurarAutomatico(bomba3);
          Serial.println("Zona 3 em modo automatico");
        }
      }

      if(zona.equals("zona4"))
      {
        String zona4Mess = message.substring(indexZona+1);
        Serial.println("zona 4");
        Serial.println(zona4Mess);

        if(zona4Mess.equals("on"))
        {
          digitalWrite(bomba4, LOW);
        }

        if(zona4Mess.equals("off"))
        {
          digitalWrite(bomba4, HIGH);
        }

        if(zona4Mess.equals("manual"))
        {
          digitalWrite(bomba4, HIGH);
          Serial.println("Zona 4 em modo manual");
        }
        automaticoZona4 = zona4Mess.equals("automatico");
        if(automaticoZona4)
        {
          configurarAutomatico(bomba4);
          Serial.println("Zona 4 em modo automatico");
        }
      }
    }

    // messageZona1 = [];
    countIndexZona1 = 0;
    // Check if the message is "getReadings"
    //if (strcmp((char*)data, "getReadings") == 0) {
      //if it is, send current sensor readings
      // String sensorReadings = getSensorReadings();
      // Serial.println(message);
      // notifyClients(sensorReadings);
    //}
  }
}
void checkConsumo()
{
  if(digitalRead(bomba1) == LOW)
  {
    consumoZona1 += 0.0333;
  }

  if(digitalRead(bomba2) == LOW)
  {
    consumoZona2 += 0.0333;
  }

  if(digitalRead(bomba3) == LOW)
  {
    consumoZona3 += 0.0333;
  }

  if(digitalRead(bomba4) == LOW)
  {
    consumoZona4 += 0.0333;
  }
}
void checkAutomatico(int z1, int z2, int z3, int z4)
{
  int valorComparadoMinimo = 30;
  int valorComparadoMaximo = 60;

  if(automaticoZona1)
  {
    Serial.println("Z1 CHECK DO AUTOMATICO");
    if(enchendoZona1)
    {
      if(z1 <= valorComparadoMinimo)
      {
        Serial.println("Z1 CHECK DO AUTOMATICO DESLIGADO");
        digitalWrite(bomba1, LOW);
        enchendoZona1 = false;
      }
    }    
    else
    {
      if(z1 >= valorComparadoMaximo)
      { 
        Serial.println("Z1 CHECK DO AUTOMATICO LIGADO");
        digitalWrite(bomba1, HIGH);
        enchendoZona1 = true;
      }
    }
  }

  if(automaticoZona2)
  {
    Serial.println("Z1 CHECK DO AUTOMATICO");
    if(enchendoZona2)
    {
      if(z2 <= valorComparadoMinimo)
      {
        Serial.println("Z2 CHECK DO AUTOMATICO LIGADO");
        digitalWrite(bomba2, LOW);
        enchendoZona2 = false;
      }
    }    
    else
    {
      if(z2 >= valorComparadoMaximo)
      { 
        Serial.println("Z2 CHECK DO AUTOMATICO DESLIGADO");
        digitalWrite(bomba2, HIGH);
        enchendoZona2 = true;
      }
    }
  }

  if(automaticoZona3)
  {
    Serial.println("Z3 CHECK DO AUTOMATICO");
    if(enchendoZona3)
    {
      if(z3 <= valorComparadoMinimo)
      {
        Serial.println("Z3 CHECK DO AUTOMATICO LIGADO");
        digitalWrite(bomba3, LOW);
        enchendoZona3 = false;
      }
    }    
    else
    {
      if(z3 >= valorComparadoMaximo)
      { 
        Serial.println("Z3 CHECK DO AUTOMATICO DESLIGADO");
        digitalWrite(bomba3, HIGH);
        enchendoZona3 = true;
      }
    }


  }

  if(automaticoZona4)
  {
    Serial.println("Z4 CHECK DO AUTOMATICO");
    if(enchendoZona4)
    {
      if(z4 <= valorComparadoMinimo)
      {
        Serial.println("Z4 CHECK DO AUTOMATICO LIGADO");
        digitalWrite(bomba4, LOW);
        enchendoZona4 = false;
      }
    }    
    else
    {
      if(z4 >= valorComparadoMaximo)
      { 
        Serial.println("Z4 CHECK DO AUTOMATICO DESLIGADO");
        digitalWrite(bomba4, HIGH);
        enchendoZona4 = true;
      }
    }
  }
}

void configurarAutomatico(uint8_t pin)
{
  digitalWrite(pin, HIGH);
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

void setup() {
  Serial.begin(115200);
  initWiFi();
  initLittleFS();
  initWebSocket();

  // Web Server Root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/index.html", "text/html");
  });

  server.on("/zona1", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("Request");
    request->redirect("https://www.google.com.br");
  });

  server.on("/zona2", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("ZONA 2");
    request->send(LittleFS, "/zona2.html", "text/html");
  });

  server.on("/zona3", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("ZONA 3");
    request->send(LittleFS, "/zona3.html", "text/html");
  });

  server.on("/zona4", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("ZONA 4");
    request->send(LittleFS, "/zona4.html", "text/html");
  });

  server.serveStatic("/", LittleFS, "/");

  // pinMode(relepin, OUTPUT);
  // pinMode(releBomba, OUTPUT);
  
  pinMode(bomba1, OUTPUT);
  pinMode(bomba2, OUTPUT);
  pinMode(bomba3, OUTPUT);
  pinMode(bomba4, OUTPUT);

  // pinMode(sensor3, INPUT);
  // pinMode(sensor4, INPUT);
  
  // pinMode(sensor3, INPUT);  
  // digitalWrite(relepin, HIGH);
  // digitalWrite(releBomba, HIGH);

  digitalWrite(bomba1, HIGH);
  digitalWrite(bomba2, HIGH);
  digitalWrite(bomba3, HIGH);
  digitalWrite(bomba4, HIGH);
  // Start server
  server.begin();

  //LOW - LIGA
  //HIGH - DESLIGA
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    String sensorReadings = getSensorReadings(); 
    //Serial.println(sensorReadings);
    notifyClients(sensorReadings);
    lastTime = millis();
  }
  ws.cleanupClients();
}