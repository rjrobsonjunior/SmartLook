const { db } = require('../db.js');

const getUsers = (_, res) => {
  const q = "SELECT * FROM usuarios";

  db.query(q, (err, data) => {
      if(err) return res.json(err);
      
      return res.status(200).json(data);
  })
};

const addUser = (req, res) => {
  const q =  "INSERT INTO usuarios(`nome`, `login`, `senha`, `recognition1`) VALUES(?)";

  const values = [
    req.body.nome,
    req.body.login,
    req.body.senha,
    req.body.recognition1,
  ];

  db.query(q, [values], (err) => {
    if (err) return res.json(err);

    return res.status(200).json("Usuário criado com sucesso.");
  });
};

const updateUser = (req, res) => {
  const q = "UPDATE usuarios SET `nome` = ?, `login` = ?, `senha` = ?, `recognition1` = ? WHERE `id` = ?";

  const values = [ 
    req.body.nome,
    req.body.login,
    req.body.senha,
    req.body.recognition1,
  ];

  db.query(q, [...values, req.params.id], (err) => {
    if (err) return res.json(err);

    return res.status(200).json("Usuário atualizado com sucesso.");
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
updateUser,
deleteUser
};