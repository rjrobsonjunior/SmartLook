const faceapi = require('face-api.js');
const mysql = require('mysql');

const fs = require('fs');
const path = require('path');
const express = require('express');
const app = express();
//const { read } = require('jimp');
const Jimp = require('jimp');



const canvas = require("canvas");
const { Canvas, Image, ImageData } = canvas;
faceapi.env.monkeyPatch({ Canvas, Image, ImageData });

// carrega o modelo da face-api.js
Promise.all([
  faceapi.nets.ssdMobilenetv1.loadFromDisk(path.join(__dirname, 'models')),
  faceapi.nets.faceLandmark68Net.loadFromDisk(path.join(__dirname, 'models')),
  faceapi.nets.faceRecognitionNet.loadFromDisk(path.join(__dirname, 'models'))
]);

// insere as informações no banco de dados
const connection = mysql.createConnection({
  host: 'localhost',
  user: 'root',
  password: '12345',
  database: 'tabela_teste'
});

app.get('/add', async (req, res) => {
  // cria o caminho completo para a imagem
  const imagePath = path.join(__dirname, 'img', '3.jpg');

  // lê a imagem a partir do disco e carrega em um buffer
  const buffer = fs.readFileSync(imagePath);

  // carrega a imagem do buffer com a biblioteca canvas
  const img = await canvas.loadImage(buffer);

  // detecta as faces na imagem
  const detections = await faceapi.detectSingleFace(img).withFaceLandmarks().withFaceDescriptor();
  

  // converte as características faciais em um objeto JSON
  //const json = JSON.stringify(detections.map(d => d.descriptor));
  const json = JSON.stringify (detections.descriptor);

  connection.connect();

  const query = "INSERT INTO faceteste (login, senha, face, nome) VALUES (?, ?, ?, ?)";
  const values = ["123", "12345", json, "Neymar"];

  connection.query(query, values, function (error, results, fields) {
    if (error) {
      console.log(error);
      res.status(500).send("Erro ao inserir usuário no banco de dados");
    } else {
      console.log("Usuário cadastrado com sucesso!");
      res.status(200).send("Usuário cadastrado com sucesso!");
    }
  });

  connection.end();
});


app.get('/compare', async (req, res) => {

  connection.connect();

  const query = "SELECT * FROM faceteste";

  connection.query(query, async function (error, results, fields) {
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

    // Carrega a imagem a ser reconhecida
    const imagePath = path.join(__dirname, 'img', '5.jpg');
    const buffer = fs.readFileSync(imagePath);
    const img = await canvas.loadImage(buffer);

    // Extrai as características faciais da imagem
    const detections = await faceapi.detectSingleFace(img).withFaceLandmarks().withFaceDescriptor();
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

  connection.end();
});

app.get('/compareMULTER', async (req, res) => {

  connection.connect();

  const query = "SELECT * FROM faceteste";

  connection.query(query, async function (error, results, fields) {
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

    const imagePath = path.join(__dirname, 'img', '5.jpg');

    // Carrega a imagem a ser reconhecida usando o Jimp
    Jimp.read(imagePath)
      .then((image) => {
        // Converte a imagem para um buffer
        return image.getBufferAsync(Jimp.MIME_JPEG);
      })
      .then((buffer) => {
        // Carrega a imagem usando o canvas.loadImage() com o buffer
        const img = canvas.loadImage(buffer);

        // Extrai as características faciais da imagem
        const detections = faceapi.detectSingleFace(img).withFaceLandmarks().withFaceDescriptor();
        let queryDescriptors = [];
        queryDescriptors = detections.descriptor;
        
        /* DEBUG */
        if (savedDescriptors.length === 0) {
          console.log('Não há descritores salvos para comparar');
          res.status(500).send("Não há descritores salvos para comparar");
        }

        else if (queryDescriptors.length === 0) {
          console.log('Não há descritores na imagem para comparar');
          res.status(500).send("Não há descritores na imagem para comparar");
          
        }
        
        else{
      
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
        
      })

      .catch((error) => {
        console.error('Erro ao ler a imagem:', error);
        res.status(500).send('Erro ao ler a imagem');
      });

    connection.end();
  });

});


// Importe as bibliotecas necessárias
const multer = require('multer');
const upload = multer().single('imagem'); // 'image' é o nome do campo que contém a imagem na requisição POST

// Rota para receber a imagem via POST e realizar o reconhecimento facial
app.post('/reconhecimento-facial', upload, async (req, res) => {
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

    // Extrai as características faciais da imagem
    const detections = await faceapi.detectSingleFace(img).withFaceLandmarks().withFaceDescriptor();
    if (!detections) {
      res.status(400).send('Nenhuma face detectada na imagem');
      return;
    }

    const queryDescriptors = detections.descriptor;

    /* CARACTERISTICAS DAS IMAGENS NO BANCO DE DADOS */

    connection.connect();
    const query = "SELECT * FROM faceteste";

    connection.query(query, async function (error, results, fields) {
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
      console.log(result);

      // Envie a resposta com o resultado da comparação
      res.status(200).send(result);
      connection.end();
    });

  } catch (error) {
    console.error('Erro ao processar a imagem:', error);
    res.status(500).send('Erro ao processar a imagem');
    connection.end();
  }

});




// inicia o servidor na porta 3000
app.listen(3000, () => {
  console.log('Servidor iniciado na porta 3000');
});
