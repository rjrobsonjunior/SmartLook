const express = require("express");
const app = express();
const bodyParser = require("body-parser");
const faceapi = require("face-api.js");
const mysql = require("mysql");

app.use(bodyParser.urlencoded({ extended: false }));
app.use(bodyParser.json());

const router = express.Router();

app.post("/process-images", async (req, res) => {
  const { images } = req.body;

  // Carrega os modelos necessários do Faceapi.js
  await faceapi.nets.faceRecognitionNet.loadFromDisk("./models");
  await faceapi.nets.faceLandmark68Net.loadFromDisk("./models");
  await faceapi.nets.ssdMobilenetv1.loadFromDisk("./models");

  const descriptors = [];

  // Itera pelas imagens recebidas e extrai as características faciais de cada uma
  for (let i = 0; i < images.length; i++) {
    const img = await faceapi.bufferToImage(images[i]);
    const detections = await faceapi
      .detectSingleFace(img)
      .withFaceLandmarks()
  }
}

//export default router;