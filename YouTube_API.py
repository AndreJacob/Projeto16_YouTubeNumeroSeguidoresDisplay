import googleapiclient.discovery
import time
import paho.mqtt.client as mqtt

def get_channel_subscribers(channel_id, api_key):
    youtube = googleapiclient.discovery.build('youtube', 'v3', developerKey=api_key)
    request = youtube.channels().list(part="statistics", id=channel_id)
    response = request.execute()
    return response['items'][0]['statistics']['subscriberCount']

# Configurações MQTT
mqtt_broker = "10.0.0.97"  # Exemplo de broker MQTT
mqtt_port = 1885
mqtt_topic = "automacao84"

# Função para enviar a mensagem via MQTT
def send_mqtt_message(client, message):
    client.publish(mqtt_topic, message)

# Conectar ao broker MQTT
client = mqtt.Client()
client.connect(mqtt_broker, mqtt_port, 60)

# Substitua com o ID do canal e sua chave de API
channel_id = 'UCoyPRGrpP_G7i2sgEytbnBQ'  # Exemplo de canal
api_key = 'Sua chave api cadastrada no youtube'

while True:
    subscribers = get_channel_subscribers(channel_id, api_key)
    print(f'Número de inscritos: {subscribers}')
    
    # Inverter o número de inscritos e enviar via MQTT
    inverted_subscribers = str(subscribers)[::-1]  # Inverte a string
    send_mqtt_message(client, inverted_subscribers)
    
    time.sleep(0.5)  # Pausa de 2 segundos
