const { EventEmitter } = require("events");
const { Server } = require("ws");
const express = require("express")
const cors = require("cors");
const db = require("./db");
let server = express();

server.use(cors());
db.init();
server.get("/getReadings",(req,res) => {
    let type = req.query.type;
    console.log(type);
    db.select(type).then(data => {
        res.send(data);
    });
});

server = server.listen(process.env.PORT || 3000, () => console.log(`Listening on ${process.env.PORT || 3000}`));
const ws = new Server({ server });
const bus = new EventEmitter();

const subscribers = [];
let type ;
let publisher = null;
let tempReadingNumber = 1;
let gasReadingNumber = 1;
bus.on("update", async (data) => {
    subscribers.forEach((sub) => {
        sub.send(data, { binary: false });
    })
    if(type === "temperature") {
        await db.insert([{readingNumber : tempReadingNumber,value : data,type : type,unit : "C"}]);
        tempReadingNumber++;
    }else {
        await db.insert([{readingNumber : gasReadingNumber,value : data,type : type,unit : "PPM"}]);
        gasReadingNumber++;
    } 
});
bus.on("command", (cmd) => {
    type = String(cmd);
    if (publisher) {
        publisher.send(cmd, { binary: false });
    }
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

