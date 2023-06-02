const { db } = require('../db.js');

const getUsers = (_, res) => {
  const q = "SELECT * FROM presents";

  db.query(q, (err, data) => {
      if(err) return res.json(err);
      
      return res.status(200).json(data);
  })
};

const addUser = (req, res) => {  

  const id = req.body.id;
  const nome = req.body.nome;
  const login = req.body.login;
  
  const q = `INSERT INTO presents('id', 'nome', 'login') VALUES ('${id}', '${nome}', '${login}') `;

  console.log(q);

  db.query(q, [values], (err) => {
    if (err) {
      if (err.code !== 'ER_SIGNAL_EXCEPTION') {
        throw err; // Rethrow non-MySQL errors
      }
      // Handle MySQL errors
    // ...
    return;
    }

    return res.status(200).json("Usuário criado com sucesso.");
  });
};

const deleteUser = (req, res) => {
  const q = "DELETE FROM presents WHERE `id` = ?";

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
