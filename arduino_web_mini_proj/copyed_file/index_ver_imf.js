const express = require('express');
const mysql = require('mysql');
const dbconfig = require('./config/database.js');
const connection = mysql.createConnection(dbconfig);
const mqtt = require('mqtt');

const app = express();
const bodyParser = require('body-parser');

let deviceConnectFlag = false;
const device = mqtt.connect({
    host: "61.103.243.247",
    port: 1883,
    protocol: 'mqtt',
    clientId: "20221020"
})

device.on('connect', (connack) => {
    console.log('Device Connect: ', connack);
    deviceConnectFlag = true;
    device.publish('topic/A', JSON.stringify({
        data: "Hello World!"
    }), {
        qos: 1
    });
    device.subscribe('serverRequest');
});

device.on('message', (topic, payload) => {
    console.log("Received Topic: " + topic);
    console.log("Received Message: " + payload);
});

device.on('close', (err) => {
    console.log('Device Close: ' + err);
    deviceConnectFlag = false;
});

device.on('reconnect', () => {
    console.log('Device Reconnect');
    deviceConnectFlag = true;
});

device.on('offline', () => {
    console.log('Device Offline');
    deviceConnectFlag = false;
});

device.on('error', (err) => {
    console.log('Device Error: ' + err);
    deviceConnectFlag = false;
});

app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));

app.get('/api', (req, res) => {
    res.json({
        result: true
    })
});

app.get('/api/status', (req, res) => {
    res.json({
        result: true,
        message: "Connect Success"
    });
});

app.get('/api/users', (req, res) => {

    connection.query('SELECT * FROM users', (err, rows) => {
        if (err) throw err;
        console.log("User Info is: ", rows);
        res.json({
            result: true,
            data: rows
        });
    });

});

app.get('/api/sensor_logs', (req, res) => {

    connection.query('SELECT * FROM sensor_log ORDER BY id DESC', (err, rows) => {
        if (err) throw err;
        console.log("Sensor Log is: ", rows);
        res.json({
            result: true,
            data: rows
        })
    });

});

app.post('/api/sensor_logs', (req, res) => {
    console.log(req.body);
    const temp = req.body.temp;
    const humidity = req.body.humidity;

    connection.query('INSERT INTO sensor_log (temp, humidity) VALUES (?,?)', [temp, humidity], (err) => {
        if (err) {
            res.statusCode = 500;
            res.json({
                result: false,
                message: err.message
            });
        } else {
            res.json({
                result: true,
                message: "Success"
            })
        }
    })
})

app.listen(3000, () => {
    console.log("Run Hello World on Port 3000");
});