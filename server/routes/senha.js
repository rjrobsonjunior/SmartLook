import express from 'express';
import { db } from "../db.js";

const router = express.Router();

router.post('/login', (req, res) => {
    const login = req.body.login;
    const query = `SELECT senha FROM usuarios WHERE login = '${login}'`;
  
    db.query(query, (err, result) => {
      if (err) throw err;
      if (result.length === 0) {
        res.status(401).send('Login inválido!');
      } else {
        const senha = result[0].senha;
        res.send(`Senha do usuário ${login}: ${senha}`);
      }
    });
  });

export default router;