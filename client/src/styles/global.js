import { createGlobalStyle } from "styled-components";

const Global = createGlobalStyle`

    * {
        margin: 0;
        padding: 0;
        font-family: 'poppins', sans-serif;
        color: white;
    }

    body {
        width: 100%;
        heigth: 100%;
        display: flex;
        justify-content: center;
        background-color: #070707;
    }
`;

export default Global;