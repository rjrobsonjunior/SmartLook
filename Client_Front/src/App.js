import GlobalStyle  from "./styles/global";
import styled from "styled-components";
/*
import Home from "./Pages/Home.js";
import ActiveUsers from "./Pages/ActiveUsers.js";
import Register from "./Pages/Register.js";
*/
import Dashboard from "./componets/Dashboard.js";
import Form from "./componets/Form.js";
import Grid from "./componets/Grid.js";
import WebcamCapture from "./componets/Video.js";
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

const Title = styled.h1``;

const Title1 = styled.h2``;

function App() {

  const [users, setUsers] = useState([]);
  const [onEdit, setOnEdit] = useState(null);

  const [presents, setPresents] = useState([]);

  const [faceFeatures, setFaceFeatures] = useState({});
  const [faceDescription, setFaceDescription] = useState('');
  const [number,setNumber] = useState(0);

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

  const getPresents = async () => {
    try {
      const res = await axios.get("http://localhost:8800/presents");
      setPresents(res.data.sort((a, b) => (a.nome > b.nome ? 1 : -1)));
    } catch (error) {   
      toast.error(error);
    }
  };

  useEffect(() => {
    getPresents();
  }, [setPresents]);

  const getNumber = async () => {
    try {
      const response = await axios.get('http://localhost:8800/setNumber');
      const { number } = response.data;
      setNumber(number);
    } catch (error) {
      console.error('Erro ao obter o número:', error);
    }
  };

  useEffect(() => {
    getNumber();
  }, []);

  return (
    <>
      <Container>
        <Title>Modular Lock </Title>
        <Title1>REGISTER</Title1>
        <WebcamCapture setFaceFeatures={setFaceFeatures} />
        <Form onEdit={onEdit} setOnEdit={setOnEdit} getUsers={getUsers} faceDescription={faceDescription} setFaceDescription={setFaceDescription} faceFeatures={faceFeatures} />
        <Title1>REGISTERED</Title1>
        <Grid users={users} setUsers={setUsers} setOnEdit={setOnEdit} />
        <Title1> USERS CURRENT:  {number}</Title1>
        <Dashboard presents={presents}/>
      </Container>
      <ToastContainer theme="dark" autoClose={300} position={toast.POSITION.BUTTON_LEFT} />
      <GlobalStyle/>
    </>
  );
}

export default App;
