import React from "react";
import styled from "styled-components";

const Table = styled.table`
  width: 100%;
  background-color: #29292E;
  padding: 20px;
  box-shadow: 0px 0px 5px #323238;
  border-radius: 5px;
  max-width: 1120px;
  margin: 20px auto;
  word-break: break-all;
`;
export const Thead = styled.thead``;

export const Tbody = styled.tbody``;

export const Tr = styled.tr``;

export const Th = styled.th`
  text-align: start;
  border-bottom: inset;
  padding-bottom: 5px;
  @media (max-width: 500px) {
    ${(props) => props.onlyWeb && "display: none"}
  }
`;

export const Td = styled.td`
  padding-top: 15px;
  text-align: ${(props) => (props.alignCenter ? "center" : "start")};
  width: ${(props) => (props.width ? props.width : "auto")};
  @media (max-width: 500px) {
    ${(props) => props.onlyWeb && "display: none"}
  }
`;

const Dashboard = ({presents}) => {

  return(
      <Table>
          <Thead>
            <Tr>
                <Th>ID</Th>
                <Th>Nome</Th>
                <Th>Login</Th>
            </Tr>
          </Thead>
          <Tbody>
              {presents.map((item, i) => (
              <Tr key={i}>
                <Td width="20%">{item.id}</Td>
                <Td width="40%">{item.nome}</Td>
                <Td width="20%">{item.login}</Td>
              </Tr>
              ))}
          </Tbody>
      </Table>
  );
};

export default Dashboard;