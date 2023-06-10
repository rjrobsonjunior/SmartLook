const express = require('express');
const { db } = require('../db.js');

const router = express.Router();

router.post('/exit', (req, res) => {

  const login = req.body.login;
  const senha = req.body.senha;

  const query = `SELECT id FROM usuarios WHERE login = '${login}' AND senha = '${senha}' `;
  console.log(query);

  db.query(query, (err, result) => {
    
    if (err) throw err;
    
    if (result.length > 0) {
      const id = result[0].id;
      console.log(id);
      res.redirect(`/delete/${id}`);
    } 
    else {
      res.status(401).send('Usuário não encontrado');
    }
    
  });
});

module.exports = router;