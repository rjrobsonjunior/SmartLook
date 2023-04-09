import mysql from "mysql";

export const db = mysql.createConnection({
    host: "localhost",
    user: "root",
    password: "OficinadeIntegeracao1",
    database: "CRUD",
});