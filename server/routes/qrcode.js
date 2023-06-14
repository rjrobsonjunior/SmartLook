const express = require('express');
const { db } = require('../db.js');
const multer = require('multer');
const qr = require('qr-image');
const fs = require('fs');
const jsQR = require('jsqr');
const {createCanvas, loadImage } = require('canvas');

const upload = multer({
  dest: './routes/uploads',
});

/*const multer = require('multer');
const qr = require('qr-image');
const processQRCode = (imagePath) => {
  return new Promise((resolve, reject) => {
    const qrDecoder = qr.reader();
    qrDecoder.callback = (err, result) => {
      if (err) {
        reject(err);
      } else {
        resolve(result.data);
      }
    };

    const image = qr.image(imagePath);
    image.pipe(qrDecoder);
  });
};
*/
/**
const processQRCode = async (filePath) => {
  try {
    // Carrega a imagem usando a biblioteca Jimp
    const image = await Jimp.read(filePath);

    // Converte a imagem para um array de pixels
    const { width, height } = image.bitmap;
    const imageData = new Uint8ClampedArray(width * height * 4);

    image.scan(0, 0, width, height, (x, y, idx) => {
      const pixel = image.getPixelColor(x, y);
      imageData[idx] = Jimp.rgbaToInt(...Jimp.intToRGBA(pixel)).r;
      imageData[idx + 1] = Jimp.rgbaToInt(...Jimp.intToRGBA(pixel)).g;
      imageData[idx + 2] = Jimp.rgbaToInt(...Jimp.intToRGBA(pixel)).b;
      imageData[idx + 3] = Jimp.rgbaToInt(...Jimp.intToRGBA(pixel)).a;
    });

    // Decodifica a imagem em busca do QR Code
    const code = jsQR(imageData, width, height);

    // Verifica se o QR Code foi encontrado
    if (!code) {
      throw new Error('Não foi possível encontrar o QR Code na imagem.');
    }

    // Obtém os dados do QR Code
    const qrCodeData = code.data;

    // Retorna os dados do QR Code
    return qrCodeData;
  } catch (error) {
    throw new Error(`Erro ao processar a imagem do QR Code: ${error.message}`);
  }
};
 */


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
      console.log(qrCodeData);
      return qrCodeData;
    } else {
      return null;
    }
  } catch (error) {
    console.error('Erro ao processar o QR Code:', error);
    return null;
  }
};

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