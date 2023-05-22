/*
import mysql from "mysql";
export const db = mysql.createConnection({
    host: "localhost",
    user: "root",
    password: "12345",
    database: "tabela_teste",
});
*/

const mysql = require("mysql");

const db = mysql.createConnection({
    host: "localhost",
    user: "root",
    password: "12345",
    database: "tabela_teste",
});

module.exports = {
    db
};
