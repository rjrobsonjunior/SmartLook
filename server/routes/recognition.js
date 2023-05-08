import express from 'express';
import { db } from "../db.js";

const router = express.Router();

const express = require('express');
const bodyParser = require('body-parser');
const faceapi = require('face-api.js');
const canvas = require('canvas');

const app = express();

// Configura o body-parser para lidar com o corpo da requisição
app.use(bodyParser.urlencoded({ extended: true }));
app.use(bodyParser.json());

// Inicializa os modelos da face-api.js
Promise.all([
  faceapi.nets.ssdMobilenetv1.loadFromDisk('./models'),
  faceapi.nets.faceLandmark68Net.loadFromDisk('./models'),
  faceapi.nets.faceRecognitionNet.loadFromDisk('./models')
]).then(() => {
  console.log('Modelos carregados com sucesso!');
}).catch((error) => {
  console.error('Erro ao carregar modelos:', error);
});

// Cria a rota para receber a imagem
app.post('/processar-imagem', async (req, res) => {
  try {
    // Converte a imagem recebida em um objeto ImageData usando a biblioteca canvas
    const image = await canvas.loadImage(req.body.imagem);
    const canvasElement = canvas.createCanvas(image.width, image.height);
    const canvasContext = canvasElement.getContext('2d');
    canvasContext.drawImage(image, 0, 0, image.width, image.height);
    const imageData = canvasContext.getImageData(0, 0, image.width, image.height);

    // Detecta as faces na imagem usando a face-api.js
    const faceDetectionOptions = new faceapi.SsdMobilenetv1Options({ minConfidence: 0.5 });
    const faceDetections = await faceapi.detectAllFaces(imageData, faceDetectionOptions).withFaceLandmarks().withFaceDescriptors();

    // Busca no banco de dados o dado tipo blob do usuário que mais se assemelha da foto recebida
    const [rows] = await pool.query(`SELECT * FROM usuarios ORDER BY ABS(face_data - ?) LIMIT 1`, [faceDetections[0].descriptor.join()]);

    // Retorna o resultado
    res.json(rows[0]);
  } catch (error) {
    console.error('Erro ao processar imagem:', error);
    res.status(500).send('Erro ao processar imagem');
  }
});
export default router;