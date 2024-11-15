#include <WiFi.h>
#include <PubSubClient.h>
#include <MD_MAX72XX.h>
#include <SPI.h>

#define DATA_PIN   13     // Pino de dados (MOSI)
#define CLOCK_PIN  14     // Pino de clock (SCK)
#define LOAD_PIN   15     // Pino de carregamento (CS)
#define MAX_DEVICES 4     // Número de displays

MD_MAX72XX mx = MD_MAX72XX(MD_MAX72XX::FC16_HW, DATA_PIN, CLOCK_PIN, LOAD_PIN, MAX_DEVICES);

const char* ssid = "Cobertura";  // Rede WiFi
const char* password = "Dev@!2024"; // Senha WiFi
const char* mqtt_server = "10.0.0.97"; // Endereço do broker MQTT
const char* topic = "automacao84"; // Tópico MQTT

WiFiClient espClient;
PubSubClient client(espClient);
char message[4];  // Mensagem recebida via MQTT (3 caracteres + '\0')

void setup() {
  Serial.begin(115200);   // Inicializa a comunicação serial
  mx.begin();             // Inicializa o display
  mx.clear();             // Limpa o display

  connectWiFi();          // Conecta à rede WiFi
  client.setServer(mqtt_server, 1885);  // Configura o servidor MQTT
  client.setCallback(callback);  // Define a função de callback
  reconnectMQTT();        // Tenta reconectar ao broker MQTT
}

void connectWiFi() {
  WiFi.begin(ssid, password);  // Conecta à WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);  // Aguarda até conectar
    Serial.print(".");  // Exibe no serial enquanto aguarda
  }
  Serial.println("Conectado ao WiFi");
}

void reconnectMQTT() {
  while (!client.connected()) {
    if (client.connect("ESP32Client")) {
      Serial.println("Conectado ao MQTT");
      client.subscribe(topic);  // Se conectar, se inscreve no tópico
    } else {
      delay(1000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  memset(message, 0, sizeof(message));  // Limpa a variável da mensagem
  for (int i = 0; i < length && i < sizeof(message) - 1; i++) {
    message[i] = (char)payload[i];
  }
  Serial.print("Mensagem recebida: ");
  Serial.println(message);  // Exibe a mensagem na serial
}

void loop() {
  if (!client.connected()) {
    reconnectMQTT();  // Reconnecta ao MQTT se desconectado
  }
  client.loop();  // Processa as mensagens MQTT

  // Exibe os caracteres da mensagem recebida
  int len = strlen(message);
  for (int offset = -(len * 8); offset < 0; offset++) {
    mx.clear();  // Limpa o display a cada iteração

    for (int i = 0; i < len; i++) {
      mx.setChar(i * 8 - offset, message[i]);  // Exibe os números como caracteres
    }
    delay(150);  // Reduz o delay para acelerar a exibição
  }
}
