import React, { useState, useEffect } from 'react';
import Webcam from 'react-webcam';
import * as faceapi from 'face-api.js';
import styled from 'styled-components';
//import "./Checkbox.css";
const MODEL_URL = './models'


const WebcamContainer = styled.div`
  border-radius: 10px;
  text-align: center;
`;
const WebcamVideo = styled.video`
  width: 60%;
  height: 60%;
  object-fit: cover;
  border-radius: 10px;
  box-shadow: 2px 2px 5px rgba(255, 255, 255, 1);
`;

const Button = styled.button`
  color: #04D361;
  margin: 0 auto;
  display: block;
  padding: 10px;
  border-radius: 5px;
  border: none;
  background-color: #29292E;
  height: 50px;
  font-size: 16px;
  cursor: pointer;
`;

const WebcamCapture = () => {
  const [image, setImage] = useState(null);
  const [faceDetected, setFaceDetected] = useState(false);
  
  //load face detection models
  useEffect(() => {
    //console.log("carregando modelos");
    const loadModels = async () => {
      await faceapi.nets.ssdMobilenetv1.loadFromUri(MODEL_URL);
      await faceapi.nets.faceLandmark68Net.loadFromUri(MODEL_URL);
      await faceapi.nets.faceRecognitionNet.loadFromUri(MODEL_URL);
    };
    loadModels();
    //console.log("modelos carregados");
  }, []);
  
  // Função para processar a imagem capturada
  const capture = async () => {
    const imageSrc = webcamRef.current.getScreenshot();
    setImage(imageSrc);
    const imageElement = document.createElement('img');
    imageElement.src = imageSrc;
    
    // Detecta a face na imagem usando a face-api.js
    const detections = await faceapi.detectAllFaces(imageElement).withFaceLandmarks().withFaceDescriptors();
    
    // Verifica se uma face foi detectada na imagem
    if (detections.length > 0) {
      setFaceDetected(true);
      // Salva a face detectada no banco de dados
      // ...
    } else {
      setFaceDetected(false);
    }
    console.log(setFaceDetected);
  };
  
  // Referência para o componente Webcam
  const webcamRef = React.useRef(null);
  
  return (
    <div>
      <Webcam
        audio={false}
        ref={webcamRef}
        screenshotFormat="image/jpeg"
      />
      <Button onClick={capture}>CAPTURE</Button>
      {faceDetected && <p>Face detectada!</p>}
      {image && <img src={image} alt="captured" />}
    </div>
  );
};

export default WebcamCapture;
