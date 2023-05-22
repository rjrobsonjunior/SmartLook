import express from 'express';
import multer from 'multer';

const router = express.Router();

const storage = multer.diskStorage({
  destination: function (req, file, cb) {
    cb(null, './routes/uploads')
  },
  filename: function (req, file, cb) {
    cb(null, file.originalname)
  }
});

const upload = multer({ storage: storage });

router.post('/upload', upload.single('image'), function (req, res) {
  
  if (req.file) {
    res.send("Single file uploaded successfully");
  } 
  else {
    res.status(400).send("Please upload a valid image");
  }
})

export default router;