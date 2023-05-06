const express = require('express')
const app = express()
const fs = require('fs');
const bodyParser = require('body-parser');
const path = require('path');
const request = require('request');


// Configurações do ESP32-CAM
const esp32CamUrl = 'http://192.168.1.19';
const esp32CamPort = 80; 

app.use(bodyParser.urlencoded({ extended: true }));
app.use(bodyParser.json());
app.use(bodyParser.raw({type: 'image/jpeg', limit: '50mb'}));

// Define a pasta de arquivos estáticos
app.use(express.static('public'))
app.use('/tmp', express.static(path.join(__dirname, '/tmp')));

// Define a rota para a página HTML
app.get('/', function (req, res) {
  res.sendFile(__dirname + '/public/index.html')
})

// Define a rota para retomar a foto
app.post('/capture', function (req, res) {
  // Envia a instrução para o ESP32-CAM
  request(`${esp32CamUrl}:${esp32CamPort}/capture`, function (err, response, body) {
    if (err) {
      console.error(err);
      res.status(500).send('Erro ao enviar instrução para o ESP32-CAM');
    } else {
      res.status(200).send('Instrução enviada com sucesso');
    }
  });
});


app.post('/uploadImagem', (req, res) => {
  // salvar a imagem em disco
  const targetPath = `tmp/imagemTeste.jpg`;
  fs.writeFile(targetPath, req.body, (err) => {
    if (err) {
      console.error(err);
      res.status(500).send('Erro ao salvar imagem');
    } else {
      res.status(200).send('Imagem enviada com sucesso');
    }
  });
});



// Inicie o servidor
app.listen(3000, function () {
  console.log('Servidor iniciado na porta 3000!')
})