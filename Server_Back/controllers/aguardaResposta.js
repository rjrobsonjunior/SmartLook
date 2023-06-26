//Controle de espera da resposta do usuario em uma aplicação web

let pode_continuar = false;
const tempoMaximoEspera = 600000; //(10 minutos) 
const intervaloVerificacao = 500; // (0,5 segundo)

function formatarTempo(tempo) {
    const minutos = Math.floor(tempo / 60000); // Extrai os minutos
    const segundos = Math.floor((tempo % 60000) / 1000); // Extrai os segundos
  
    const minutosFormatados = minutos.toString().padStart(2, '0'); // Formata os minutos com 2 dígitos
    const segundosFormatados = segundos.toString().padStart(2, '0'); // Formata os segundos com 2 dígitos
  
    return `${minutosFormatados}:${segundosFormatados}`;
}

function aguardarMudancaVariavel() {
  return new Promise((resolve, reject) => {    
    let tempoDecorrido = 0;

    const intervalo = setInterval(() => {
      if (pode_continuar) {
        clearInterval(intervalo);
        console.log();
        resolve();
      } else {
        //process.stdout.write(".");
        tempoDecorrido += intervaloVerificacao;

        const tempoRestante = (tempoMaximoEspera - tempoDecorrido);
        const tempoRestanteFormatado = formatarTempo(tempoRestante);

        process.stdout.clearLine(); // Limpa a linha anterior
        process.stdout.cursorTo(0); // Move o cursor para o início da linha
        process.stdout.write(`Aguardando o usuario... Tempo restante: ${tempoRestanteFormatado}`);


        if (tempoDecorrido >= tempoMaximoEspera) {
          clearInterval(intervalo);
          reject(
            new Error(
              "\nTempo máximo de espera atingido! A requisição sera finalizada!"
            )
          );
        }
      }
    }, intervaloVerificacao); // Intervalo de verificação (500 milissegundos)
  });
}

module.exports = {
  setPodeContinuar: function (valor) {
    pode_continuar = valor;
  },
  aguardarMudancaVariavel: aguardarMudancaVariavel,
};
