import React, { useState } from "react";
import Webcam from "react-webcam";
import Cropper from "react-image-crop";

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
    <div>
      {capturing ? (
        <div>
          <Webcam />
          <button onClick={handleCapture}>Capture</button>
        </div>
      ) : (
        <div>
          {image ? (
            <div>
              <Cropper src={image} crop={crop} onChange={setCrop} />
              <button onClick={() => handleCrop(croppedImage)}>Save</button>
              <button onClick={handleRetake}>Retake</button>
            </div>
          ) : (
            <button onClick={handleCapture}>Start Capture</button>
          )}
        </div>
      )}
    </div>
  );
};

export default WebcamCapture;