import paho.mqtt.client as mqtt

# Callback para o evento de conexão
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Conectado ao broker MQTT!")
    else:
        print("Falha na conexão. Código de retorno:", rc)

# Callback para o evento de publicação
def on_publish(client, userdata, mid):
    print("Mensagem publicada com sucesso!")

# Configurações do broker MQTT
broker = "mqtt://164.41.98.25"
porta = 443
cliente_id = "SU6LjX84paNYZPKyWeij"

# Criação do cliente MQTT
client = mqtt.Client(cliente_id)

# Configuração das callbacks
client.on_connect = on_connect
client.on_publish = on_publish

# Conexão ao broker MQTT
client.connect(broker, porta)

# Loop para manter a conexão e processar as mensagens
client.loop_start()

# Publicação da mensagem
topico = "v1/devices/me/telemetry"
mensagem = "mundo"
client.publish(topico, mensagem)

# Aguarda um tempo para a mensagem ser publicada
client.loop_stop()
client.disconnect()