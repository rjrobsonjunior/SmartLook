<?php
/*
* Change the value of $password if you have set a password on the root userid
* Change NULL to port number to use DBMS other than the default using port 3306
*
*/
$user = 'root';
$password = ''; //To be completed if you have set a password to root
$database = 'teste2'; //To be completed to connect to a database. The database must exist.
$port = 3306; //Default must be NULL to use default port
$mysqli = new mysqli('127.0.0.1', $user, $password, $database, $port);

if ($mysqli->connect_error) {
    die('Connect Error (' . $mysqli->connect_errno . ') '
            . $mysqli->connect_error);
}
echo '<p>Connection OK '. $mysqli->host_info.'</p>';
echo '<p>Server '.$mysqli->server_info.'</p>';
echo '<p>Initial charset: '.$mysqli->character_set_name().'</p>';

// recebe o login enviado via HTTP GET
$login = $_GET['login'];

// realiza a consulta no banco de dados para buscar a senha correspondente
$sql = "SELECT senha FROM pessoas WHERE login = '$login'";
$sql2 = "SELECT nome FROM pessoas WHERE login = '$login'";

$result = mysqli_query($mysqli, $sql);
$result_nome = mysqli_query($mysqli, $sql2);

if ($result && mysqli_num_rows($result) > 0) 
{

    $row_senha = mysqli_fetch_assoc($result);
    $senha = $row_senha['senha'];
    echo $senha;
    
    /*
    $row_nome = mysqli_fetch_assoc($result_nome);
    $nome = $row_nome['nome'];
    echo "Nome: " . $nome . "<br>";
    */
    
} else {
    echo "Login não encontrado ou senha não cadastrada";
    http_response_code(404);
}


$mysqli->close();
?>
