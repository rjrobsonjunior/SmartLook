const { db } = require('../db.js');

const getUsers = (_, res) => {
  const q = "SELECT * FROM presents";

  db.query(q, (err, data) => {
      if(err) return res.json(err);
      
      return res.status(200).json(data);
  })
};

const addUser = (req, res) => {  
  
  const { id, nome, login } = req.body;
  const sql = 'INSERT INTO presents (id, nome, login) VALUES (?, ?, ?)';
  const values = [id, nome, login];
        
  db.query(sql, values, (err, result) => {
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
};

const deleteUser = (req, res) => {
  const q = "DELETE FROM presents WHERE `id` = ?";
  console.log("Estou no delete user!");

  db.query(q, [req.params.id], (err) => {
    if (err) return res.status(400).json(err);

    return res.status(200).json("Usuário deletado com sucesso.");
  });
};

module.exports = {
getUsers,
addUser,
deleteUser
};
