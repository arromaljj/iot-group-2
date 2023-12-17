// import logo from "./logo.svg";
// import "./App.css";

import { f } from "nextra/dist/types-c8e621b7";
import { useEffect, useRef, useState } from "react";
import io from "socket.io-client";
import mqtt from "mqtt";

// Write a function that will give me the range of numbers from start to end
const range = (start, end) => {
  const arr = [];
  for (let i = start; i <= end; i++) {
    arr.push(i);
  }
  return arr;
};

const data = [
  { active: 0, id: 0 },
  { active: 1, id: 1 },
  { active: -1, id: 2 },
  { active: -1, id: 3 },
  { active: -1, id: 4 },
  { active: -1, id: 5 },
  { active: -1, id: 6 },
  { active: -1, id: 7 },
  { active: -1, id: 8 },
  { active: -1, id: 9 },
];

const Seat = ({ active }) => {
  return (
    <div
      className={`w-10 h-10 border rounded border-gray-400 flex justify-center items-center shadow shadow-lg ${
        active == 1 ? "bg-red-600" : active == 0 ? "bg-gray-200" : "bg-gray-700"
      }`}
    ></div>
  );
};

const useMessages = () => {
  const mqttHost =
    "wss://4d94fb43621849c4aa17a09c9f754df6.s2.eu.hivemq.cloud:8884/mqtt";
  const username = "Arromal123";
  const password = "Arromal123";
  const _seats = range(0, 9).map((_, idx) => {
    return { active: -1, id: idx, mac: null };
  });
  const [seats, setSeats] = useState(_seats);
  const stateRef = useRef({ seats, setSeats });

  useEffect(() => {
    stateRef.current = { seats, setSeats: setSeats };
  }, [seats, setSeats]);

  useEffect(() => {
    const client = mqtt.connect(mqttHost, { username, password });
    client.on("connect", () => {
      console.log("Connected to HiveMQ MQTT Broker");
      client.subscribe("seat_pad/data/json", (err) => {
        if (!err) {
          console.log("Successfully subscribed to the topic");
        }
      });
    });

    client.on("message", (topic, message) => {
      const res = JSON.parse(message.toString());
      console.log("Received message:", res);

      setSeats((prevSeats) => {
        if (prevSeats.filter((s) => s.mac === res.mac).length === 0) {
          // If the seat is not in the list, add it
          const emptyId = prevSeats.filter((s) => s.active === -1)[0].id;
          const newSeats = prevSeats.map((s) =>
            s.id === emptyId
              ? { ...s, active: res.limitSwitchActive ? 1 : 0, mac: res.mac }
              : s
          );
          return newSeats;
        } else {
          // If the seat is in the list, update it
          const newSeats = prevSeats.map((s) =>
            s.mac === res.mac
              ? { ...s, active: res.limitSwitchActive ? 1 : 0 }
              : s
          );
          return newSeats;
        }
      });
    });

    // client.on("message", (topic, message) => {
    //   // message is a buffer
    //   const res = JSON.parse(message.toString());
    //   console.log(res);
    //   if (res && Object.keys(res).length > 1) {
    //     const mac = res.mac;
    //     const isActive = res.limitSwitchActive;
    //     const seatsRef = stateRef.current.seats;
    //     const setSeatsRef = stateRef.current.setSeats;

    //     console.log("Received message", seatsRef);
    //     if (seatsRef.filter((s) => s.mac === mac).length === 0) {
    //       const emptyId = seatsRef.filter((s) => s.active === -1)[0].id;
    //       const newSeats = seatsRef.map((s) =>
    //         s.id === emptyId ? { ...s, active: isActive, mac } : s
    //       );
    //       setSeats(newSeats);
    //     } else {
    //       console.log("CAME HERE");
    //       const newSeats = seatsRef.map((s) =>
    //         s.mac === mac ? { ...s, active: isActive } : s
    //       );
    //       console.log("NEW SEATS", newSeats);
    //       setSeats([...newSeats]);
    //     }
    //   }

    //   console.log("Seats", seats);
    //   return seats;
    // });

    return () => {
      client.end(); // Disconnect client when component unmounts
    };
  }, []);
  console.log("Seats", seats);
  return seats;
};

function App() {
  const seats = useMessages();
  const [issleep, setIssleep] = useState(false);
  const styles = !issleep
    ? "hover:bg-blue-500 hover:text-white  border-blue-400"
    : "hover:bg-red-500 hover:text-white  border-red-400";

  console.log("Seats in app", seats);
  return (
    <div className="w-full flex flex-col mt-6 font-bold ">
      <header className="w-full flex ">Bus Tracker</header>

      <div className="w-fit grid grid-cols-3 gap-4 mt-12">
        {seats.map((_, idx) => (
          <Seat active={seats[idx].active} key={idx} />
        ))}
      </div>
      {/* active == 1 ? "bg-red-600" : active == 0 ? "bg-gray-200" : "bg-gray-700" */}
      <div className="mt-5">
        <div className="flex flex-row items-center">
          <div className="w-3 h-3 bg-red-600 rounded-sm"></div>
          <div className="ml-2 text-sm  p-0">Occupied</div>
        </div>
        <div className="flex flex-row items-center">
          <div className="w-3 h-3 bg-gray-200 rounded-sm"></div>
          <div className="ml-2 text-sm  p-0">Free</div>
        </div>
        <div className="flex flex-row items-center">
          <div className="w-3 h-3 bg-gray-700 rounded-sm"></div>
          <div className="ml-2 text-sm  p-0">No Data</div>
        </div>
      </div>
      <div className="w-32 h-10 mt-10 ">
        <button
          onClick={() => setIssleep(!issleep)}
          className={`p-2 border ${styles} rounded-md text-xs`}
        >
          {!issleep ? "Enter Deep Sleep" : "Exit Deep Sleep"}
        </button>
      </div>
    </div>
  );
}

export default App;
