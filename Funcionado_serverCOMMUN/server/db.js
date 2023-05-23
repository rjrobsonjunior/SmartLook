const mysql =require('mysql');  

const db = mysql.createConnection({
    host: "localhost",
    user: "root",
    password: "OficinadeIntegeracao1",
    database: "CRUD",
});

module.exports = {
    db
}