const express = require('express');
const { db } = require('../db.js');

const router = express.Router();

router.post('/exit', (req, res) => {

  console.log("\n --- SAIDA ---\n");

  const login = req.body.login;
  const senha = req.body.senha;

  console.log("----------------------------------------------------------");
  console.log("Dados recebidos: ", req.body);
  console.log("----------------------------------------------------------");
  console.log();

  const query = `SELECT id FROM usuarios WHERE login = '${login}' AND senha = '${senha}' `;

  db.query(query, (err, result) => {
    
    if (err) throw err;
    
    if (result.length > 0) {
      const id = result[0].id;
      
      console.log("ID do usuario: ", id);
      console.log("Consultando na tabela presents!");
     
      const checkQuery = `SELECT id FROM presents WHERE id = '${id}'`;
      db.query(checkQuery, (err, checkResult) => {
        if (err) {
          console.log(err);
          return res.status(400).json(err);
        }

        if (checkResult.length === 0) {
          console.log("O id não existe na tabela 'presents'");
          return res.status(400).json("O id não existe na tabela 'presents'");
        }

        const deleteQuery = `DELETE FROM presents WHERE id = '${id}'`;
        db.query(deleteQuery, (err, deleteResult) => {
          if (err) {
            console.log("ERRO");
            console.log(err);
            return res.status(400).json(err);
          }
          
          console.log("Usuário deletado com sucesso.")
          return res.status(200).json("Usuário deletado com sucesso.");
        });

      });
    } 
    else {
      res.status(401).send('Usuário não encontrado');
    }
    
  });
});

module.exports = router;