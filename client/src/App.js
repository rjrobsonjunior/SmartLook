import GlobalStyle  from "./styles/global";
import styled from "styled-components";
import Form from "./componets/Form.js"
import { toast, ToastContainer } from "react-toastify";
import "react-toastify/dist/ReactToastify.css";

const Container = styled.div`
  width: 100%;
  max-width: 800px;
  margin-top: 20px;
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 10px;
`;

const Title = styled.h2``;

function App() {
  return (
    <>
      <Container>
        <Title>USUARIOS</Title>
      </Container>
      <ToastContainer autoClose={300} position={toast.POSITION.BUTTON_LEFT} />
      <GlobalStyle/>
    </>
  );
}

export default App;
