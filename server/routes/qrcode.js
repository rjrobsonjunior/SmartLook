const express = require('express');
const { db } = require('../db.js');

const router = express.Router();

router.post('/login', (req, res) => {

    const login = req.body.login;
    const senha1 = req.body.senha;

    const query = `SELECT senha FROM usuarios WHERE login = '${login}'`;
  
    db.query(query, (err, result) => {
      if (err) throw err;
      if (result.length === 0) {
        res.status(401).send('Login inválido!');
      } 
      else {
        if(senha1 == result[0].senha)
        {
          res.send(`true`);
        }
        else
        {
          res.send(`false`);
        }
      }
    });
  });

module.exports = router;