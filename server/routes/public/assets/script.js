function obterDataModificacaoFoto() {
    var xhr = new XMLHttpRequest();
    xhr.open('GET', '/dataModificacaoFoto', true);
  
    xhr.onload = function() {
      if (xhr.status === 200) {
        var dataModificacao = JSON.parse(xhr.responseText).dataModificacao;
  
        document.getElementById('data-modificacao').textContent = dataModificacao;
      }
    };
  
    xhr.onerror = function() {
      console.error('Erro na requisição');
    };
  
    xhr.send();
}
  
// Chamar a função para obter a data de modificação da foto ao carregar a página
window.addEventListener('DOMContentLoaded', obterDataModificacaoFoto);

function tirarFoto() {

    //Apenas manda uma requisição get para o espcam tirar uma nova foto

    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function() {
        if (xhr.readyState === 4) {
            if (xhr.status === 200) {

                exibirContador();
                
                setTimeout(function() {
                    exibirPopup("Foto capturada com sucesso!");
                }, 5000);

            } 
            else {
                exibirPopup("Falha no envio da requisição!");
            }
        }
    };

    xhr.open("GET", "http://192.168.0.8/capture", true);
    xhr.send();

    
}

function enviarParaAnalise() {

    //Manda uma requisição get ao servidor nodeJS que muda uma variavel global para true;
    //Essa mudança permite com que a requisição continue

    var xhr = new XMLHttpRequest();
    xhr.open('GET', '/continuarProcessamentoFace', true);

    xhr.onload = function() {
    if (xhr.status >= 200 && xhr.status < 400) {
        exibirPopup('Foto enviada com sucesso!');
    } else {
        exibirPopup('Erro na requisição!');
        
    }
    };

    xhr.onerror = function() {
        exibirPopup('Erro na requisição!');
    };

    xhr.send();

}

function exibirContador() {
    var counterContainer = document.getElementById("counterContainer");
    counterContainer.classList.add("show");

    var counter = 5;
    var counterElement = document.getElementById("counter");
    counterElement.textContent = counter;

    var countdown = setInterval(function() {
        counter--;
        counterElement.textContent = counter;

        if (counter <= 0) {
            clearInterval(countdown);
            counterContainer.classList.remove("show");
        }
    }, 1000);

}


function exibirPopup(texto) {
    var popupContainer = document.getElementById("popupContainer");
    var popupText = document.getElementById("popupText");
    popupText.textContent = texto;
    popupContainer.classList.add("show");
}

function exibirPopupFoto(texto) {
    var popupContainer = document.getElementById("popupContainerFoto");
    var popupText = document.getElementById("popupText");
    popupText.textContent = texto;
    popupContainer.classList.add("show");
}

function fecharPopupFoto() {
    var popupContainer = document.getElementById("popupContainerFoto");
    popupContainer.classList.remove("show");

}

function fecharPopup() {
    var popupContainer = document.getElementById("popupContainer");
    popupContainer.classList.remove("show");

}

function selecionarImagem() {
    var uploadInput = document.getElementById("uploadInput");
    uploadInput.click();
}

var foto;

document.getElementById("uploadInput").addEventListener("change", function() {
    foto = this.files[0];
    //enviarImagemParaServidor(file);
    exibirPopupFoto("Deseja enviar a foto?");
});


function enviarImagemParaServidor() {
    
    if(foto){
        var formData = new FormData();
        formData.append("imagem", foto);

        var request = new XMLHttpRequest();
        request.open("POST", "/foto", true);
        request.onreadystatechange = function() {
            if (request.readyState === 4 && request.status === 200) {
                exibirPopup("Enviado com sucesso!");
            }
        };
        request.send(formData);
    }
    else
    {
        exibirPopup("Nao há foto para enviar!");
    }

}