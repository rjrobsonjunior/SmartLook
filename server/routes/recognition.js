import express from 'express';
import Jimp from 'jimp';
import multer from 'multer';
import { Canvas, Image, ImageData } from 'canvas';
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

//const { Canvas, Image, ImageData } = canvas;
faceapi.env.monkeyPatch({ Canvas, Image, ImageData });


// carrega o modelo da face-api.js
Promise.all([
  faceapi.nets.ssdMobilenetv1.loadFromDisk(filePath),
  faceapi.nets.faceLandmark68Net.loadFromDisk(filePath),
  faceapi.nets.faceRecognitionNet.loadFromDisk(filePath)
]).then(() => console.log('Models loaded!'));

// 
const storage = multer.diskStorage({
  destination: function (req, file, cb) {
    cb(null, './routes/uploads')
  },
  filename: function (req, file, cb) {
    cb(null, file.originalname)
  }
});

const upload = multer({ storage: storage });

router.post('/recognition', upload.single('image'), async (req, res) => {
  try {
    // Carrega a imagem com a biblioteca Jimp
    console.log(req.file.path);
    const image = await Jimp.read(req.file.path);
    // Converte a imagem para um objeto ImageData suportado pelo face-api.js
    console.log("convertendo imagem");
    const canvas = new Canvas(image.bitmap.width, image.bitmap.height).then(() => console.log('create canva whit image'));
    const ctx = canvas.getContext('2d');
    ctx.drawImage(new Image(image.bitmap.width, image.bitmap.height));
    const imageData = ctx.getImageData(0, 0, image.bitmap.width, image.bitmap.height);

    // Detecta as faces na imagem usando o face-api.js
    const detection = await faceDetectionNet.detectSingleFace(imageData, faceDetectionOptions);

    if (!detection) {
      return res.status(400).json({ error: 'Nenhuma face detectada na imagem.' });
    }

    // Extrai as características da face usando o face-api.js
    const faceDescriptor = await faceRecognitionNet.computeFaceDescriptor(imageData, detection.box);

    // Consulta o banco de dados em busca do usuário com a descrição facial mais próxima
    db.query( 'SELECT id, recognition1 FROM CRUD.usuarios',
      async (error, results) => {
        if (error) throw error;

        let bestMatch = null;
        let bestMatchDistance = Number.MAX_VALUE;

        for (const result of results) {
          const descriptorBuffer = Buffer.from(result.recognition1, 'hex');
          const referenceDescriptor = new Float32Array(descriptorBuffer.buffer);
          const distance = faceapi.euclideanDistance(faceDescriptor, referenceDescriptor);

          if (distance < bestMatchDistance) {
            bestMatchDistance = distance;
            bestMatch = result;
          }
        }

        if (!bestMatch) {
          return res.status(404).json({ error: 'Nenhum usuário encontrado.' });
        }

        return res.json({ id: bestMatch.id, distance: bestMatchDistance });
      }
    );
  } catch (err) {
    return res.status(500).json({error: err.message});
  }
});

export default router;