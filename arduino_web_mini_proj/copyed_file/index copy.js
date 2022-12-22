const express = require("express");
const res = require("express/lib/response");
const app = express();
const mysql = require("mysql");
const dbconfig = require("./config/database.js");

const awsIoT = require(`aws-iot-device-sdk`);

const connection = mysql.createConnection(dbconfig);
const bodyParser = require("body-parser");

//00000000000000000000000000000000000000000000000000000000000000000000

let deviceConnectFlag = false;
const device = awsIoT.device({
  keyPath: "config/IoT_2016244084.private.key",
  certPath: "config/IoT_2016244084.cert.pem",
  caPath: "config/root-CA.crt",
  clientID: "IoT_2016244084.",
  host: "a6og56iymx4d5-ats.iot.ap-northeast-1.amazonaws.com",
  keepalive: 10,
});

device.on("connect", (connack) => {
  console.log("Device Connect: ", connack);
  deviceConnectFlag = true;
  device.publish(
    "topic/A",
    JSON.stringify({
      data: "Hello World!",
    }),
    {
      qos: 1,
    }
  );
  device.subscribe("serverRequest");
  console.log("연결됨");
});

device.on("message", (topic, payload) => {
  console.log("Received Topic: " + topic);
  console.log("Received Message: " + payload);
});

device.on("close", (err) => {
  console.log("Device Close: " + err);
  deviceConnectFlag = false;
});

device.on("reconnect", () => {
  console.log("Device Reconnect");
  deviceConnectFlag = true;
});

device.on("offline", () => {
  console.log("Device Offline");
  deviceConnectFlag = false;
});

device.on("error", (err) => {
  console.log("Device Error: " + err);
  deviceConnectFlag = false;
});

//0000000000000000000000000000000000000000000000000000000000000000000000

app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));

//앱(웹페이지)이 실행되면 응답으로 헬로 월드를 보냄.
app.get("/api", (req, res) => {
  res.json({
    result: true,
  });
  console.log("api Run now");
});

//앱(웹페이지)이 실행되면 응답으로 커넥트 success를 보냄.
app.get("/api/status", (req, res) => {
  res.json({
    result: true,
    message: "Connect Success",
  });
});

app.get("/api/users", (req, res) => {
  connection.query("select * from users", (err, rows) => {
    if (err) throw err;
    res.json({
      result: true,
    });
  });
});

//sensor_log 데이터 로그 api(최신순서)
app.get("/api/sensor_log", (req, res) => {
  connection.query("select * from sensor_log ORDER BY id DESC", (err, rows) => {
    if (err) throw err;
    console.log("sensor_log Info is : ", rows);
    res.json({
      result: true,
      data: rows,
    });
  });
});

//sensor_log에 데이터 생성
app.post("/api/sensor_log", (req, res) => {
  const temp = req.body.temp;
  const humidity = req.body.humidity;

  connection.query(
    "insert into sensor_log (temp, humidity) values (?,?)",
    [temp, humidity],
    (err) => {
      if (err) throw err;
      res.json({
        result: true,
      });
    }
  );
});

//3000번 포트에 실행.
app.listen(3000, () => {
  console.log("Runnig Hello world on 3000 ports");
});
