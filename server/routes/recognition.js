const express = require('express');
const path = require('path');

const bodyParser = require('body-parser');
const faceapi = require('face-api.js');
const { db } = require('../db.js');

const canvas = require("canvas");
const { Canvas, Image, ImageData } = canvas;
faceapi.env.monkeyPatch({ Canvas, Image, ImageData });
const multer = require('multer');

/*
const storage = multer.diskStorage({
  destination: function (req, file, cb) {
    cb(null, 'img');
  },
  filename: function (req, file, cb) {
    cb(null, 'img.jpg');
  }
});

const upload = multer({ storage: storage });
*/

const upload = multer().single('imagem'); // 'image' é o nome do campo que contém a imagem na requisição POST


const router = express.Router();
router.use(bodyParser.json());
router.use(bodyParser.urlencoded({ extended: true }));
router.use(express.static('public'));
router.use(express.urlencoded({ extended: true }));
router.use(express.json({ limit: '50mb' }));

const modelsPath = '/home/rj/Documentos/OF1/server/routes/models';


// carrega o modelo da face-api.js
Promise.all([
  faceapi.nets.ssdMobilenetv1.loadFromDisk(path.join(modelsPath)),
  faceapi.nets.faceLandmark68Net.loadFromDisk(path.join(modelsPath)),
  faceapi.nets.faceRecognitionNet.loadFromDisk(path.join(modelsPath))
]).then(() => console.log('Models loaded!'));

router.post('/recognition', upload, async (req, res) => {
  
  try {

    /* IMAGEM RECEBIDA VIA POST */

    // Verifique se a imagem foi enviada corretamente
    if (!req.file) {
      res.status(400).send('Nenhuma imagem enviada');
      return;
    }

    // Acesso à imagem através do req.file.buffer
    const buffer = req.file.buffer;
    const img = await canvas.loadImage(buffer);
    
    if(img){
      console.log("Imagem carregada pelo canvas!");
    }

    // Extrai as características faciais da imagem
    const detections = await faceapi.detectSingleFace(img).withFaceLandmarks().withFaceDescriptor();
    if (!detections) {
      res.status(400).send('Nenhuma face detectada na imagem');
      return;
    }
    else
    {
      console.log("Imagem analisada pela face-api!");
    }

    const queryDescriptors = detections.descriptor; 

    /* CARACTERISTICAS DAS IMAGENS NO BANCO DE DADOS */

    const query = "SELECT * FROM usuarios";

    db.query(query, async function (error, results, fields) {
      if (error) throw error;
      console.log("Entrei no connect");

      let savedDescriptors = [];

      // Extrai as informações de cada linha do resultado da consulta
      for (let i = 0; i < results.length; i++) {

        // Extrai as características faciais em formato JSON de cada linha
        let json = JSON.parse(results[i].recognition1);

        // Converte o objeto em um array
        let values = Object.values(json);

        // Converte o JSON em um array de descritores faciais
        let descriptors = new Float32Array(values);

        // Converte as características faciais em formato JSON em um objeto LabeledFaceDescriptors do face-api.js
        let labeledDescriptors  = new faceapi.LabeledFaceDescriptors(results[i].nome, [descriptors]);
        
        // Adiciona os descritores do usuário ao array de descritores
        savedDescriptors.push(labeledDescriptors);
      }


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

      // Compara as características faciais da imagem com as características faciais do banco de dados
      const faceMatcher = new faceapi.FaceMatcher(savedDescriptors);
      const bestMatch = faceMatcher.findBestMatch(queryDescriptors);
      const result = bestMatch.toString() 

      // Identifica a pessoa na imagem
      console.log("result:")
      console.log(result);

      // Envie a resposta com o resultado da comparação
      res.status(200).send(result);
    });

  } catch (error) {
    console.error('Erro ao processar a imagem:', error);
    res.status(500).send('Erro ao processar a imagem');
  }


});


//export default router;
module.exports = router;
