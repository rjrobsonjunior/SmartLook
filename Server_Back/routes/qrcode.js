const express = require('express');
const { db } = require('../db.js');
const path = require('path');
const jsQR = require('jsqr');
const {createCanvas, loadImage } = require('canvas');
const router = express.Router();
const aguardaResposta = require('../controllers/aguardaResposta.js');

const path_qr = './routes/uploads/qrCode.jpg';

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


//Rota para mudança da variável
router.get('/continuarProcessamentoQR', async(req, res) =>{

  //Altera o valor da variavel que permite com que a requisição de processamento continuar
  aguardaResposta.setPodeContinuar(true);
  //console.log("\n\nVariavel de controle alterada! Processo iniciado!");
  res.status(200).send('Variavel alterada com sucesso!');

  //Isso pode ser feito também atraves de comunicação webSocket entre o servidor e o site (que esta na rota GET /foto em upload)

}); 

router.get('/analiseQR', async (req, res) => {
  
  console.log("\n --- ACESSO POR QR CODE ---\n");
  console.log("Acesse o site http://localhost:8800/qrcode e faça o upload do seu QRCODE para o acesso\n");

  //Sempre que começa o processo ele reinicia a variavel de controle
  aguardaResposta.setPodeContinuar(false);

  // Aguardar a mudança da variável global com tempo máximo de espera de 30 segundos
  try {
    await aguardaResposta.aguardarMudancaVariavel();
    
  } 
  catch (error) {
    console.error(error.message);
    return res.status(404).send('Tempo maximo atigindo! O processamento vai continuar com a imagem atual');
  }

  
  const qrCodeData = await processQRCode(path_qr);

  if (!qrCodeData) {
    return res.status(500).send('QRCode data null');
  }

  const parsedData = JSON.parse(qrCodeData);

  const login = parsedData.login;
  const senha = parsedData.senha;

  console.log("----------------------------------------------------------");
  console.log("Dados recebidos: Login = ", login, " Senha = ", senha);
  console.log("----------------------------------------------------------\n");
  
  const query = `SELECT id, nome, login, senha FROM usuarios WHERE login = '${login}'`;
   
  db.query(query, (err, result) => {
    if (err) throw err;

    if (result.length === 0) {
      res.status(500).send('Login inválido!');
      console.log("Login invalido");
    } 

    else {
      if(senha == result[0].senha)
      {
        console.log(`Login valido! Seja bem vindo ${result[0].nome}\n`);
        const usuario = result[0];
        
        const sql = 'INSERT INTO presents (id, nome, login) VALUES (?, ?, ?)';
        const values = [usuario.id, usuario.nome, usuario.login];
      
        db.query(sql, values, (err) => {
          if (err) {
            if (err.code === 'ER_DUP_ENTRY') {
              console.error('Erro ao inserir os dados na lista de presents! O usuário já foi inserido anteriormente.');
            } else {
              console.error('Erro ao inserir os dados na lista de presents!', err);
            }
          }
          else{
            console.log('Dados inseridos com sucesso na lista de presentes');
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