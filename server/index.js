const express = require("express");
const userRoutes = require("./routes/users.js");
const SenhaRoutes = require("./routes/senha.js");
const RecognitionRoutes = require("./routes/recognition.js");
const presentRoutes = require("./routes/presents.js")
const cors = require("cors");
const app = express();

app.use(express.json());
app.use(cors());


app.use("/", userRoutes);
app.use("/", SenhaRoutes);
app.use("/", RecognitionRoutes);
app.use("/", presentRoutes);

app.listen(8800);