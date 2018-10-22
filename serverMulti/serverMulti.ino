#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "Game.h";

IPAddress apIP(10, 10, 10, 10);
const char* ssid = "ESP_luc_AP";
const char* pass = "123456789";

// variables d'initialisation des parties
int nb_life = 1;
int nb_bullet = 1;

ESP8266WebServer server(80);
// tableau stockant les parties
Game * game[4];
bool calculation[4] = { false, false, false, false };

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

//requete pour rejoindre une partie
  server.on("/joinGame", [](){
    server.sendHeader("Access-Control-Allow-Origin", "*");
    //on récupère les anciennes valeur de l'utilisateur s'il a deja jouer
    int oldId = server.arg(0).toInt();
    int oldGame = server.arg(1).toInt();
    if(oldGame != -1){ // si l'utilisateur recommence une partie
	  int idGame = oldGame;
      if(oldId != -1){ //on remet les variable du client aux valeurs initiales et on lui renvoie
        int idPlayer = oldId;
        game[idGame]->getPlayer(idPlayer)->resetPlayer(nb_life, nb_bullet);
        String res = String(nb_life) + '-' + String(nb_bullet) + '-' + String(idPlayer) + '-' + String(idGame);
		    Serial.print("restart game : ");
        Serial.println(res);
        server.send(200, "text/plain", res);
      }  
	}
	else {
		int idGame = findGame(); //sinon on  chercher un partie
		if (idGame != -1) {
			int idPlayer = game[idGame]->addPlayer(nb_life, nb_bullet); //on attribut un id au client et on lui envoie ces valeurs initiales
			String res = String(nb_life) + '-' + String(nb_bullet) + '-' + String(idPlayer) + '-' + String(idGame);
			Serial.print("new game : ");
			Serial.println(res);
			server.send(200, "text/plain", res);
		}
		else {
			Serial.println("no game available");
			server.send(400, "text/plain", "0");
		}
	}

  });

  server.on("/gameStarted", [](){
    server.sendHeader("Access-Control-Allow-Origin", "*");
    int idGame = server.arg(0).toInt();
    if(game[idGame]->isFull()){ // si la partie est pleine
      if(game[idGame]->isReady()){ // et que les des joueurs sont prêt, on autorise les clients à commencer la partie
        server.send(200, "text/plain", "1");
      }else{
        server.send(400, "text/plain", "0");
      }
    } else {
        server.send(400, "text/plain", "0");
    }
  });

  server.on("/action", [](){
    server.sendHeader("Access-Control-Allow-Origin", "*");
    int idPlayer = server.arg(0).toInt();
    int action = server.arg(1).toInt();
  	int idGame = server.arg(2).toInt();
    Serial.print("player");
    Serial.print(idPlayer+1);
  	Serial.print(" dans game: ");
  	Serial.print(idGame);
    Serial.print(" fait action: ");
    Serial.print(action);
    Serial.print(" nombre de balle:");
    Serial.println(game[idGame]->getPlayer(idPlayer)->getBullet());
    bool res = game[idGame]->getPlayer(idPlayer)->setAction(action); // on verifie que l'action est possible
    if(res == false){
      Serial.println("action non autorisée");
      server.send(400, "text/plain", "0");
    }else{
      game[idGame]->getPlayer((idPlayer + 1)%2)->setOtherAction(action);
      Serial.print("player");
      Serial.print(idPlayer + 1);
      Serial.print(" play ");
      Serial.println(game[idGame]->getPlayer(idPlayer)->getAction());
      server.send(200, "text/plain", String(action));
    }
    if(game[idGame]->turnReady()){ // si les deux joueuurs ont joué, on autorise le calcul du résultat
      calculation[idGame] = true;
    }
  });

  server.on("/getResult", [](){
    server.sendHeader("Access-Control-Allow-Origin", "*");
    int idPlayer = server.arg(0).toInt();
  	int idGame = server.arg(1).toInt();
    Serial.print("player");
    Serial.print(idPlayer + 1);
	  Serial.print(" dans game: ");
	  Serial.print(idGame);
    Serial.print(" demande le resultat avec getresult=");
    Serial.println(game[idGame]->getPlayer(idPlayer)->getresult);
    if(game[idGame]->getPlayer(idPlayer)->getresult == true){ // si le calcul a été effectué pour le client, on lui renvoie les résultat et l'action de l'autre client (nb_life-nb_bullet-result-otheraction)
      int life = game[idGame]->getPlayer(idPlayer)->getLife();
      int result = game[idGame]->getPlayer(idPlayer)->getResult();
      int bullet = game[idGame]->getPlayer(idPlayer)->getBullet();
      int otherAction = game[idGame]->getPlayer(idPlayer)->getOtherAction();
      game[idGame]->getPlayer(idPlayer)->getresult = false;
      game[idGame]->getPlayer(idPlayer)->setAction(0);
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
	  int idGame = server.arg(1).toInt();
    Serial.print("player");
    Serial.print(idPlayer + 1);
    Serial.println(" quitte la partie");
    game[idGame]->deletePlayer(idPlayer); // si le joueur quitte la partie, on le supprime
	  if (game[idGame]->isEmpty() == true) { // si la partie est vide, on supprime la partie
  		delete game[idGame];
  		game[idGame] = nullptr;
  		Serial.print("game : ");
  		Serial.print(idGame);
  		Serial.println("supprimée");
  	}
    server.send(200, "text/plain", "1");
  });
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  for (int i = 0; i < 4; i++) {
	  if (calculation[i]) { // quand le calcul est possible, on calcul le resultat de la partie et on autorise les utilisateurs à les lire
		  game[i]->calculate();
		  game[i]->getPlayer(0)->getresult = true;
		  game[i]->getPlayer(1)->getresult = true;
		  Serial.println("______________________________________");
		  Serial.print("calculation game ");
		  Serial.println(i);
		  calculation[i] = false;
	  }
  }

}

int findGame() { //recherche une game non pleine ou en créé une 
	for (int i = 0; i < 4; i++) {
		if (game[i] != nullptr) {
			if (game[i]->isFull() == false) {
				return i;
			}
		}
		else {
      game[i] = new Game();
			return i;
		}
	}
	return -1;
}
