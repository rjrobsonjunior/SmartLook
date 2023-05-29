import GlobalStyle  from "./styles/global";
import styled from "styled-components";
import Form from "./componets/Form.js";
import Grid from "./componets/Grid.js";
import Dashboard from "./componets/Dashboard";
import WebcamCapture from "./componets/Video";
import { useEffect, useState } from "react";
import { toast, ToastContainer } from "react-toastify";
import "react-toastify/dist/ReactToastify.css";
import axios from "axios";


const Container = styled.div`
  width: 100%;
  max-width: 900px;
  margin-top: 20px;
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 10px;
`;

const Title = styled.h2``;

function App() {

  const [users, setUsers] = useState([]);
  const [onEdit, setOnEdit] = useState(null);

  const [faceFeatures, setFaceFeatures] = useState({});
  const [faceDescription, setFaceDescription] = useState('');
  
  const getUsers = async () => {
    try {
      const res = await axios.get("http://localhost:8800/");
      setUsers(res.data.sort((a, b) => (a.nome > b.nome ? 1 : -1)));
    } catch (error) {   
      toast.error(error);
    }
  };

  useEffect(() => {
    getUsers();
  }, [setUsers]);

  return (
    <>
      <Container>
        <Title>USUARIOS</Title>
        <WebcamCapture setFaceFeatures={setFaceFeatures} />
        <Form onEdit={onEdit} setOnEdit={setOnEdit} getUsers={getUsers} faceDescription={faceDescription} setFaceDescription={setFaceDescription} faceFeatures={faceFeatures} />
        <Grid users={users} setUsers={setUsers} setOnEdit={setOnEdit} />
      </Container>
      <ToastContainer theme="dark" autoClose={300} position={toast.POSITION.BUTTON_LEFT} />
      <GlobalStyle/>
    </>
  );
}

export default App;
