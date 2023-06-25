from pyzbar import pyzbar
import cv2
import argparse

def process_qr_code(image_path, output_path):
    # Carrega a imagem
    image = cv2.imread(image_path)

    # Verifica se a imagem foi carregada corretamente
    if image is None:
        print("Erro ao carregar a imagem.")
        return

    # Converte a imagem para escala de cinza
    #gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

    # Realiza a detecção dos códigos de barras e QR codes na imagem
    decoded_objects = pyzbar.decode(image)

    # Verifica se algum QR code foi detectado
    if decoded_objects:
        qr_code_data = decoded_objects[0].data.decode('utf-8')

        # Salva os dados do QR code no arquivo de saída
        with open(output_path, 'w') as f:
            f.write(qr_code_data)

        print("QR code detectado. Dados: ", qr_code_data)
    else:
        print("QR code não detectado.")

if __name__ == '__main__':
    # Argumentos da linha de comando
    ap = argparse.ArgumentParser()
    ap.add_argument("-i", "--image", required=True, help="Caminho da imagem a ser processada")
    ap.add_argument("-o", "--output", default="resultado.txt", help="Caminho do arquivo de saída")
    args = vars(ap.parse_args())

    # Chama a função para processar o QR code na imagem
    process_qr_code(args["image"], args["output"])
