import React, { useRef } from "react";
import styled from "styled-components";

const FormContainer = styled.form`
    display: flex;
    aling-itens: flex-end;
    gap: 10px;
    flex-wrap: wrap;
    background-color: #29292E;
    padding: 20px;
    box-shadow: 0px 0px 5px #323238;
    border-radius: 5px;
`;

const InputArea = styled.div`
    display: flex;
    flex-direction: column;
`;

const Input = styled.input`
  width: 120px;
  padding: 0 10px;
  border: 1px solid #bbb;
  border-radius: 5px;
  height: 40px;
`;
 
const Label = styled.label`
`;

const Button = styled.button`
  padding: 10px;
  cursor: pointer;
  border-radius: 5px;
  border: none;
  background-color: #04D361;
  color: white;
  height: 42px;
`;

const Form = ({ onEdit }) => {

    const ref = useRef();

    return(
        <FormContainer ref={ref}>
            <InputArea>
                <Label>Nome</Label>
                <Imput name="nome"/>
            </InputArea>
            <InputArea>
                <Label>Login</Label>
                <Imput name="login"/>
            </InputArea>
            <InputArea>
                <Label>Senha</Label>
                <Imput name="senha"/>
            </InputArea>
            <InputArea>
                <Label>DataNascimento</Label>
                <Imput name="data_nascimento" type="date" />
            </InputArea>

            <Button type="submit">Salvar</Button>

        </FormContainer>
    )
};

export default Form;
