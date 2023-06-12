const express = require('express');
const { db } = require('../db.js');

const router = express.Router();

router.post('/login', (req, res) => {

  const login = req.body.login;
  const senha = req.body.senha;
  console.log(req.body);  

  const query = `SELECT id, nome, login, senha FROM usuarios WHERE login = '${login}'`; 
  
  db.query(query, (err, result) => {
    if (err) throw err;
    if (result.length === 0) {
      res.status(401).send('Login inválido!');
    } 
    else {
      if(senha == result[0].senha)
      {
        res.status(200).send(result[0].nome);
        console.log(`Login valido! Seja bem vindo ${result[0].nome}`);

        const usuario = result[0];

        //console.log(`/insert/${usuario.id}/${usuario.nome}/${usuario.login} `);
        //res.redirect(`/insert/${usuario.id}/${usuario.nome}/${usuario.login}`);
          
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
        res.status(400).send(`Senha invalida`);
      }
    }
  });
});

module.exports = router;