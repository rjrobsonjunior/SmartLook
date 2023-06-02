const express = require('express');
const { db } = require('../db.js');

const router = express.Router();

router.post('/login', (req, res) => {

    const login = req.body.login;
    const senha1 = req.body.senha;

    const query = `SELECT id, nome, login, senha FROM usuarios WHERE login = '${login}'`;
  
    db.query(query, (err, result) => {
      if (err) throw err;
      if (result.length === 0) {
        res.status(401).send('Login inválido!');
      } 
      else {
        if(senha1 == result[0].senha)
        {
          res.send(`true`);
          const usuario = result[0];
          console.log(`/insert/${usuario.id}/${usuario.nome}/${usuario.login} `);
          res.redirect(`/insert/${usuario.id}/${usuario.nome}/${usuario.login}`);
        }
        else
        {
          res.send(`false`);
        }
      }
    });
  });

module.exports = router;