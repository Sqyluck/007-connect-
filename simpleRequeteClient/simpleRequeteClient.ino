#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

char ssid[] = "ESP_luc_AP";           // SSID of your AP
char pass[] = "123456789";         // password of your AP

int validation = 0;
int NbLife;
int NbBullet;
int result;
int gameStarted = 0;
int idUser=-1;
int timout = millis();
int otherResult = 0;

bool joinGame(){
  HTTPClient http;
  http.begin("http://10.10.10.10:80/joinGame?idUser=" + String(idUser));
  int httpCode = http.GET(); //Send the request
  if (httpCode == 200) { //Check the returning code
      String payload = http.getString();
      //Récupération et traitement des données
      NbLife = payload.substring(0,2).toInt();Serial.println(NbLife);
      NbBullet = payload.substring(2,3).toInt();Serial.println(NbBullet);
      idUser = payload.substring(4,5).toInt();Serial.println(idUser);
      http.end();
      return true;
    }
  http.end();
  return false;
}
    
bool started(){
    Serial.println("Préparation de la partie");
    while(gameStarted == 0){
    //On envoie la requete toutes les secondes*/
    if(millis() > timout + 1000){
      Serial.print(".");
      timout = millis();
      HTTPClient http;
      http.begin("http://10.10.10.10:80/gameStarted");
      int httpCode = http.GET(); //Send the request
      if (httpCode == 200) { //Check the returning code
        String payload = http.getString();
        //Récupération et traitement des données
        gameStarted = payload.toInt();
        Serial.println(gameStarted);
      }
      http.end();
      } 
    }
    Serial.println("Commencement");
    return true;
  }

  bool action(int choice){
    /*Choix entre 0, 1, 2 et 3*/
    if(choice < 4){
      HTTPClient http;
      Serial.println("Envoi de l'action");
      http.begin("http://10.10.10.10:80/action?id=" + String(idUser) + "&action=" + String(choice));
      int httpCode = http.GET(); //Send the request
      if (httpCode == 200) { //Check the returning code
        String payload = http.getString();
        //Récupération et traitement des données
        validation = payload.toInt();
        Serial.println(payload);
        }
        http.end(); 
      }
      if(validation != 0){
        bool reponse = false;
        Serial.println("Attente de résultat");
        while(!reponse){
          if(millis() > timout + 1000){
            timout = millis();
            Serial.print(".");
            HTTPClient http;
            http.begin("http://10.10.10.10:80/getResult?userName=" + String(idUser));
            int httpCode = http.GET(); //Send the request
            if (httpCode == 200) { //Check the returning code
              String payload = http.getString();
              //Récupération et traitement des données
              Serial.println(payload);
              NbLife = payload.substring(0,2).toInt();Serial.println(NbLife);
              NbBullet = payload.substring(2,3).toInt();Serial.println(NbBullet);
              result = payload.substring(4,5).toInt();Serial.println(result);
              otherResult = payload.substring(5,6).toInt();Serial.println(otherResult);
              reponse = true;
              }
            http.end();
            } 
          }
          Serial.println("Fin de l'action");
        }
        return true;  
    }

  bool quit(){
    HTTPClient http;
    http.begin("http://10.10.10.10:80/quit?id=" + String(idUser));
    int httpCode = http.GET(); //Send the request
    if (httpCode == 200) { //Check the returning code
      String payload = http.getString();
      //Récupération et traitement des données
      Serial.println(payload);
      http.end();
    return true;
    }
    http.end();
    return false;
  }
  
void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  Serial.println("Connexion à l'access point");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connecté mon gars !!");
  Serial.println(joinGame());
  started();
}

void loop() {}
