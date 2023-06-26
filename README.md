# Fechadura Modular Inteligente

Este trabalho tem como objetivo desenvolver uma fechadura eletrônica controlada por um microcontrolador, que oferece três tipos de acesso: login e senha, reconhecimento facial e código QR. 

Através do uso de microcontroladores, foi construída a estrutura física da fechadura e um banco de dados desenvolvido para armazenar as informações de acesso dos usuários. As informações recebidas pela fechadura são analisadas e comparadas com os dados do banco de dados. 

Além disso, o trabalho propõe a implementação de uma interface web para atualização do banco de dados, monitoramento do acesso à fechadura e facilitar o cadastro de novos usuários. 

Ao final do trabalho, foi obtido um protótipo funcional de uma fechadura eletrônica inteligente, que oferece diferentes formas de acesso e é controlada por meio de microcontroladores. Essa solução visa aumentar a comodidade e a segurança dos usuários, tornando o processo de abertura de portas mais automatizadas.

## Fluxograma do fluxo de liberação

![_Fluxograma - Software](https://github.com/rjrobsonjunior/OficinadeIntegracao1/assets/70554459/bf830aab-70fa-4e5d-8ac9-89901b943bea)


## Reconhecimento facial

Para realizar a liberação por reconhecimento facial, é necessário utilizar um algoritmo treinado para essa tarefa. No caso do servidor desenvolvido em JavaScript, a biblioteca escolhida foi a ’face-api.js’, que é escrita na mesma linguagem, facilitando sua integração. Essa biblioteca é construída sobre a API principal do ’tensorflow.js’, outra biblioteca que permite a execução de tarefas de aprendizado de máquina e inferência em tempo real diretamente no navegador ou em ambientes JavaScript.


## Aplicação WEB (Front-End)

Tem como funcionalidade efetuar o cadastro e remoção de usuários, bem como, permitir o controle dos presentes no ambiente controlado. 

![image](https://github.com/rjrobsonjunior/OficinadeIntegracao1/assets/70554459/bf55c251-5ca6-46ed-9914-79e95c01cbe3)

## Servidor nodeJS (Back-End)

O Back-End foi responsável por processar as solicitações feitas tanto pela aplicação web quanto pelos embarcados, executar a lógica necessária para atender a essas solicitações e retornar os resultados apropriados. Ele lidou com a segurança, autenticação de usuários e outras tarefas relacionadas ao controle de entradas e saídas. A figura abaixo mostra as rotas de comunicação estabelecidas possibilitando o bom funcionamento da fechadura.

![image](https://github.com/rjrobsonjunior/OficinadeIntegracao1/assets/70554459/394535c7-79dd-46e8-bb20-fd30e6c1dad5)

## Páginas auxiliares

Com o intuito de otimizar o controle e a interação dos usuários com o projeto, foi elaborada uma solução composta por duas páginas web distintas, cada uma desempenhando um papel fundamental.

### Pagina 1 - Conferência de Imagem

A primeira página Web permite que os usuários visualizem a fotografia capturada pelo dispositivo ESPCAM e verifiquem se a imagem está adequada para ser enviada ao sistema de reconhecimento facial, garantindo que o rosto esteja claramente visível e bem posicionado na fotografia, ponto crucial para uma análise precisa e confiável.

Caso a fotografia não atenda aos critérios estabelecidos, o usuário tem a opção de realizar novamente a captura da foto, até que se encontre uma adequada para se continuar o processo. 

Se, ainda sim, o ESPCACM nao consiga capturar uma imagem adequada, o usuário tem a opção de realizar o upload de uma foto contendo o rosto que o mesmo quer enviar para o reconhecimento. 

Essa abordagem oferece aos usuários um controle mais preciso e eficiente sobre o processo de autenticação facial, ao mesmo tempo que auxilia no desacoplamento da obtenção de fotos através do ESPCAM, que por sua vez apresentou algumas falhas na captura de fotos em nosso testes.

![image](https://github.com/rjrobsonjunior/OficinadeIntegracao1/assets/70554459/9617329b-339d-4e46-9e80-171281fc735c)



### Pagina 2 - Qr Code

A segunda página web foi projetada para permitir que o usuário faça o upload do código QR obtido durante o processo de cadastro. Nessa etapa, é possível visualizar a imagem selecionada antes de enviá-la para o servidor. Essa funcionalidade proporciona uma autenticação rápida e confiável, garantindo a integridade e proteção das informações sensíveis do usuário.

![image](https://github.com/rjrobsonjunior/OficinadeIntegracao1/assets/70554459/fe704dfd-0cff-4b32-bd8b-d284a2249937)


## Protótipo Final

![Porta](https://github.com/rjrobsonjunior/OficinadeIntegracao1/assets/70554459/e0d314cd-0e24-4825-8aec-82427308be97)

## Video demonstrativo

https://www.youtube.com/watch?v=dbPdomMdL2M








