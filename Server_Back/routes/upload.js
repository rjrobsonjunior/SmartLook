const express = require('express');
const multer = require('multer');
const path = require('path');
const fs = require('fs');
const moment = require('moment-timezone');

const router = express.Router();
const path_imagem = './routes/public/assets/fotos';
const path_qr = './routes/uploads';

const salvarFoto = multer.diskStorage({
  destination: function (req, file, cb) {
    cb(null, path_imagem)
  },
  filename: function (req, file, cb) {
    cb(null, 'img.jpg')
  }
});

const salvarQr = multer.diskStorage({
  destination: function (req, file, cb) {
    cb(null, path_qr)
  },
  filename: function (req, file, cb) {
    cb(null, 'qrCode.jpg')
  }
});

const uploadFoto = multer({ storage: salvarFoto });
const uploadQr = multer({ storage: salvarQr });

router.post('/foto', uploadFoto.single('imagem'), function (req, res) {
  
  if (req.file) {
    res.send("Single file uploaded successfully");
  } 
  else {
    res.status(400).send("Please upload a valid image");
  }

  
});

router.post('/qr', uploadQr.single('qr'), function (req, res) {
  
  if (req.file) {
    res.send("Single file uploaded successfully");
  } 
  else {
    res.status(400).send("Please upload a valid image");
  }

  
});

router.get('/foto', (req, res) => {
  const filePath = path.join(__dirname + '/public/foto.html');
  res.sendFile(filePath);
});


// Rota para obter a data de modificação da foto
router.get('/dataModificacaoFoto', (req, res) => {
  
  fs.stat('./routes/public/assets/fotos/img.jpg', (err, stats) => {
    if (err) {
      console.error(err);
      res.status(500).send('Erro ao obter a data de modificação da foto');
    } else {
      const modifiedDate = moment(stats.mtime).format('DD-MM-YYYY HH:mm:ss');
      res.status(200).json({ dataModificacao: modifiedDate });
    }
  });
});

module.exports = router;