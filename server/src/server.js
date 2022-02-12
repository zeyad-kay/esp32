const { EventEmitter } = require("events");
const { WebSocketServer } = require("ws");

const ws = new WebSocketServer({ port: 3000 });
const bus = new EventEmitter();

const subscribers = [];
let publisher = null;

bus.on("update", (data) => {
    subscribers.forEach((sub) => {
        sub.send(data);
    })
});
bus.on("command", (cmd) => {
    publisher.send(cmd);
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
