const express = require('express');
const { db } = require('../db.js');
const multer = require('multer');
const fs = require('fs');
const jsQR = require('jsqr');
const {createCanvas, loadImage } = require('canvas');
const Jimp = require('jimp');
//const QRCodeReader = require('qrcode-reader');

const storage = multer.diskStorage({
  destination: function (req, file, cb) {
    cb(null, './routes/uploads');
  },
  filename: function (req, file, cb) {
    cb(null, file.originalname);
  }
});

const upload = multer({ storage: storage });

/*
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

/** 
async function processQRCode(imagePath) {
  try {

    const image1 = await Jimp.read(imagePath);
    image1.resize(800, Jimp.AUTO); // Redimensiona para uma largura de 800 pixels (altura ajustada automaticamente)
    image1.greyscale(); // Converte para escala de cinza
    image1.brightness(0.2); // Aumenta o brilho em 0.2 (valor entre -1 e 1)
    image1.contrast(0.5); // Aumenta o contraste em 0.5 (valor entre -1 e 1)
    image1.threshold({ max: 128 }); // Binariza a imagem usando um limiar de 128 (pixels abaixo do limiar se tornam pretos, e acima do limiar se tornam brancos)
    await image1.writeAsync('./routes/uploads');

    const image = await loadImage(image1);
    const canvas = createCanvas(image.width, image.height);
    const ctx = canvas.getContext('2d');
    ctx.drawImage(image, 0, 0, image.width, image.height);
    console.log("Created Canvas")

    const imageData = ctx.getImageData(0, 0, image.width, image.height);
    const code = jsQR(imageData.data, imageData.width, imageData.height);
    console.log(code);
    if (code && code.data) {
      const qrCodeData = code.data;
      return qrCodeData;
    } 
    else {
      return null;
    }
  } catch (error) {
    console.error('Erro ao processar o QR Code:', error);
    return null;
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
/** 
const processQRCode = async (imagePath) => {
  try {
    await Jimp.read(imagePath, (err, img) => {
      if (err) {
        console.error('Error reading image:', err);
        return res.status(500).json({ error: 'Error reading image' });
      }
      const qr = new QRCodeReader();
    
      // qrcode-reader's API doesn't support promises, so wrap it
      const value = new Promise((resolve, reject) => {
        qr.callback = (err, v) => err != null ? reject(err) : resolve(v);
        qr.decode(img.bitmap);
      });
      
      for (const point of value.points) {
        img.scan(Math.floor(point.x) - 5, Math.floor(point.y) - 5, 10, 10, function(x, y, idx) {
          // Modify the RGBA of all pixels in a 10px by 10px square around the 'FinderPattern'
          this.bitmap.data[idx] = 255; // Set red to 255
          this.bitmap.data[idx + 1] = 0; // Set blue to 0
          this.bitmap.data[idx + 2] = 0; // Set green to 0
          this.bitmap.data[idx + 3] = 255; // Set alpha to 255
        });
      }
      
      img.writeAsync('./qr_photo_annotated.png');
      
     console.log(value);
      const result = JSON.parse(value.result);
      //console.log(result);
      
      if (result) {
        return result;
      } 
      else {
        return null;
      }
    });
  } catch (error) {
    console.error('Erro ao processar o QR Code:', error);
    return null;
  }
};
*/


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