# import paho.mqtt.client as mqtt
# from paho import mqtt as paho_mqtt
# import asyncio
# import json
# from pymongo import MongoClient
# import uvicorn
# import time
# import socketio
# import certifi
# from fastapi import FastAPI
# from fastapi.responses import HTMLResponse
# from starlette.middleware.cors import CORSMiddleware

# config = {
#     "mongo": {
#         "url": "mongodb+srv://iot-node-red:simplepassword@iot.je5vjkn.mongodb.net/?retryWrites=true&w=majority",
#         "dbName": "test",
        
#     },
#     "socket": {
#         "host": "127.0.0.1",
#         "port": 5007,  # Change this to your desired port
#         "endpoint": "/data"  # Change this to your desired endpoint
#     }
# }

# # mongo connection

# # sio = socketio.AsyncServer(async_mode="asgi")
# # app = socketio.ASGIApp(sio, static_files={})







# # async def insert_one(msg):
# #     try:
# #         # client = MongoClient(config["mongo"]["url"])
# #         # db = client[config["mongo"]["dbName"]]

# #         collection.insert_one(msg)
        
# #         client.close()
# #         return msg
# #     except Exception as e:
# #         print(f"Could not insert into database: {e}")
# #         return False



# class Server:
#     def __init__(self, host, port):
#         self.host = host
#         self.port = port
#         self.app = FastAPI()
#         self.sio = socketio.AsyncServer(async_mode="asgi", cors_allowed_origins="*")
#         self.app.mount("/", socketio.ASGIApp(self.sio))
#         self.setup_listeners()

#     def run(self):
#         uvicorn.run(self.app, host=self.host, port=self.port)


#     def setup_listeners(self):
#         self.sio.on("connect", self.handle_connect)
#         self.app.get("/", response_class=HTMLResponse)(self.handle_get)

#     async def handle_get(self):
#         return HTMLResponse("Hello from FastAPI")
    

#     # def setup_listeners(self):
#     #     self.sio.on("connect", self.handle_connect)
#     #     # Add a normal get request handler here
#     #     # self.app.get("/", self.handle_get)
#     #     self.app.get("/", response_class=HTMLResponse)(self.handle_get)
#     #     # @self.sio.on("/")
#     #     # def handle_mqtt_data(sid, msg):
#     #     #     print(msg)
#     #     #     return HTMLResponse("Hello")

#     # async def handle_get(self, sid, msg):
#     #     print("HELLOO")
#     #     return HTMLResponse("Hello")

#     async def handle_connect(self, sid, environ):
#         await self.sio.emit("connected", "Socket.IO connected", room=sid)




# class DB:
#     def __init__(self):
#         self.client = MongoClient(config["mongo"]["url"], tlsCAFile=certifi.where())  # Path to the CA bundle)
#         self.db = self.client[config["mongo"]["dbName"]]
#         self.collection = self.db["myCollection"]
    
#     async def insert_one(self,msg):
#         try:
#             self.collection.insert_one(msg)
#             return msg
#         except Exception as e:
#             print(f"Could not insert into database: {e}")
#             return False



# class MQTTPublish:
#     def __init__(self,  db: DB):
#         self.success = False
#         self.protocol = "mqtt"
#         self.mqtt_host = "4d94fb43621849c4aa17a09c9f754df6.s2.eu.hivemq.cloud"
#         self.mqtt_port = 8883
#         self.tls_uri = "4d94fb43621849c4aa17a09c9f754df6.s2.eu.hivemq.cloud:8883/mqtt"
#         self.websocket_uri = "4d94fb43621849c4aa17a09c9f754df6.s2.eu.hivemq.cloud:8884/mqtt"
#         self.username = "Arromal123"
#         self.password = "Arromal123"

#         self.db = db
#         self.client = None

    
    
#     def setup(self):
#         try:
#             print("Attempting publish connection")
#             self.client = mqtt.Client(client_id="", userdata=None, protocol=mqtt.MQTTv31)
#             self.client.username_pw_set(self.username, self.password)
#             self.client.tls_set(ca_certs=certifi.where(),tls_version=paho_mqtt.client.ssl.PROTOCOL_TLSv1_2)
#             self.client.connect(self.mqtt_host, self.mqtt_port, 60)
#             self.client.on_connect = self.on_connect
#             self.client.on_message = self.on_message
#             self.client.on_error = self.on_error
#             print("Publish connected")
#             self.success = True
#         except Exception as e:
#             print("Publish error", e)
#             self.success = False
#         return 
        
            
#     async def publish(self, topic, msg):
#         if self.success:
#             try:
#                 self.client.publish(topic, msg)
#             except Exception as e:
#                 print("Publish failed ", e)

#     def on_connect(self, client, userdata, flags, rc):
#         if rc == 0:
#             print("Connected to MQTT")
#             client.subscribe("seat_pad/data/json")
#         else:
#             print(f"Connection failed with code {rc}")


#     def parse_message(self,message):
#         try:
#             msg = json.loads(message)
#             # if 'mac' in msg:
#             #     msg['mac'] = "".join(msg['mac'].split(":"))
#             return msg
#         except json.JSONDecodeError as e:
#             print(f"Error parsing message: {e}")
#             return None



#     def on_message(self, client, userdata, msg):
#         try:
#             parsed_message = self.parse_message(msg.payload.decode())
#             if parsed_message:
#                 print(parsed_message)
#                 msg_json = json.dumps(parsed_message)
#                 asyncio.run(self.db.insert_one(parsed_message))
#                 # asyncio.run( self.server.sio.emit("mqtt_data", str(msg_json)))
#                 asyncio.run(self.mqtt_pub.publish("Test", str(msg_json)))
#         except Exception as e:
#             print("Message handling failed", e)
#             # insert_one(parsed_message)

#     def on_error(self,client, userdata, rc):
#         print(f"Connection error: {rc}")
#         client.loop_stop()


        

#     def on_connect(self, client, userdata, flags, a,  rc):
#         if rc == 0:
#             print("Connected to MQTT")
#             client.subscribe("seat_pad/data/json")
#         else:
#             print(f"Connection failed with code {rc}")


#     def parse_message(self,message):
#         try:
#             msg = json.loads(message)
#             # if 'mac' in msg:
#             #     msg['mac'] = "".join(msg['mac'].split(":"))
#             return msg
#         except json.JSONDecodeError as e:
#             print(f"Error parsing message: {e}")
#             return None



#     def on_message(self, client, userdata, msg):
#         try:
#             parsed_message = self.parse_message(msg.payload.decode())
#             if parsed_message:
#                 print(parsed_message)
#                 msg_json = json.dumps(parsed_message)
#                 asyncio.run(self.db.insert_one(parsed_message))
#                 # asyncio.run( self.server.sio.emit("mqtt_data", str(msg_json)))
#                 asyncio.run(self.mqtt_pub.publish("Test", str(msg_json)))
#         except Exception as e:
#             print("Message handling failed", e)
#             # insert_one(parsed_message)

#     def on_error(self,client, userdata, rc):
#         print(f"Connection error: {rc}")
#         client.loop_stop()




# # class MQTTSubscribe:
# #     def __init__(self, db: DB, mqtt_pub: MQTTPublish):
# #         self.success = False
# #         self.protocol = "mqtt"
# #         self.mqtt_host = "4d94fb43621849c4aa17a09c9f754df6.s2.eu.hivemq.cloud"
# #         self.mqtt_port = 8883
# #         self.tls_uri = "4d94fb43621849c4aa17a09c9f754df6.s2.eu.hivemq.cloud:8883/mqtt"
# #         self.websocket_uri = "4d94fb43621849c4aa17a09c9f754df6.s2.eu.hivemq.cloud:8884/mqtt"
# #         self.username = "Arromal123"
# #         self.password = "Arromal123"

# #         self.db = db
# #         self.mqtt_pub = mqtt_pub
# #         self.db = db

    
# #     def setup(self):
# #         try:
# #             print("Attempting Connection")
# #             self.client = mqtt.Client(client_id="", userdata=None, protocol=mqtt.MQTTv5)
# #             self.client.username_pw_set(self.username, self.password)
# #             self.client.tls_set(ca_certs=certifi.where(),tls_version=paho_mqtt.client.ssl.PROTOCOL_TLSv1_2)
# #             self.client.connect(self.mqtt_host, self.mqtt_port, 60)
# #             self.client.on_connect = self.on_connect
# #             self.client.on_message = self.on_message
# #             self.client.on_error = self.on_error
# #             print("Connected")
     
# #             self.success = True
# #         except Exception as e:
# #             print("Connection error", e)
# #             self.success = False
# #         return 
        
            
        

# #     def on_connect(self, client, userdata, flags, a,  rc):
# #         if rc == 0:
# #             print("Connected to MQTT")
# #             client.subscribe("seat_pad/data/json")
# #         else:
# #             print(f"Connection failed with code {rc}")


# #     def parse_message(self,message):
# #         try:
# #             msg = json.loads(message)
# #             # if 'mac' in msg:
# #             #     msg['mac'] = "".join(msg['mac'].split(":"))
# #             return msg
# #         except json.JSONDecodeError as e:
# #             print(f"Error parsing message: {e}")
# #             return None



# #     def on_message(self, client, userdata, msg):
# #         try:
# #             parsed_message = self.parse_message(msg.payload.decode())
# #             if parsed_message:
# #                 print(parsed_message)
# #                 msg_json = json.dumps(parsed_message)
# #                 asyncio.run(self.db.insert_one(parsed_message))
# #                 # asyncio.run( self.server.sio.emit("mqtt_data", str(msg_json)))
# #                 asyncio.run(self.mqtt_pub.publish("Test", str(msg_json)))
# #         except Exception as e:
# #             print("Message handling failed", e)
# #             # insert_one(parsed_message)

# #     def on_error(self,client, userdata, rc):
# #         print(f"Connection error: {rc}")
# #         client.loop_stop()
















# async def main():
#     db = DB()
#     # server = Server("127.0.0.1", 5007)

#     mqtt_pub = MQTTPublish( db)
#     mqtt_pub.setup()
#     # mqtt_sub = MQTTSubscribe( db, mqtt_pub)

#     # mqtt_sub.setup()
#     mqtt_pub.client.loop_forever()
#     # mqtt_pub.publish()
#     # while not mqtt_sub.success:
#     #     print(mqtt_sub.success)
#     #     mqtt_sub.setup()
#     #     time.sleep(2)
#     #     print("Attempting again")

#     # server.run()



    


# if __name__ == "__main__":
#     asyncio.run(main())


