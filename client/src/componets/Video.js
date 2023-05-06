import React, { useState } from "react";
import Webcam from "react-webcam";
import Cropper from "react-image-crop";
import styled from 'styled-components';

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
`;

const WebcamCapture = () => {
  const [capturing, setCapturing] = useState(false);
  const [crop, setCrop] = useState({ aspect: 1 / 1 });
  const [image, setImage] = useState(null);

  const handleCapture = () => {
    setCapturing(true);
  };

  const handleRetake = () => {
    setCapturing(false);
    setImage(null);
  };

  const handleCrop = (croppedImage) => {
    setImage(croppedImage);
    setCapturing(false);
  };

  return (
    <WebcamContainer>
      <Webcam/>
      <Button onClick={handleCapture}>CAPTURE</Button>
    </WebcamContainer>
  );
};

export default WebcamCapture;