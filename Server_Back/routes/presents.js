const express = require('express');
const {
  getUsers,
  addUser,
  deleteUser
} = require('../controllers/present.js');

const router = express.Router();

router.get("/presents", getUsers);
router.get("/insert/:id/:nome/:login", addUser);
//router.get("/insert", addUser);

//router.delete("/presents:id", deleteUser);
router.get("/delete/:id", deleteUser);

//export default router;
module.exports = router;