import * as faceapi from 'face-api.js';
import React, { useState, useEffect, useRef } from 'react';
import styled from 'styled-components';
import Form from './Form';
import Webcam from 'react-webcam';
//import "./Checkbox.css";


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

function WebcamCapture(props) {

  const [image, setImage] = useState(null);
  const [faceDescriptor, setFaceDescriptor] = useState(null);

  const [modelsLoaded, setModelsLoaded] = React.useState(false);
  const [captureVideo, setCaptureVideo] = React.useState(false);

  const webcamRef = React.useRef();
  const canvasRef = React.useRef(null);

  const videoRef = React.useRef(null);
  const videoHeight = 480;
  const videoWidth = 640;

  useEffect(() => {
    const loadModels = async () => {
      const MODEL_URL = process.env.PUBLIC_URL + '/models';
      Promise.all([
        faceapi.loadSsdMobilenetv1Model(MODEL_URL),
        faceapi.loadFaceLandmarkModel(MODEL_URL),
        faceapi.loadFaceRecognitionModel(MODEL_URL),
        faceapi.nets.tinyFaceDetector.loadFromUri(MODEL_URL),
        faceapi.nets.faceLandmark68Net.loadFromUri(MODEL_URL),
        faceapi.nets.faceRecognitionNet.loadFromUri(MODEL_URL),
        faceapi.nets.faceExpressionNet.loadFromUri(MODEL_URL),
      ]).then(setModelsLoaded(true));
    }
    loadModels();
  }, []);

  const startVideo = () => {
    setCaptureVideo(true);
    navigator.mediaDevices.getUserMedia({ video: { width: 300 } })
      .then(stream => {
        let video = videoRef.current;
        video.srcObject = stream;
        video.play();
      })
      .catch(err => {
        console.error("error:", err);
      });
  }

  const handleVideoOnPlay = () => {

    setInterval(async () => {
      
      if (canvasRef && canvasRef.current) {
        canvasRef.current.innerHTML = faceapi.createCanvasFromMedia(videoRef.current);
        const displaySize = {
          width: videoWidth,
          height: videoHeight
        }

        faceapi.matchDimensions(canvasRef.current, displaySize);

        const detections = await faceapi.detectAllFaces(videoRef.current, new faceapi.TinyFaceDetectorOptions()).withFaceLandmarks().withFaceExpressions();

        const resizedDetections = faceapi.resizeResults(detections, displaySize);

        canvasRef && canvasRef.current && canvasRef.current.getContext('2d').clearRect(0, 0, videoWidth, videoHeight);
        canvasRef && canvasRef.current && faceapi.draw.drawDetections(canvasRef.current, resizedDetections);
        canvasRef && canvasRef.current && faceapi.draw.drawFaceLandmarks(canvasRef.current, resizedDetections);
        canvasRef && canvasRef.current && faceapi.draw.drawFaceExpressions(canvasRef.current, resizedDetections);
      }
    }, 100)
  }

  const handleCapture = async () => {
    const video = videoRef.current;
    const canvas = document.createElement('canvas');
    canvas.width = video.videoWidth;
    canvas.height = video.videoHeight;
    const ctx = canvas.getContext('2d');
    ctx.drawImage(video, 0, 0, canvas.width, canvas.height);
    const imgData = canvas.toDataURL('image/png');
    const img = new Image();
    img.src = canvas.toDataURL();
    img.onload = async () => {
      const detections = await faceapi.detectSingleFace(img).withFaceLandmarks().withFaceDescriptor();
      console.log(detections.descriptor);
      setImage(img);
      if (detections) {
        props.setFaceFeatures(detections.descriptor);
      }
    };
    img.src = imgData;
  };

  const closeWebcam = () => {
    videoRef.current.pause();
    videoRef.current.srcObject.getTracks()[0].stop();
    setCaptureVideo(false);
  }

  return (
    <div>
      <div style={{ textAlign: 'center', padding: '10px' }}>
        {
          captureVideo && modelsLoaded ?
            <Button onClick={closeWebcam} >Close Webcam</Button>
            :
            <Button onClick={startVideo}>Open Webcam</Button>
        }
      </div>
      {
        captureVideo ?
          modelsLoaded ?
            <div>
              <div style={{ display: 'flex', justifyContent: 'center', padding: '10px' }}>
                <video ref={videoRef} height={videoHeight} width={videoWidth} onPlay={handleVideoOnPlay} style={{ borderRadius: '10px' }} />
                <canvas ref={canvasRef} style={{ position: 'absolute' }} />
              </div>
              <div>
                <Button onClick={handleCapture}>Capture Image</Button>
                {image && <img src={image} alt="captured" />}
              </div>
            </div>
            :
            <div>loading...</div>
          :
          <>
          </>
      }
    </div>

  );
}


export default WebcamCapture;
