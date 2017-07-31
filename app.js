var express = require('express');
var app = express();
var http = require('http').Server(app);
var io = require('socket.io')(http);
var SerialPort = require('serialport');
var commander = require('commander');
var path = require('path');
var Readline = SerialPort.parsers.Readline;

const BAUDRATE = 115200;

app.use('/public', express.static('static'))

app.get("/", (req, res) => {
    res.sendFile(path.join(__dirname, "index.html"));
});

commander
    .version('0.0.1')
    .option('-t, --tcp-port', 'TCP port to start server on')
    .option('-p, --port <address>', 'Serial Port Path/Name')
    .option('-b, --baud [baudrate]', 'Baudrate to open port at [baudrate]', 115200)
    .parse(process.argv);

if (!commander.port) {
    console.error("Please specify a serial port with --port or use --demo mode");
    console.log("A list of available ports is provided below");
    SerialPort.list()
        .then(ports => {
            ports.forEach(function (port) {
                console.log("%s", port.comName);
                // TODO: Make this information print if it's relevant
                // console.log(port.pnpId);
                // console.log(port.manufacturer);
            });
            process.exit();
        })
        .catch(err => {
            console.log("Unable to enumerate serial ports");
            console.error(err);
            process.exit();
        });
} else {

    console.log("Using %s at %s ", commander.port, BAUDRATE);


    var port = new SerialPort(commander.port, {
        baudRate: BAUDRATE
    }, function (err) {
        if (err) {
            return console.log('Error: ', err.message);
        }
    });

    port.on('data', function (data) {
        console.log('Data:', data);
    });

    // Read data that is available but keep the stream from entering "flowing mode"
    port.on('readable', function () {
        console.log('Data:', port.read());
    });

    let currentlyConnected = false;

    io.on('disconned', () => {
        currentlyConnected = false;
    });

    io.on('connection', function (socket) {
        currentlyConnected = true;
        socket.on('command-radio', (commandPayload) => {
            console.log(commandPayload);
            const cmdString = commandPayload + "\n";
            if (port.isOpen) {
                console.log("Port is open!");
                port.write(cmdString);
            }
        });

    });





    http.listen(3003, function () {
        console.log('Web server running on 3003!')
    });
}
