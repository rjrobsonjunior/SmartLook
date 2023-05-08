import React, { useEffect, useRef } from "react";
import styled from "styled-components";
import axios from "axios";
import { toast } from "react-toastify";

const FormContainer = styled.form`
    width: 100%;
    display: flex;
    aling-itens: flex-end;
    flex-wrap: wrap;
    background-color: #29292E;
    padding: 20px;
    max-width: 1120px;
    margin: 20px auto;
    box-shadow: 0px 0px 5px #323238;
    border-radius: 5px;
`;

const InputArea = styled.div`
    display: flex;
    flex-direction: column;
`;

const Input = styled.input`
    padding: 0 10px;
    border: 1px solid #bbb;
    border-radius: 5px;
    height: 40px;
    color: white;
    background-color: #070707;
`;
 
const Label = styled.label`
    text-align: center;
`;

const Button = styled.button`
    padding: 10px;
    cursor: pointer;
    border-radius: 5px;
    border: none;
    background-color: #04D361;
    color: white;
    height: 62px;
    font-size:16px;
`;

const Form = ({ getUsers, onEdit, setOnEdit}) => {

    const ref = useRef();

    useEffect(() => {
        if (onEdit) {
          const user = ref.current;
    
          user.nome.value = onEdit.nome;
          user.login.value = onEdit.login;
          user.senha.value = onEdit.senha;
          user.recognition1.value = onEdit.recognition1;
          user.recognition2.value = onEdit.recognition2;
          user.recognition3.value = onEdit.recognition3;
          user.recognition4.value = onEdit.recognition4;
          user.recognition5.value = onEdit.recognition5;
        }
      }, [onEdit]);


    const handleSubmit = async (e) => {
        e.preventDefault();

        const user = ref.current;

        if (
            !user.nome.value ||
            !user.login.value ||
            !user.recognition1.value ||
            !user.recognition2.value ||
            !user.recognition3.value ||
            !user.recognition4.value ||
            !user.recognition5.value 
        ) {
            return toast.warn("Preencha todos os campos!");
        }

        if (onEdit) {
            await axios
            .put("http://localhost:8800/" + onEdit.id, {
                nome: user.nome.value,
                login: user.login.value,
                senha: user.senha.value,
                recognition1: user.recognition1.value,
                recognition2: user.recognition2.value,
                recognition3: user.recognition3.value,
                recognition4: user.recognition4.value,
                recognition5: user.recognition5.value,
            })
            .then(({ data }) => toast.success(data))
            .catch(({ data }) => toast.error(data));
        } 
        else {
            await axios
            .post("http://localhost:8800", {
                nome: user.nome.value,
                login: user.login.value,
                senha: user.senha.value,
                recognition1: user.recognition1.value,
                recognition2: user.recognition2.value,
                recognition3: user.recognition3.value,
                recognition4: user.recognition4.value,
                recognition5: user.recognition5.value,
            })
            .then(({ data }) => toast.success(data))
            .catch(({ data }) => toast.error(data));
        }

        user.nome.value = "";
        user.login.value = "";
        user.senha.value = "";
        user.recognition1.value = "";
        user.recognition2.value = "";
        user.recognition3.value = "";
        user.recognition4.value = "";
        user.recognition5.value = "";

        setOnEdit(null);
        getUsers();
    };   

    return(
        <FormContainer ref={ref} onSubmit={handleSubmit}>
            <InputArea>
                <Label>Nome</Label>
                <Input name="nome" width="20%"/>
            </InputArea>
            <InputArea >
                <Label>Login</Label>
                <Input name="login" />
            </InputArea>
            <InputArea>
                <Label>Senha</Label>
                <Input name="senha" type="text"/>
            </InputArea>
            <InputArea>
                <Label>1</Label>
                <Input name="recognition1" type="checkbox"  className="checkbox-input"/>
            </InputArea>
            <InputArea>
                <Label>2</Label>
                <Input name="recognition2" type="checkbox" className="checkbox-input"/>
            </InputArea>
            <InputArea>
                <Label>3</Label>
                <Input name="recognition3" type="checkbox" className="checkbox-input"/>
            </InputArea>
            <InputArea>
                <Label>4</Label>
                <Input name="recognition4" type="checkbox" className="checkbox-input"/>
            </InputArea>
            <InputArea>
                <Label>5</Label>
                <Input name="recognition5" type="checkbox" className="checkbox-input"/>
            </InputArea>

            <Button type="submit">SALVAR</Button>

        </FormContainer>
    )
};

export default Form;
