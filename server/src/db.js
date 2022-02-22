let sqlite3 = require('sqlite3').verbose();
let db = new sqlite3.Database(':memory:');

db.init = init;
db.insert = insert;
db.select = select;

async function init() {
    return new Promise((resolve, reject) => {
        db.serialize(() => {
            db.run("CREATE TABLE reading (time TEXT, value FLOAT, type TEXT, unit TEXT)", (err, res) => {
                err ? reject(err) : resolve(res);
            });
        });
    });
}

async function insert(readings) {
    let sql = "INSERT INTO reading VALUES ";
    let res = readings.reduce((prev, cur) => prev + "(" + `'${cur.timestamp}'` + "," + `'${cur.value}'` + "," + `'${cur.type}'` + "," + `'${cur.unit}'` + "),", "")
    res = res.substring(0, res.length - 1)
    return new Promise((resolve, reject) => {
        db.serialize(() => {
            db.run(sql + res, (err, res) => {
                err ? reject(err) : resolve(res);
            });
        });
    });
}

async function select(type) {
    return new Promise((resolve, reject) => {
        db.serialize(() => {
            db.all(`SELECT * FROM reading WHERE type = '${type}'`, (err, res) => {
                err ? reject(err) : resolve(res);
            });
        });
    });
}

module.exports = db;