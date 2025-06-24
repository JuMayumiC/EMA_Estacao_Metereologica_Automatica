#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

// === Definições ===
#define DHTPIN 2
#define DHTTYPE DHT11
#define SUS 14
#define MQ_digital 5

DHT dht(DHTPIN, DHTTYPE);
ESP8266WebServer server(80);

// === Configuração Wi-Fi ===
const char* ssid = "Unexoffice_2G";
const char* password = "Celebrando2025";

// === Página principal (com AJAX) ===
void handleRoot() {
  String html = "<!DOCTYPE html><html lang=\"pt-br\"><head><meta charset=\"UTF-8\">";
  html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
  html += "<title>A EMPO</title>";
  html += "<style>";
  html += "body { background-color: #4B4B4B; color: white; font-family: Arial; text-align: center; }";
  html += ".dado { background-color: #4682B4; padding: 20px; margin: 20px; border-radius: 10px; }";
  html += "h1 { color: #70c673; }";
  html += "a.externo { text-decoration: none; color: #FFFFFF; }";
  html += "a.externo:hover { text-decoration: underline; color: #B3A0C5; }";
  html += "</style>";

  // Script AJAX para atualizar os dados
  html += "<script>";
  html += "function atualizar() {";
  html += "  var xhr = new XMLHttpRequest();";
  html += "  xhr.onreadystatechange = function() {";
  html += "    if (xhr.readyState == 4 && xhr.status == 200) {";
  html += "      var dados = JSON.parse(xhr.responseText);";
  html += "      document.getElementById('temp').innerHTML = dados.temperatura + '°C';";
  html += "      document.getElementById('umi').innerHTML = dados.umidade + '%';";
  html += "      document.getElementById('solo').innerHTML = dados.solo + '%';";
  html += "      document.getElementById('gas').innerHTML = (dados.gas == 0) ? 'GÁS DETECTADO !!!' : 'GÁS AUSENTE !!!';";
  html += "    }";
  html += "  };";
  html += "  xhr.open('GET', '/dados', true);";
  html += "  xhr.send();";
  html += "}";
  html += "setInterval(atualizar, 1000);";
  html += "window.onload = atualizar;";
  html += "</script>";

  html += "</head><body>";
  html += "<img src=\"https://github.com/JuMayumiC/EMAP-interface/blob/main/EMAP%20logotipo.png?raw=true\" alt=\"Logo da EMAP\" style=\"width:250px;height:auto;\">";
  html += "<h1>A Estação Meteorológica Portátil - A EMPO</h1>";
  html += "<div class='dado'><h2>Temperatura: <span id='temp'>---</span></h2></div>";
  html += "<div class='dado'><h2>Umidade: <span id='umi'>---</span></h2></div>";
  html += "<div class='dado'><h2>Umidade do Solo: <span id='solo'>---</span></h2></div>";
  html += "<div class='dado'><h2>Gás: <span id='gas'>---</span></h2></div>";
  html += "<footer><p>Desenvolvido por: <a href=\"https://github.com/JuMayumiC\" class=\"externo\">Mayumi</a></p></footer>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

// === Endpoint de dados JSON ===
void handleDados() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int ValDigitalIn = digitalRead(SUS);
  int PorcentoSolo = 100 - (ValDigitalIn * 100);
  int gas = digitalRead(MQ_digital);

  String json = "{";
  json += "\"temperatura\": " + String(t, 1) + ",";
  json += "\"umidade\": " + String(h, 1) + ",";
  json += "\"solo\": " + String(PorcentoSolo) + ",";
  json += "\"gas\": " + String(gas);
  json += "}";

  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(SUS, INPUT);
  pinMode(MQ_digital, INPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Conectado ao WiFi! IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/dados", handleDados);
  server.begin();
}

void loop() {
  server.handleClient();
}
