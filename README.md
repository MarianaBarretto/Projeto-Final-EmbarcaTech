<h1>Sistema de Notificação Visual para Campainha  - Embarcatech 💻</h1>

   <p><strong>Descrição:</strong></p>
   <p>Este projeto foi desenvolvido com o objetivo de proporcionar acessibilidade para pessoas com deficiência auditiva, permitindo que elas possam identificar quando uma campainha está tocando. O sistema utiliza um microfone para detectar o som da campainha e fornece feedback visual, acendendo um LED, exibindo a mensagem "Campainha Tocando" no display OLED e mostrando uma carinha feliz na matriz de LEDs 5x5. Também há a opção de apagar os LEDs e o display utilizando um botão.</p>

   <h2>Hardware Utilizado</h2>
    <ul>
        <li><strong>Microfone</strong> (conectado no pino 28)</li>
        <li><strong>LED Azul</strong> (conectado no pino 12)</li>
        <li><strong>Botão A</strong> (conectado no pino 5)</li>
        <li><strong>Matriz de LEDs 5x5</strong> (controlada via PIO e conectada ao pino 7)</li>
        <li><strong>Display OLED (SSD1306)</strong> (controlado via I2C nos pinos SDA (14) e SCL (15))</li>
        <li><strong>Raspberry Pi Pico</strong> (microcontrolador)</li>
    </ul>

   <h2>Funcionalidades</h2>
    <p>1. <strong>Detecção de Pico de Som</strong>: O sistema utiliza um microfone conectado ao ADC da Raspberry Pi Pico para detectar picos de som característicos de uma campainha.</p>
    <p>2. <strong>Feedback Visual</strong>:
        <ul>
            <li>Ao detectar o som da campainha, um LED é aceso.</li>
            <li>A mensagem "Campainha Tocando" é exibida no display OLED.</li>
            <li>Uma carinha feliz é exibida na matriz de LEDs 5x5.</li>
        </ul>
    </p>
    <p>3. <strong>Controle do LED via Botão</strong>: O LED pode ser desligado ao pressionar o botão A. O display OLED e a matriz de LEDs também são apagados nesse caso.</p>
    <p>4. <strong>Média Móvel</strong>: O sistema utiliza um filtro de média móvel para suavizar os valores do microfone e detectar picos de som com maior precisão.</p>

   <h2>Estrutura do Código</h2>
    <p>O código está organizado da seguinte forma:</p>
    <ul>
        <li><strong>Definições de Pinos</strong>: Configuração dos pinos GPIO, I2C e ADC.</li>
        <li><strong>Inicialização do ADC</strong>: Configuração para leitura do microfone analógico.</li>
        <li><strong>Controle do LED</strong>: Funções para ligar/desligar o LED azul.</li>
        <li><strong>Display OLED</strong>: Funções para inicializar e atualizar o display.</li>
        <li><strong>Matriz de LEDs</strong>: Funções para controlar a exibição de padrões na matriz de LEDs.</li>
        <li><strong>Detecção de Pico de Som</strong>: Lógica para detectar picos de som usando média móvel.</li>
        <li><strong>Interrupção de Botão</strong>: Configuração para detectar pressões do botão A e apagar os LEDs e o display.</li>
    </ul>

   <h2>Funcionalidades Detalhadas</h2>

   <h3>Detecção de Pico de Som</h3>
    <p>O valor de áudio do microfone é lido constantemente pelo ADC.</p>
    <p>O valor é suavizado usando um filtro de média móvel com uma amostra de tamanho definido.</p>
    <p>Quando o valor suavizado ultrapassa um limiar pré-definido, o sistema detecta um pico de som e acende o LED azul, exibindo a mensagem no display OLED e a carinha feliz na matriz de LEDs.</p>

   <h3>Interrupção do Botão</h3>
    <p>O botão A (GPIO5) é configurado para gerar uma interrupção na borda de queda (quando pressionado).</p>
    <p>Quando pressionado, o botão desliga o LED azul, apaga a mensagem no display OLED e apaga a matriz de LEDs.</p>

   <h2>Como Compilar e Rodar o Código</h2>
    <p>1. <strong>Instalar o SDK da Raspberry Pi Pico</strong>: Para compilar o código, é necessário configurar o SDK da Raspberry Pi Pico. Siga o guia oficial de configuração do SDK <a href="https://github.com/raspberrypi/pico-sdk" target="_blank">aqui</a>.</p>
    <p>2. <strong>Compilar o código</strong>: Compile o código utilizando CMake. Assegure-se de que o arquivo <code>CMakeLists.txt</code> esteja corretamente configurado para o projeto.</p>
    <p>3. <strong>Carregar o código na Raspberry Pi Pico</strong>: Após a compilação, use o método de upload (geralmente pressionando o botão BOOTSEL na Pico e conectando via USB).</p>
    <p>4. <strong>Execução</strong>: Após o código ser carregado, o sistema começará a monitorar o microfone e fornecer feedback visual conforme a detecção do som da campainha.</p>

   <h2>Dependências</h2>
    <ul>
        <li><strong>SSD1306</strong>: Biblioteca para controle do display OLED via I2C.</li>
        <li><strong>PIO (Programmable Input/Output)</strong>: Utilizado para controlar a matriz de LEDs WS2812.</li>
        <li><strong>ADC</strong>: Biblioteca para controle do ADC (Conversor Analógico-Digital).</li>
    </ul>
   
 <h2>Imagem</h2>
<p align="center">
    <img src="https://github.com/MarianaBarretto/Projeto-Final-EmbarcaTech/blob/main/Imagem/Circuito%20Completo.jpg" alt="Circuito Completo" />
</p>

<h3>Vídeo de Demostração</h3>
<p>Google Drive: https://drive.google.com/file/d/1q6poX7Lsi-FKz9LFSryedhb0u-zUus99/view?usp=sharing</p>
<p>YouTube: https://youtu.be/vhKtg6NUxIk</p>

<h4>Autor: <strong>Mariana Barretto</strong></h4>
