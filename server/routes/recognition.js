import express from 'express';
import fs from 'fs';
import bodyParser from 'body-parser';
import * as canvas from 'canvas';
import * as faceapi from 'face-api.js';
import { db } from "../db.js";
import path from 'path';
import { fileURLToPath } from 'url';
import { dirname } from 'path';

const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);

// Now you can use __dirname in the rest of your code
const filePath = path.join(__dirname, 'models');

const router = express.Router();

router.use(bodyParser.json());
router.use(bodyParser.urlencoded({ extended: true }));
router.use(express.static('public'));
router.use(express.urlencoded({ extended: true }));
router.use(express.json({ limit: '50mb' }));

// Configurações do ESP32-CAM
const esp32CamUrl = 'http://192.168.1.19';
const esp32CamPort = 80; 



const { Canvas, Image, ImageData } = canvas;
faceapi.env.monkeyPatch({ Canvas, Image, ImageData });


// carrega o modelo da face-api.js
Promise.all([
  faceapi.nets.ssdMobilenetv1.loadFromDisk(filePath),
  faceapi.nets.faceLandmark68Net.loadFromDisk(filePath),
  faceapi.nets.faceRecognitionNet.loadFromDisk(filePath)
]).then(() => console.log('Models loaded!'));

router.post('/recognition', async (req, res) => {
  
  //carrega imagem recebida
  const image = req.body.image;

  // Decode base64 image
  const base64Image = image.split(';base64,').pop();
  const imageBuffer = Buffer.from(base64Image, 'base64');

  // Save image to disk
  const fileName = `${Date.now()}.jpg`;
  fs.writeFileSync(fileName, imageBuffer);

  // Load image from disk and detect face
  const imageFile = await faceapi.bufferToImage(fs.readFileSync(fileName));
  const detection = await faceapi.detectSingleFace(imageFile, faceDetectionOptions).withFaceLandmarks().withFaceDescriptor();

  const query = "SELECT * FROM CRUD.usuarios";

  db.query(query, async function (error, results, fields) {
    if (error) throw error;
    
    let savedDescriptors = [];

    // Extrai as informações de cada linha do resultado da consulta
    for (let i = 0; i < results.length; i++) {

      // Extrai as características faciais em formato JSON de cada linha
      let json = JSON.parse(results[i].face);

      // Converte o objeto em um array
      let values = Object.values(json);

      // Converte o JSON em um array de descritores faciais
      let descriptors = new Float32Array(values);

      // Converte as características faciais em formato JSON em um objeto LabeledFaceDescriptors do face-api.js
      let labeledDescriptors  = new faceapi.LabeledFaceDescriptors(results[i].nome, [descriptors]);
      
      // Adiciona os descritores do usuário ao array de descritores
      savedDescriptors.push(labeledDescriptors);
    }

      // Load image from disk and detect face
    const imageFile = await faceapi.bufferToImage(fs.readFileSync(fileName));
    const detections = await faceapi.detectSingleFace(imageFile, faceDetectionOptions).withFaceLandmarks().withFaceDescriptor();
    let queryDescriptors = [];
    queryDescriptors = detections.descriptor;
    
    /* DEBUG */
    if (savedDescriptors.length === 0) {
      console.log('Não há descritores salvos para comparar');
      res.status(500).send("Não há descritores salvos para comparar");
    }

    if (queryDescriptors.length === 0) {
      console.log('Não há descritores na imagem para comparar');
      res.status(500).send("Não há descritores na imagem para comparar");
      
    }
    
    // Verifica se todos os descritores faciais têm o mesmo tamanho
    let allSameSize = true;
    for (let i = 0; i < savedDescriptors.length; i++) {
      if (savedDescriptors[i].descriptors[0].length !== queryDescriptors.length) {
        console.log("IMG 1");
        console.log(savedDescriptors[i].descriptors[0].length);
        console.log("IMG 2");
        console.log(queryDescriptors.length)
        allSameSize = false;
        break;
      }
    }

    if (!allSameSize) 
    {
      console.log('Erro: nem todos os descritores faciais têm o mesmo tamanho');
      res.status(500).send("As dimensões dos descritores não são iguais");
    } 

    // Imprime os valores dos descritores faciais armazenados no banco de dados
    console.log('Descritores faciais armazenados no banco de dados:');
    for (let i = 0; i < savedDescriptors.length; i++) {
      console.log(savedDescriptors[i].descriptors[0]);
    }

    // Imprime os valores dos descritores faciais da imagem
    console.log('Descritores faciais da imagem:');
    console.log(queryDescriptors);

    // Compara as características faciais da imagem com as características faciais do banco de dados
    const faceMatcher = new faceapi.FaceMatcher(savedDescriptors);
    const bestMatch = faceMatcher.findBestMatch(queryDescriptors);

    // Identifica a pessoa na imagem
    console.log(bestMatch.toString());

    // Envie a resposta com o resultado da comparação
    res.send(bestMatch.toString());
  });
  // Delete image file from disk

});

export default router;