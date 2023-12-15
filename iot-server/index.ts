import mongo from "./db";
import express from "express";
import { Server } from "socket.io";
import cors from "cors";

const app = express();
const http = require("http");
app.use(cors());
const server = http.createServer(app);

server.listen(4000, () => {
  console.log("listening on *:4000");
});

const io = new Server(server, {
  cors: {
    origin: "*",
  },
});

// timer
const sleep = (ms: number) => {
  return new Promise((resolve) => setTimeout(resolve, ms));
};

const getReadingsSim = async () => {
  for (var i = 0; i < 500; i++) {
    const payload = {
      buttonPressed: i % 2 === 0,
      mac: "08:3A:8D:EE:E2:27",
      timestamp: "567037",
    };
    console.log(payload);
    io.to("/readings").emit(JSON.stringify(payload));
    await sleep(1000);
  }
};

const getReadings = async () => {
  const conn = await mongo();
  const res = await conn.db
    .collection("myCollection")
    .find()
    .sort({ timestamp: -1 })
    .limit(1)
    .toArray();
  return res;
};

io.on("connection", async (socket) => {
  // console.log('a user connected');
  socket.send("hello");
  const conn = await mongo();

   await getReadingsSim();

  // Open a Change Stream on the "haikus" collection
  const changeStream = conn.db.watch();
  // Print change events as they occur
  for await (const change of changeStream) {
    console.log("Received change:\n", change);
  }
});

const main = async () => {
  try {
    const conn = await mongo();
    const res = await conn.db
      .collection("myCollection")
      .find()
      .sort({ timestamp: -1 })
      .limit(1)
      .toArray();
    console.log(res);
  } catch (err) {
    console.log(err);
    console.log("Failed to connect");
  }
};

main();
