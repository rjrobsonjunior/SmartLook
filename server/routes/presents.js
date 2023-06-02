const express = require('express');
const {
  getUsers,
  addUser,
  deleteUser
} = require('../controllers/present.js');

const router = express.Router();

router.get("/presents", getUsers);
router.post("/presents", addUser);
//router.delete("/presents:id", deleteUser);
router.get("/presents/:id", deleteUser);

//export default router;
module.exports = router;