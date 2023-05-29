const { db } = require('../db.js');

const getUsers = (_, res) => {
  const q = "SELECT * FROM presents";

  db.query(q, (err, data) => {
      if(err) return res.json(err);
      
      return res.status(200).json(data);
  })
};

const addUser = (req, res) => {
  const q = "INSERT INTO usuarios(`id`, `nome`, `login`) VALUES(?)";
    /* 
  const values = [
    req.body.nome,
    req.body.login,
    req.body.senha,
    req.body.recognition1,
  ];*/

  db.query(q, [values], (err) => {
    if (err) return res.json(err);

    return res.status(200).json("Usuário criado com sucesso.");
  });
};

const deleteUser = (req, res) => {
  const q = "DELETE FROM usuarios WHERE `id` = ?";

  db.query(q, [req.params.id], (err) => {
    if (err) return res.json(err);

    return res.status(200).json("Usuário deletado com sucesso.");
  });
};

module.exports = {
getUsers,
addUser,
deleteUser
};
