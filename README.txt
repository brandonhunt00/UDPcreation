# Projeto de Comunicação com Protocolo UDP Simulado via Sockets RAW

Este projeto implementa um protocolo "UDP" personalizado utilizando sockets RAW no C, permitindo comunicação entre um cliente e um servidor. O projeto inclui o cálculo e a verificação de checksums para garantir a integridade dos pacotes, além da verificação do tamanho do pacote.
Estrutura do Projeto

O projeto consiste em dois programas principais:

    Cliente: Envia pacotes para o servidor, construindo manualmente o cabeçalho IP e simulando o comportamento de um pacote UDP.
    Servidor: Recebe pacotes enviados pelo cliente, verifica o checksum e o tamanho do pacote, e processa os dados.

Arquivos:

    cliente.c: Código do cliente, que constrói o pacote com cabeçalho IP e envia os dados.
    servidor.c: Código do servidor, que recebe o pacote, verifica sua integridade e exibe os dados.

Requisitos

Para compilar e rodar este projeto, você precisará de:

    GCC (compilador C)
    Permissões de superusuário (para usar sockets RAW)
    Sistema operacional baseado em Unix (como Linux ou macOS)

Compilação

Para compilar os arquivos do cliente e do servidor, utilize os seguintes comandos no terminal:

gcc cliente.c -o cliente
gcc servidor.c -o servidor

Execução

Como o uso de sockets RAW requer permissões de superusuário, execute tanto o cliente quanto o servidor com sudo:

Execute o servidor em uma janela do terminal:

sudo ./servidor

Em outra janela do terminal, execute o cliente:

sudo ./cliente

Funcionamento

    O cliente constrói um pacote contendo um cabeçalho IP e um payload simulando o protocolo UDP.
        O checksum é calculado tanto para o cabeçalho IP quanto para o payload UDP.
        O pacote é enviado para o servidor através de um socket RAW.
    O servidor recebe o pacote através de um socket RAW.
        O checksum é verificado para garantir a integridade do pacote.
        O tamanho do pacote também é verificado para assegurar que ele foi transmitido corretamente.
        Se o pacote for válido, os dados são exibidos no terminal.

Validação de Integridade

O servidor verifica a integridade dos pacotes recebidos da seguinte forma:

    Checksum: O checksum do pacote UDP é recalculado e comparado com o valor enviado pelo cliente.
    Tamanho do pacote: O servidor verifica se o tamanho total do pacote (cabeçalho IP + dados UDP) corresponde ao valor esperado.

Se houver algum erro no checksum ou no tamanho do pacote, o servidor exibirá uma mensagem de erro e descartará o pacote.

Exemplo de Saída
Cliente:

Pacote enviado com sucesso!

Servidor (caso de sucesso):

Pacote recebido!
Origem: 12345
Destino: 8080
Dados: Olá, este é um pacote UDP simulado!

Servidor (caso de erro de checksum ou tamanho):

Erro: Checksum incorreto!
Erro: Tamanho do pacote incorreto!

Contribuições

Contribuições são bem-vindas! Se você encontrar algum bug ou tiver sugestões de melhorias, sinta-se à vontade para criar um pull request ou abrir uma issue.
