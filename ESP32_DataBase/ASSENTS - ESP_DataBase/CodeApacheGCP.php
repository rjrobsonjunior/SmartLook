<?php
$servername = "34.95.187.202";
$username = "root";
$password = "12345";
$dbname = "users";

// Cria conexão
$conn = new mysqli($servername, $username, $password, $dbname);

// Verifica conexão
if ($conn->connect_error) {
  die("Conexão falhou: " . $conn->connect_error);
}
echo "Conectado com sucesso!";
?>