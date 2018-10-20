#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "Game.h";

IPAddress apIP(10, 10, 10, 10);
const char* ssid = "ESP_luc_AP";
const char* pass = "123456789";
int nb_life = 1;
int nb_bullet = 1;

ESP8266WebServer server(80);
Game * game = new Game();
const int led = 13;
bool calculation = false;

void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/plain", "hello from esp8266!");
  digitalWrite(led, 0);
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  Serial.print("Setting soft-AP ... ");
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  boolean result = WiFi.softAP(ssid, pass);
  if(result == true)
  {
    Serial.println("Ready");
    server.begin();
    IPAddress ip = WiFi.softAPIP();
    Serial.print("connected to ");
    Serial.println(ip);
  }
  else
  {
    Serial.println("Failed!");
  }

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/joinGame", [](){
    server.sendHeader("Access-Control-Allow-Origin", "*");
    Serial.println("someone try to join a game");
    int oldId = server.arg(0).toInt();
    if(oldId != -1){
      int idPlayer = oldId;
      game->getPlayer(idPlayer)->resetPlayer(nb_life, nb_bullet);
      String res = String(nb_life) + '-' + String(nb_bullet) + '-' + String(idPlayer);
      Serial.println(res);
      server.send(200, "text/plain", res);
    } else {
      if(game->isFull() == false){
        int idPlayer = game->addPlayer(nb_life, nb_bullet);
        String res = String(nb_life) + '-' + String(nb_bullet) + '-' + String(idPlayer);
        Serial.println(res);
        server.send(200, "text/plain", res);
      }else{
        server.send(400, "text/plain", "no game available");
      }
    }
  });

  server.on("/gameStarted", [](){
    server.sendHeader("Access-Control-Allow-Origin", "*");
    if(game->isFull()){
      server.send(200, "text/plain", "1");
    }else{
      server.send(400, "text/plain", "0");
    }
  });

  server.on("/action", [](){
    server.sendHeader("Access-Control-Allow-Origin", "*");
    int idPlayer = server.arg(0).toInt();
    int action = server.arg(1).toInt();
    Serial.print("player");
    Serial.print(idPlayer+1);
    Serial.print(" fait action: ");
    Serial.print(action);
    Serial.print(" nombre de balle:");
    Serial.println(game->getPlayer(idPlayer)->getBullet());
    bool res = game->getPlayer(idPlayer)->setAction(action);
    if(res == false){
      Serial.println("action non autorisée");
      server.send(400, "text/plain", "0");
    }else{
      game->getPlayer((idPlayer + 1)%2)->setOtherAction(action);
      Serial.print("player");
      Serial.print(idPlayer + 1);
      Serial.print(" play ");
      Serial.println(game->getPlayer(idPlayer)->getAction());
      server.send(200, "text/plain", String(action));
    }
    if(game->turnReady()){
      calculation = true;
    }
  });

  server.on("/getResult", [](){
    server.sendHeader("Access-Control-Allow-Origin", "*");
    int idPlayer = server.arg(0).toInt();
    Serial.print("player");
    Serial.print(idPlayer + 1);
    Serial.print(" demande le resultat avec getresult=");
    Serial.println(game->getPlayer(idPlayer)->getresult);
    if(game->getPlayer(idPlayer)->getresult == true){
      int life = game->getPlayer(idPlayer)->getLife();
      int result = game->getPlayer(idPlayer)->getResult();
      int bullet = game->getPlayer(idPlayer)->getBullet();
      int otherAction = game->getPlayer(idPlayer)->getOtherAction();
      game->getPlayer(idPlayer)->getresult = false;
      game->getPlayer(idPlayer)->setAction(0);
      String res = String(life) + '-' + String(bullet) + '-' + String(result) + '-' + String(otherAction);
      Serial.println(res);
      server.send(200, "text/plain", res);
    }else{
  		server.send(400, "text/plain", "0");
  	}
    
  });

  server.on("/quit", [](){
    server.sendHeader("Access-Control-Allow-Origin", "*");
    int idPlayer = server.arg(0).toInt();
    Serial.print("player");
    Serial.print(idPlayer + 1);
    Serial.println(" quitte la partie");
    game->deletePlayer(idPlayer);
    server.send(200, "text/plain", "1");
  });
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  if(calculation) {
    game->calculate();
    game->getPlayer(0)->getresult = true;
    game->getPlayer(1)->getresult = true;
    Serial.println("______________________________________");
    Serial.println("calculation...");
    calculation = false;
  }
}
