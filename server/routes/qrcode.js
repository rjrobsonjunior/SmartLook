const express = require('express');
const { db } = require('../db.js');
const multer = require('multer');
const path = require('path');
const jsQR = require('jsqr');
const {createCanvas, loadImage } = require('canvas');
const Jimp = require('jimp');
////const QRCodeReader = require('qrcode-reader');

const storage = multer.diskStorage({
  destination: function (req, file, cb) {
    cb(null, './routes/uploads');
  },
  filename: function (req, file, cb) {
    cb(null, 'qrCode.jpg');
  }
});

const upload = multer({ storage: storage });

router.post('/qrcode', upload.single('qrcode'), async (req, res) => {
  if(!req.file){
    return res.status(400).send("QRCode null");
  }
  else{
    return res.status(200).send('Qr code salvo com sucesso!');
  }
});


async function processQRCode(imagePath) {
  console.log("Estou na processQRCode");

  try {
    const image = await loadImage(imagePath);

    const ctx = canvas.getContext('2d');
    ctx.drawImage(image, 0, 0);
    const imageData = ctx.getImageData(0, 0, image.width, image.height);

    const code = jsQR(imageData.data, imageData.width, imageData.height);

    if (code && code.data) {
      const qrCodeData = code.data;

      return qrCodeData;
    } 

    else {
      return null;
    }
  } 

  catch (error) {
    console.error('Erro ao processar o QR Code:', error);
    return null;
  }
};


router.get('/qrcodeAnalise', async (req, res) => {
  
  //Sempre que começa o processo ele reinicia a variavel de controle
  pode_continuar = false;

  // Aguardar a mudança da variável global com tempo máximo de espera de 30 segundos
  try {
    await aguardarMudancaVariavel();
    
  } 
  catch (error) {
    console.error(error.message);
    return res.status(404).send('Tempo maximo atigindo! O processamento vai continuar com a imagem atual');
  }

  
  console.log("Continuando reconhecimento...");

  const qrCodeData = await processQRCode(path_qr);

  if (!qrCodeData) {
    return res.status(450).send('QRCode data null');
  }

  const parsedData = JSON.parse(qrCodeData);

  const login = parsedData.login;
  const senha1 = parsedData.senha;

  console.log("Login:");
  console.log(login);
  console.log("Senha:");
  console.log(senha1);
  
  const query = `SELECT id, nome, login, senha FROM usuarios WHERE login = '${login}'`;
   
  db.query(query, (err, result) => {
    if (err) throw err;

    if (result.length === 0) {
      res.status(500).send('Login inválido!');
    } 

    else {
      if(senha1 == result[0].senha)
      {
        
        const usuario = result[0];
        
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

        res.status(200).send(usuario.nome);
      }
      else
      {
        res.status(400).send(`false`);
      }
    }
  });

});


router.get('/qrcode', async (req, res) => {
  const filePath = path.join(__dirname + '/public/qr.html');
  res.sendFile(filePath);
});


module.exports = router;