//Programa: Sensor de temperatura e umidade HDC1080 com Arduino
//Autor: Matheus Sena

#include <Wire.h>;
#include <ClosedCube_HDC1080.h>;
#include <LiquidCrystal_I2C.h>;
#include <ESP8266WiFi.h>;
ClosedCube_HDC1080 hdc1080;

String apiKey = "AACZG356LWB3HDKU"; // API DISPONIBILIZADA PELA THINGSPEAK
const char* ssid = "Casa 2.4g"; //SUA REDE WIFI
const char* password = "guilherme"; //SENHA DE SUA REDE
int duration=5;//DELAY ENTRE A MEDIÇÃO DE DATA E UPLOADING                
const char* server = "api.thingspeak.com"; //URL DA API   
WiFiClient client; //INICIALIZANDO CLIENT

//INICIALIZANDO LCD
LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7);

void setup(){
  Serial.begin(115200);
  //INICIALIZANDO HDC1080
  hdc1080.begin(0x40);
  
//INICIALIZANDO HDC1080
  WiFi.begin(ssid, password);
  Serial.println("Sensor HDC1080 com WemosD1");

  //Inicializa o display LCD I2C
  lcd.begin(16,2);  
  lcd.setBacklightPin(3, POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.home();

  //MOSTRA NO DISPLAY AS INFORMAÇÕES INICIAIS E A FORMATAÇÃO PADRÃO DE COMO OS DADOS SERÃO APRESENTADOS
  lcd.setCursor(0, 0);
  lcd.print("Temp.: XX.XX C");
  lcd.setCursor(0, 1);
  lcd.print("Umid.: YY.YY %");
  Serial.println();
  Serial.println();
  
  //VERIFICA CONEXÃO WIFI
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void loop(){
  //LEITURA DE DADOS DE TEMPERATURA E UMIDADE
  float t = hdc1080.readTemperature();
  float h = hdc1080.readHumidity();

  //VERIFICA SE DADOS FORAM LIDOS CO SUCESSO, CASO SIM, CONCATENA OS DADOS UMA STRING PARA ENVIAR AO CANAL DA THINGSPEAK
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from HDC1080 sensor!");
    delay(1000);
    return;
  }
  if (client.connect(server, 80)) {
    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(t);
    postStr += "&field2=";
    postStr += String(h);
    postStr += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);

    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" degrees Celcius Humidity: ");
    Serial.print(h);
    Serial.println("% send to Thingspeak");
  }
  client.stop();

  Serial.println("Waiting…");
  delay(duration*1000);
  //APRESENTA VALORES NO DISPLAY
  lcd.setCursor(7, 0);
  lcd.print(t);
  lcd.setCursor(7, 1);
  lcd.print(h);

}
