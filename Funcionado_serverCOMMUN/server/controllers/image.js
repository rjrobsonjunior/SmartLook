//import { db } from "../db.js";
const { db } = require('../db.js');


// converte o vetor de características faciais em um buffer
const buffer = Buffer.from(JSON.stringify(faceDescriptor));

// insere as informações no banco de dados
await connection.query('INSERT INTO faces (name, face_descriptor) VALUES (?, ?)', ['John Doe', buffer]);

const faceapi = require('face-api.js');
const canvas = require('canvas');


// inicialização da FaceAPI.js
faceapi.env.monkeyPatch({ Canvas, Image });

// carrega o modelo para detecção facial e extração de características
await faceapi.nets.ssdMobilenetv1.loadFromDisk('/path/to/models');
await faceapi.nets.faceLandmark68Net.loadFromDisk('/path/to/models');
await faceapi.nets.faceRecognitionNet.loadFromDisk('/path/to/models');

// carrega a imagem do rosto da pessoa
const image = await canvas.loadImage('/path/to/image');

// detecta os rostos na imagem
const detections = await faceapi.detectAllFaces(image).withFaceLandmarks().withFaceDescriptors();

// extrai as características faciais do primeiro rosto detectado
const faceDescriptor = detections[0].descriptor;
