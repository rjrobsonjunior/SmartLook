const express = require('express');
const bodyParser = require('body-parser');

const router = express.Router();
router.use(bodyParser.json());

let numberP = 0;

router.post('/pessoas', (req, res) => {
    const { pessoas } = req.body;

    numberP += parseInt(pessoas);
    console.log(numberP);
    res.json({ success: true });
});

router.post('/pessoas-sairam', (req, res) => {
  const { pessoas } = req.body;

  numberP -= parseInt(pessoas);
  console.log(numberP);
  res.json({ success: true });
});

router.get('/setNumber', (req, res) => {
  res.json({number: numberP });
});

module.exports = router;