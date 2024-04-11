import json
import mqtt

mqtt = mqtt.MqttClient()

with open("config.json", "r") as config_file:
    config = json.load(config_file)

    hostname = config["hostname"]
    username = config["username"]
    password = config["password"]
    mqtt.init(hostname, username, password)