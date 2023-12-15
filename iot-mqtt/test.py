import paho.mqtt.client as mqtt
import asyncio
import json
from pymongo import MongoClient
import time

config = {
    "mongo": {
        "url": "mongodb+srv://iot-node-red:simplepassword@iot.je5vjkn.mongodb.net/?retryWrites=true&w=majority",
        "dbName": "test",
        
    }
}

# mongo connection
client = MongoClient(config["mongo"]["url"])
db = client[config["mongo"]["dbName"]]
collection = db["myCollection"]



def parse_message(message):
    try:
        msg = json.loads(message)
        print(msg)
        return msg
    except json.JSONDecodeError as e:
        print(f"Error parsing message: {e}")
        return None

async def insert_one(msg):
    try:
        # client = MongoClient(config["mongo"]["url"])
        # db = client[config["mongo"]["dbName"]]

        collection.insert_one(msg)
     
        client.close()
        return msg
    except Exception as e:
        print(f"Could not insert into database: {e}")
        return False

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to MQTT")
        client.subscribe("seat_pad/data/json")
    else:
        print(f"Connection failed with code {rc}")

def on_message(client, userdata, msg):
    parsed_message = parse_message(msg.payload.decode())
    if parsed_message:
        print(parsed_message)
        try:
            asyncio.run(insert_one(parsed_message))
        except:
            return
        # insert_one(parsed_message)

def on_error(client, userdata, rc):
    print(f"Connection error: {rc}")
    client.loop_stop()

def main():
    protocol = "mqtt"
    mqtt_host = "localhost"
    mqtt_port = 1883

    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.on_error = on_error

    print(f"Connecting to {protocol}://{mqtt_host}:{mqtt_port}")
    client.connect(mqtt_host, mqtt_port, 60)

    client.loop_forever()

if __name__ == "__main__":
    main()
