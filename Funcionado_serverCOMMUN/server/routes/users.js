/*
import express from 'express';
import { addUser, deleteUser, getUsers, updateUser } from "../controllers/user.js";
*/

const express = require('express');
const {
  addUser,
  deleteUser,
  getUsers,
  updateUser
} = require('../controllers/user.js');

const router = express.Router();

router.get("/", getUsers);
router.post("/", addUser);
router.put("/:id", updateUser);
router.delete("/:id", deleteUser);

//export default router;
module.exports = router;
