import mqtt from "mqtt";
// import mongo from "./db";
import { sleep } from "bun";
import { Db, MongoClient } from "mongodb";

const config = {
  mongo: {
    url: "mongodb://iot-node-red:simplepassword@ac-hgit8ry-shard-00-00.je5vjkn.mongodb.net:27017,ac-hgit8ry-shard-00-01.je5vjkn.mongodb.net:27017,ac-hgit8ry-shard-00-02.je5vjkn.mongodb.net:27017/?ssl=true&replicaSet=atlas-7ghq0l-shard-0&authSource=admin&retryWrites=true&w=majority",
    dbName: "test",
  },
};

const parseMessage = (message: any) => {
  try {
    const msg = JSON.parse(message);
    console.log(msg);
    return msg;
  } catch (err) {
    return null;
  }
};

const insertOne = async (msg: any) => {
  try {
    const client = await MongoClient.connect(config.mongo.url);
    const c = await client.db(config.mongo.dbName);
    c.collection("myCollection").insertOne(msg);
    return msg;
  } catch (err) {
    console.log("Couldnt insert into database");
    return false;
  }
};

const main = async () => {
  const protocol = "mqtt";
  const mqtt_host = "172.20.10.3";
  const mqtt_port = 1883;
  const mqtt_topic = "seat_pad/data/json";
  const url = `${protocol}://${mqtt_host}:${mqtt_port}`;
  console.log("Connecting to MQTT");
  const client = mqtt.connect(url);

  // const conn = mongo();

  console.log("url", url);
  client.on("connect", () => {
    client.subscribe(mqtt_topic, (err) => {
      if (!err) {
        console.log("connected");
      } else {
        console.log("error", err);
      }
    });
  });

  client.on("message", (topic, message) => {
    const parsedMessage = parseMessage(message.toString());
    if (parsedMessage) {
      console.log(parsedMessage);
      insertOne(parseMessage);
    }
  });

  client.on("error", (err) => {
    console.error(`Connection error: ${err.message}`);
    console.error(err);
    client.end();
  });

  while (true) {
    await sleep(200);
  }
};

main();
