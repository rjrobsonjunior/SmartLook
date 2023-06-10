const express = require('express');
const multer = require('multer');
const path = require('path');
const fs = require('fs');
const moment = require('moment-timezone');

const router = express.Router();
const path_imagem = './routes/public/assets/fotos';

const storage = multer.diskStorage({
  destination: function (req, file, cb) {
    cb(null, path_imagem)
  },
  filename: function (req, file, cb) {
    cb(null, 'img.jpg')
  }
});

const upload = multer({ storage: storage });

router.post('/foto', upload.single('imagem'), function (req, res) {
  
  if (req.file) {
    res.send("Single file uploaded successfully");
  } 
  else {
    res.status(400).send("Please upload a valid image");
  }

  
})

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
      console.log("Data de modificação: ");
      console.log(modifiedDate);
      res.status(200).json({ dataModificacao: modifiedDate });
    }
  });
});

module.exports = router;