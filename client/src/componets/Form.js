import React, { useEffect, useRef } from "react";
import styled from "styled-components";
import axios from "axios";
import { toast } from "react-toastify";

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

const Form = ({ getUsers, onEdit, setOnEdit}) => {

    const ref = useRef();

    useEffect(() => {
        if (onEdit) {
          const user = ref.current;
    
          user.nome.value = onEdit.nome;
          user.login.value = onEdit.login;
          user.senha.value = onEdit.senha;
          user.data_nascimento.value = onEdit.data_nascimento;
        }
      }, [onEdit]);

      
    const handleSubmit = async (e) => {
        e.preventDefault();

        const user = ref.current;

        if (
            !user.nome.value ||
            !user.login.value ||
            !user.senha.value ||
            !user.data_nascimento.value
        ) {
            return toast.warn("Preencha todos os campos!");
        }

        if (onEdit) {
            await axios
            .put("http://localhost:8800/" + onEdit.id, {
                nome: user.nome.value,
                login: user.login.value,
                senha: user.senha.value,
                data_nascimento: user.data_nascimento.value,
            })
            .then(({ data }) => toast.success(data))
            .catch(({ data }) => toast.error(data));
        } else {
            await axios
            .post("http://localhost:8800", {
                nome: user.nome.value,
                login: user.login.value,
                senha: user.senha.value,
                data_nascimento: user.data_nascimento.value,
            })
            .then(({ data }) => toast.success(data))
            .catch(({ data }) => toast.error(data));
        }

        user.nome.value = "";
        user.login.value = "";
        user.senha.value = "";
        user.data_nascimento.value = "";

        setOnEdit(null);
        getUsers();
    };   

    return(
        <FormContainer ref={ref} onSubmit={handleSubmit}>
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