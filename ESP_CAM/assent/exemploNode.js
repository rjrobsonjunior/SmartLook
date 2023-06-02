const express = require('express');
const multer = require('multer');
const fs = require('fs');
const fetch = require('node-fetch');

const app = express();

// Configuração do middleware multer para lidar com o upload de arquivos
const upload = multer({
    // Pasta onde os arquivos serão salvos temporariamente
    dest: 'uploads/' 
});

// Rota para receber a imagem enviada por uma requisição POST
app.post('/imagem', upload.single('imagem'), async (req, res) => 
{
    // Informações sobre o arquivo enviado
    const imagem = req.file; 
    console.log(imagem);

    // Lê o arquivo temporário
    const data = fs.readFileSync(imagem.path);

    // Envia a imagem para a API usando o fetch
    const response = await fetch('http://localhost:3000/analise', {
    method: 'POST',
    body: data,
    headers: {
      'Content-Type': 'image/jpeg' // Define o tipo de conteúdo da requisição como imagem/jpeg
    }

    });

    if (response.ok) {
        // Requisição bem sucedida

        // Trata a resposta da API
        const resultado = await response.json();
        console.log(resultado);

        // Envia resultado para o banco de dados

    } 
    else 
    {
        console.log('Requisição Mal Sucedida!');
    }

    

    
    
});

app.listen(3000, () => {
    console.log('Servidor iniciado na porta 3000');
});