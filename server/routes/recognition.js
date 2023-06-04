const express = require('express');
const path = require('path');
const fs = require('fs');
const bodyParser = require('body-parser');
const faceapi = require('face-api.js');
const { db } = require('../db.js');

//CANVAS
const canvas = require("canvas");
const { Canvas, Image, ImageData } = canvas;
faceapi.env.monkeyPatch({ Canvas, Image, ImageData });

//MULTER
const multer = require('multer');
const upload = multer().single('imagem'); // 'image' é o nome do campo que contém a imagem na requisição POST

//EXPRESS
const router = express.Router();
router.use(bodyParser.json());
router.use(bodyParser.urlencoded({ extended: true }));
router.use(express.static('public'));
router.use(express.urlencoded({ extended: true }));
router.use(express.json({ limit: '50mb' }));

const modelsPath = './routes/models';
const caminhoDestino = './routes/uploads/imagem.jpg';


// carrega o modelo da face-api.js
Promise.all([
  faceapi.nets.ssdMobilenetv1.loadFromDisk(path.join(modelsPath)),
  faceapi.nets.faceLandmark68Net.loadFromDisk(path.join(modelsPath)),
  faceapi.nets.faceRecognitionNet.loadFromDisk(path.join(modelsPath))
]).then(() => console.log('Models loaded!'));

router.post('/recognition', upload, async (req, res) => {

  console.time('Tempo de execução');

  try {

    /* IMAGEM RECEBIDA VIA POST */

    // Verifique se a imagem foi enviada corretamente
    if (!req.file) {
      res.status(400).send('Nenhuma imagem enviada');
      return;
    }
    
    // Salve a imagem em disco
    await new Promise((resolve, reject) => {
      fs.writeFile(caminhoDestino, req.file.buffer, (err) => {
        if (err) {
          console.error('Erro ao salvar a imagem:', err);
          res.status(500).send('Erro ao salvar a imagem');
          reject(err);
        } else {  
          console.log('Imagem salva com sucesso');
          resolve();
        }
      });
    });

    // Acesso à imagem através do req.file.buffer
    const buffer = req.file.buffer;
    const img = await canvas.loadImage(buffer);
    
    if(img){
      console.log("Imagem carregada pelo canvas!");
    }

    // Extrai as características faciais da imagem
    const detections = await faceapi.detectSingleFace(img).withFaceLandmarks().withFaceDescriptor();
    
    if (!detections) {
      console.log("Nenhuma face detectada na imagem");
      res.status(550).send('Nenhuma face detectada na imagem');
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
          console.log(savedDescriptors[i].descriptors[0].length);
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
      const result = bestMatch.toString();
      
      
      console.log("result:" + result);

      if(result.includes('unknown'))
      {
        res.status(200).send(false);
      }
      else
      {
        
        res.status(200).send(true);

        //Tempo de execução
        console.timeEnd('Tempo de execução');

        //Sepera a string da resposta (NOME (0.5))
        const nome = result.substring(0, result.indexOf(' (')).trim();
        
        //Realiza uma nova consulta para procurar os dados do usuario e insere eles na tabela presents
        const query1 = `SELECT id, nome, login FROM usuarios WHERE nome = '${nome}'`;
        console.log(query1);
        db.query(query1, (err, result1) => {
          if (err) {
            console.error('Erro ao inserir os dados:', err);
          }
          else {
            const usuario = result1[0];
            console.log(usuario);
            const sql = 'INSERT INTO presents (id, nome, login) VALUES (?, ?, ?)';
            const values = [usuario.id, usuario.nome, usuario.login];
          
            db.query(sql, values, (err) => {
              if (err) {
                console.error('Erro ao inserir os dados:', err);
              }
              else{
                console.log('Dados inseridos com sucesso');
              }
            });
          }
          
        });
      }
    });

  } catch (error) {
    console.error('Erro ao processar a imagem:', error);
    res.status(500).send('Erro ao processar a imagem');
  }


});

module.exports = router;
