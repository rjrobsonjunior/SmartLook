const express = require("express");
const userRoutes = require("./routes/users.js");
const SenhaRoutes = require("./routes/senha.js");
const RecognitionRoutes = require("./routes/recognition.js");
const presentRoutes = require("./routes/presents.js");
const exitRoutes = require("./routes/exit.js");
const QRcodeRoutes = require("./routes/qrcode.js");
const uploadRoutes = require("./routes/upload.js");
const cors = require("cors");
const app = express();

app.use(express.json());
app.use(express.static('./routes/public'));

app.use(cors());

app.use("/", userRoutes);
app.use("/", SenhaRoutes);
app.use("/", RecognitionRoutes);
app.use("/", presentRoutes);
app.use("/", exitRoutes);
app.use("/", QRcodeRoutes);
app.use("/", uploadRoutes);
app.listen(8800);