const express = require('express');
const { db } = require('../db.js');
const multer = require('multer');
const jsQR = require('jsqr');
const {createCanvas, loadImage } = require('canvas');

const upload = multer({
  dest: './routes/uploads'
})

async function processQRCode(imagePath) {
  try {
    const image = await loadImage(imagePath);
    const canvas = createCanvas(image.width, image.height);
    const ctx = canvas.getContext('2d');
    ctx.drawImage(image, 0, 0);
    const imageData = ctx.getImageData(0, 0, image.width, image.height);
    const code = jsQR(imageData.data, imageData.width, imageData.height);

    if (code && code.data) {
      const qrCodeData = code.data;
      return qrCodeData;
    } else {
      return null;
    }
  } catch (error) {
    console.error('Erro ao processar o QR Code:', error);
    return null;
  }
}

const router = express.Router();

router.post('/qrcode', upload.single('qrcode'), async (req, res) => {
  
  if(!req.file){
    return res.status(400).send("QRCode null");
  }

  const qrCodeData = await processQRCode(req.file.path);

  if (!qrCodeData) {
    return res.status(450).send('QRCode data null');
  }

  console.log(qrCodeData);
  const parsedData = JSON.parse(qrCodeData);

  const login = parsedData.login;
  const senha1 = parsedData.senha;


  const query = `SELECT id, nome, login, senha FROM usuarios WHERE login = '${login}'`;
   
  db.query(query, (err, result) => {
    if (err) throw err;
    if (result.length === 0) {
      res.status(401).send('Login inválido!');
    } 
    else {
      if(senha1 == result[0].senha)
      {
        res.status(200).send(`true`);
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
      }
      else
      {
        res.status(200).send(`false`);
      }
    }
  });
});

module.exports = router;