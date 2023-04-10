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
  width:  120px;
  padding: 0 10px;
  border: 1px solid #bbb;
  border-radius: 5px;
  height: 40px;
  color: white;
  background-color: #070707;
`;
 
const Label = styled.label``;

const Button = styled.button`
  padding: 20px;
  cursor: pointer;
  border-radius: 5px;
  border: none;
  background-color: #04D361;
  color: white;
  height: 62px;
`;

const Form = ({ onEdit }) => {

    const ref = useRef();

    return(
        <FormContainer ref={ref}>
            <InputArea>
                <Label>Nome</Label>
                <Input name="nome"/>
            </InputArea>
            <InputArea>
                <Label>Login</Label>
                <Input name="login"/>
            </InputArea>
            <InputArea>
                <Label>Senha</Label>
                <Input name="senha"/>
            </InputArea>
            <InputArea>
                <Label>Data de Nascimento</Label>
                <Input name="data_nascimento" type="date" />
            </InputArea>

            <Button type="submit">Salvar</Button>

        </FormContainer>
    )
};

export default Form;
