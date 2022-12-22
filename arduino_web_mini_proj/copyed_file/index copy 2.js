const express = require("express");
const mysql = require("mysql");
const dbconfig = require("./config/database");
const bodyParser = require("body-parser");
const awsIoT = require("aws-iot-device-sdk");
const connection = mysql.createConnection(dbconfig);
const app = express();

var deviceConnectFlag = false;
const device = awsIoT.device({
  keyPath: "config/IoT_2016244084.private.key",
  certPath: "config/IoT_2016244084.cert.pem",
  caPath: "config/root-CA.crt",
  clientID: "IoT_2016244084",
  host: "a6og56iymx4d5-ats.iot.ap-northeast-1.amazonaws.com",
  keepalive: 10,
});

device.on("connect", (connack) => {
  console.log("Device Connect: ", connack);
  deviceConnectFlag = true;
  device.publish(
    "topic/A",
    JSON.stringify({
      data: "hello World",
    }),
    {
      qos: 1,
    }
  );
  device.subscribe("serverRequest");
  console.log("연결됨");
});
device.on("close", (err) => {
  console.log("Device Close: ", err);
  console.log(String(device.err));
  deviceConnectFlag = false;
});

device.on("reconnect", () => {
  console.log("Device Reconnect: ");
  deviceConnectFlag = true;
});
device.on("offline", () => {
  console.log("Device Offline: ");
  deviceConnectFlag = false;
});
device.on("error", (err) => {
  console.log("Device Error: ", err);
  deviceConnectFlag = false;
});
device.on("message", (topic, payload) => {
  console.log("Recived Topic " + topic);
  console.log("Received Message" + payload);
});
app.get("/api", (req, res) => {
  res.json({
    result: true,
  });
});

app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));

app.get("/api/status", (req, res) => {
  res.json({
    result: true,
    message: "Connect Success",
  });
});

app.get("/api/users", (req, res) => {
  connection.query("SELECT * FROM users", (err, rows) => {
    if (err) throw err;
    console.log(`User info is: ${rows}`, rows);
    res.json({
      result: true,
      data: rows,
    });
  });
});
app.get("/api/sensor", (req, res) => {
  connection.query("SELECT * FROM sensor_log", (err, rows) => {
    if (err) throw err;
    console.log(`sensor info is: ${rows}`, rows);
    res.json({
      result: true,
      data: rows,
    });
  });
});
app.listen(3000, () => {
  console.log("Run hello world on Port 3000");
  const i = 1;
});
app.post("/api/sensor_logs", (req, res) => {
  const temp = req.body.temp;
  const humidity = req.body.humidity;

  // connection.query(
  //   `INSERT INTO sensor_log (temp,humidity) VALUES (?,?)`,
  //   [temp, humidity],
  //   (err) => {
  //     if (err) throw err;
  //     res.json({
  //       result: true,
  //     });
  //   }
  // );
});
