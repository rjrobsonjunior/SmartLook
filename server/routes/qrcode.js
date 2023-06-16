const express = require('express');
const { db } = require('../db.js');
const multer = require('multer');
const path = require('path');
const jsQR = require('jsqr');
const {createCanvas, loadImage } = require('canvas');
const router = express.Router();

const path_qr = './routes/uploads/qrCode.jpg';

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

let pode_continuar;

router.get('/continuarProcessamentoQR', async(req, res) =>{

  //Altera o valor da variavel que permite com que a requisição de processamento continuar
  pode_continuar = true;
  console.log("\nVariavel de controle alterada! Processo iniciado!");
  res.status(200).send('Variavel alterada com sucesso!');

  //Isso pode ser feito também atraves de comunicação webSocket entre o servidor e o site (que esta na rota GET /foto em upload)

}); 

// Função para aguardar a mudança da variável global
const aguardarMudancaVariavel = () => new Promise((resolve, reject) => {

  const tempoMaximoEspera = 60000; // (60 segundos)
  const intervaloVerificacao = 500; // (0,5 segundo)
  let tempoDecorrido = 0;

  const intervalo = setInterval(() => {

    if (pode_continuar) {
      clearInterval(intervalo);
      console.log("resolvido!");
      resolve();
      
    }
    else{
      process.stdout.write(".");
      tempoDecorrido += intervaloVerificacao;

      if (tempoDecorrido >= tempoMaximoEspera) {
        clearInterval(intervalo);
        reject(new Error("\nTempo máximo de espera atingido! O processamento vai continuar com a imagem atual!"));
      }
    }

  }, intervaloVerificacao); // Intervalo de verificação (500 milissegundos)

});

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

router.get('/qrcode', async (req, res) => {
  const filePath = path.join(__dirname + '/public/qr.html');
  res.sendFile(filePath);
});


module.exports = router;