# Manual de Uso — Sistema de Oficina Mecânica
## 1. Sobre o Sistema

Este programa em linguagem C simula o funcionamento básico de uma oficina mecânica, permitindo:
Cadastro, atualização, remoção e listagem de clientes;
Cadastro, atualização, remoção e listagem de veículos;
Abertura, atualização, remoção e listagem de ordens de serviço.
Os dados são armazenados em arquivos de texto (clientes.txt, veiculos.txt, ordens.txt) para garantir que não sejam perdidos ao fechar o programa.

## 2. Requisitos para Compilação

Para compilar o programa, você precisa de um compilador C, como:
GCC (Linux, Windows com MinGW, ou macOS);
Code::Blocks, Dev-C++ ou outro IDE compatível com C.

Arquivos necessários:
O código-fonte principal (oficina.c ou outro nome que você tenha dado)
Nenhuma biblioteca externa é necessária além das padrões da linguagem C (stdio.h, stdlib.h, string.h, locale.h).

## 3. Compilação
No terminal (Linux ou macOS):
gcc oficina.c -o oficina

No Windows (com MinGW):
gcc oficina.c -o oficina.exe

Após a compilação bem-sucedida, será criado um executável chamado oficina (ou oficina.exe no Windows).

## 4. Execução
Linux/macOS:
./oficina

Windows (Prompt de Comando ou PowerShell):
oficina

## 5. Navegação e Utilização

Ao iniciar o programa, será exibido o menu principal:

===== OFICINA MECÂNICA =====

1 - Cadastrar Cliente

2 - Atualizar Cliente

3 - Remover Cliente

4 - Listar Clientes

5 - Cadastrar Veículo

6 - Atualizar Veículo

7 - Remover Veículo

8 - Listar Veículos

9 - Abrir Ordem

10 - Atualizar Ordem

11 - Remover Ordem

12 - Listar Ordens

0 - Sair

Escolha:

Digite o número da opção desejada e pressione ENTER.

## 6. Funções Disponíveis

### 1 a 4 — Gerenciamento de Clientes

Cadastrar Cliente: Solicita nome, CPF e telefone.
→ Valida os dados e salva no arquivo clientes.txt.

Atualizar Cliente: Permite editar nome e telefone a partir do CPF.

Remover Cliente: Remove um cliente existente informando o CPF.

Listar Clientes: Mostra todos os clientes cadastrados.


### 5 a 8 — Gerenciamento de Veículos

Cadastrar Veículo: Solicita placa, modelo, ano e CPF do dono (que deve existir).
→ Salva os dados no arquivo veiculos.txt.

Atualizar Veículo: Permite alterar modelo e ano.

Remover Veículo: Exclui um veículo pela placa.

Listar Veículos: Mostra todos os veículos cadastrados e seus donos.


### 9 a 12 — Gerenciamento de Ordens de Serviço

Abrir Ordem: Cria uma nova ordem de serviço para um veículo existente.
→ Solicita placa, data de entrada (formato dd/mm/aaaa) e descrição do problema.
→ O status inicial é “Aguardando Avaliação”.

Atualizar Ordem: Permite alterar descrição, data e status da ordem.

Remover Ordem: Exclui uma ordem existente pelo ID.

Listar Ordens: Exibe todas as ordens, com ID, veículo, data, status e descrição.

## 7. Armazenamento de Dados

Os dados são salvos automaticamente nos seguintes arquivos de texto:
clientes.txt;
veiculos.txt;
ordens.txt.

Cada vez que o programa é aberto, ele carrega automaticamente esses arquivos (caso existam) para manter a continuidade das informações.

## 8. Encerramento

Para sair do sistema, basta digitar:

0

O programa exibirá a mensagem "Encerrando..."
Antes de fechar, ele libera toda a memória alocada dinamicamente.

## 9. Observações Importantes

Use acentos corretamente, pois o setlocale(LC_ALL, "") adapta o programa ao idioma do sistema.
Os arquivos .txt serão criados automaticamente na mesma pasta do executável.
Evite deletar os arquivos de dados manualmente, pois isso apagará os registros salvos.
As datas devem estar no formato correto (dd/mm/aaaa).

## 10. Exemplo de Uso

Exemplo: Cadastro de Cliente e Veículo

===== OFICINA MECÂNICA =====

1 - Cadastrar Cliente

Escolha: 1

CPF (ex: 123.456.789-00): 111.222.333-44

Nome: Maria Silva

Telefone: (81) 99999-8888

Cliente cadastrado!

===== OFICINA MECÂNICA =====

5 - Cadastrar Veículo

Escolha: 5

Placa: ABC-1234

Modelo: Gol

Ano: 2018

CPF do dono: 111.222.333-44

Veículo cadastrado!
