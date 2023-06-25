const express = require('express');
const { db } = require('../db.js');

const router = express.Router();

router.post('/login', (req, res) => {

  console.log("\n --- ACESSO POR SENHA ---\n");

  const login = req.body.login;
  const senha = req.body.senha;
  console.log("----------------------------------------------------------");
  console.log("Dados recebidos: ", req.body);
  console.log("----------------------------------------------------------");
  console.log();

  const query = `SELECT id, nome, login, senha FROM usuarios WHERE login = '${login}'`; 
  
  db.query(query, (err, result) => {
    if (err) throw err;
    if (result.length === 0) {
      res.status(400).send('Login inválido!');
    } 
    else {
      if(senha == result[0].senha)
      {
        
        console.log(`Login valido! Seja bem vindo ${result[0].nome}`);
        console.log();

        //Inserir usuario na lista de presentes
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
            console.log('Dados inseridos com sucesso');
          }
        });

        res.status(200).send(result[0].nome);
      }
      else
      {
        res.status(400).send(`Senha invalida!`);
      }
    }
  });
});

module.exports = router;