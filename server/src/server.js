const { EventEmitter } = require("events");
const { Server } = require("ws");
const express = require("express")
const server = express()
    .listen(process.env.PORT || 3000, () => console.log(`Listening on ${process.env.PORT || 3000}`));

const ws = new Server({ server });
const bus = new EventEmitter();

const subscribers = [];
let publisher = null;

bus.on("update", (data) => {
    subscribers.forEach((sub) => {
        sub.send(data, { binary: false });
    })
});
bus.on("command", (cmd) => {
    publisher.send(cmd, { binary: false });
});

ws.on("connection", (socket, req) => {
    switch (req.url) {
        case "/sensor":
            publisher = socket;
            socket.on("message", (msg) => {
                bus.emit("update", msg);
            });
            break;
        case "/client":
            subscribers.push(socket);
            socket.on("message", (cmd) => {
                bus.emit("command", cmd);
            });
            break
        default:
            break;
    }
});
