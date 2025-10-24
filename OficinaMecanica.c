#include <stdio.h>       // Biblioteca padrão para entrada e saída (printf, scanf)
#include <stdlib.h>      // Biblioteca para alocação dinâmica (malloc, realloc, free)
#include <string.h>      // Biblioteca para manipulação de strings (strcmp, strcpy)
#include <locale.h>      // Biblioteca para configurar idioma/acentuação no terminal

#define MAX 100

// Enum de Status das ordens
typedef enum {
    AGUARDANDO_AVALIACAO = 1,
    EM_REPARO,
    FINALIZADO,
    ENTREGUE
} Status;

// Função para imprimir o status como texto
const char* statusTexto(Status status) {
    switch(status) {
        case AGUARDANDO_AVALIACAO: return "Aguardando Avaliação";
        case EM_REPARO: return "Em Reparo";
        case FINALIZADO: return "Finalizado";
        case ENTREGUE: return "Entregue";
        default: return "Desconhecido";
    }
}

// Struct de Cliente
typedef struct {
    char nome[50];
    char cpf[20];
    char telefone[20];
} Cliente;

// Struct de Veiculo
typedef struct {
    char placa[10];
    char modelo[30];
    int ano;
    Cliente *dono;
} Veiculo;

// Struct de Ordem de Serviço
typedef struct {
    int id;
    Veiculo *veiculo;
    char dataEntrada[11];
    char descricao[100];
    Status status;
} OrdemServico;

// Vetores dinâmicos globais
Cliente *clientes = NULL;
Veiculo *veiculos = NULL;
OrdemServico *ordens = NULL;
int qtdClientes = 0, qtdVeiculos = 0, qtdOrdens = 0;

// ---------- Validações ----------

// Função: nomeValido
// Objetivo: verificar se o nome digitado contém apenas letras e espaços.
// Retorna 1 se for válido, ou 0 se encontrar algum caractere inválido.
int nomeValido(const char *str) {
    // Percorre todos os caracteres da string até o final ('\0' indica o fim da string)
    for (int i = 0; str[i] != '\0'; i++) {
        unsigned char c = (unsigned char)str[i];  // Garante leitura correta de caracteres com acento

        // Verifica se o caractere é uma letra maiúscula, minúscula, espaço ou caractere acentuado simples.
        // Caso contrário, a função retorna 0 (nome inválido).
        if (!((c >= 'A' && c <= 'Z') ||  // Letras maiúsculas
              (c >= 'a' && c <= 'z') ||  // Letras minúsculas
              c == ' ' || // Espaço
              (c >= 128)))  // Permite alguns acentos básicos (UTF-8 simplificado)
            return 0; // Encontrou um caractere inválido ? nome não é válido
    }
    // Se percorreu toda a string sem achar erro, o nome é válido
    return 1;
}
// Função: cpfValido
// Objetivo: verificar se o CPF digitado contém apenas números, pontos e traços.
// Retorna 1 se for válido, 0 caso contrário.
int cpfValido(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        // Só aceita caracteres numéricos, '.' e '-'
        if (!((str[i] >= '0' && str[i] <= '9') || str[i] == '.' || str[i] == '-'))
    return 0; // Se encontrar algo fora disso, o CPF é inválido
    }
    return 1; // CPF contém apenas caracteres permitidos
}
// Função: telefoneValido
// Objetivo: verificar se o telefone contém apenas números e caracteres comuns em telefones.
// Retorna 1 se for válido, 0 caso contrário.
int telefoneValido(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
         // Permite apenas números, parênteses, hífen, espaço e fechamento de parêntese
        if (!((str[i] >= '0' && str[i] <= '9') || // Dígitos de 0 a 9
              str[i] == '(' || // Parêntese de abertura
              str[i] == ')' || // Parêntese de fechamento
              str[i] == '-' || // Hífen (para separar partes do número)
              str[i] == ' '))  // Espaço (opcional)
        return 0; // Se encontrar algo fora dessa lista, é inválido
    }
    return 1;  // Telefone contém apenas caracteres permitidos
}

// Função: dataValida
// Objetivo: verificar se uma data informada no formato "dd/mm/aaaa" é válida
// Retorno: 1 (verdadeiro) se a data for válida, ou 0 (falso) se for inválida.
int dataValida(const char *data) {
    int dia, mes, ano;
    // Usa scanf para "quebrar" a string e extrair dia, mês e ano.
    // Se não conseguir ler exatamente 3 números (dia, mês, ano), já retorna 0 (data inválida)
    if (sscanf(data, "%d/%d/%d", &dia, &mes, &ano) != 3) return 0;

    // Verifica se o ano está dentro de um intervalo aceitável (1900 a 2100).
    // Isso evita datas absurdas como 1500 ou 3000
    if (ano < 1900 || ano > 2100) 
    return 0;

    // Verifica se o mês é válido (de 1 a 12).
    if (mes < 1 || mes > 12) 
    return 0;

    // Verifica se o dia é positivo (não pode existir dia 0 ou negativo).
    if (dia < 1)
    return 0;

    int diasNoMes;
    // Define quantos dias tem o mês informado.
    // Usa o "switch" para associar o número do mês à sua quantidade de dias.
    switch (mes) {
         // Meses com 31 dias
        case 1: case 3: case 5: case 7: case 8: case 10: case 12:
            diasNoMes = 31; break;
        // Meses com 30 dias   
        case 4: case 6: case 9: case 11:
            diasNoMes = 30; break;
        // Fevereiro (pode ter 28 ou 29 dias dependendo do ano bissexto)    
        case 2:
            // Verifica se o ano é bissexto:
            // Divisível por 4 e não por 100, ou divisível por 400.
            // Se for bissexto, fevereiro tem 29 dias; caso contrário, 28.
            diasNoMes = ((ano % 4 == 0 && ano % 100 != 0) || (ano % 400 == 0)) ? 29 : 28;
            break;
         // Caso o mês não seja reconhecido (improvável, mas por segurança)    
        default:
            diasNoMes = 0;
    }
     // Se o número do dia for maior que o total de dias do mês, é inválido.
    if (dia > diasNoMes) return 0;
     // Se passou por todas as verificações, a data é válida.
    return 1;
}

// ---------- Persistência ----------
// Essas funções cuidam da "persistência" dos dados,
// ou seja, salvam e carregam as informações dos arquivos de texto.
// Assim, quando o programa é fechado e aberto novamente, os dados não se perdem.

// ----------- CLIENTES -----------
void salvarClientes() {
    FILE *f = fopen("clientes.txt", "w"); // Abre (ou cria) o arquivo para escrita
    if (!f) return;  // Se não conseguiu abrir o arquivo, sai da função

    // Percorre todos os clientes cadastrados e grava suas informações no arquivo
    for (int i = 0; i < qtdClientes; i++)
        fprintf(f, "%s;%s;%s\n", clientes[i].nome, clientes[i].cpf, clientes[i].telefone);
        // Formato usado: nome;cpf;telefone

    fclose(f); // Fecha o arquivo após a gravação
}
void carregarClientes() {
    FILE *f = fopen("clientes.txt", "r");  // Abre o arquivo de clientes para leitura
    if (!f) return; // Se o arquivo não existir ainda, apenas sai da função

    
    Cliente temp; // Variável temporária para armazenar cada cliente lido

    // Lê cada linha do arquivo no formato "nome;cpf;telefone" 
    while (fscanf(f, "%49[^;];%19[^;];%19[^\n]\n", temp.nome, temp.cpf, temp.telefone) == 3) {
        // Realoca memória para adicionar mais um cliente no vetor
        Cliente *novo = realloc(clientes, (qtdClientes+1)*sizeof(Cliente));
        if (!novo) { fclose(f); return; } // Se der erro de memória, interrompe
        clientes = novo;

        // Copia o cliente lido para o vetor principal e incrementa o contador
        clientes[qtdClientes++] = temp;
    }
    fclose(f);  // Fecha o arquivo após a leitura
}

// ----------- VEÍCULOS -----------
void salvarVeiculos() {
    FILE *f = fopen("veiculos.txt", "w"); // Abre (ou cria) o arquivo de veículos
    if (!f) return;

    // Percorre todos os veículos cadastrados
    for (int i = 0; i < qtdVeiculos; i++) {
        // Grava placa, modelo e ano separados por ponto e vírgula
        fprintf(f, "%s;%s;%d;", veiculos[i].placa, veiculos[i].modelo, veiculos[i].ano);

        // Se o veículo tiver um dono associado, grava o CPF do dono
        if (veiculos[i].dono) 
        fprintf(f, "%s\n", veiculos[i].dono->cpf);
        else 
        fprintf(f, ";\n"); // Se não tiver dono, grava CPF vazio
    }
    fclose(f); // Fecha o arquivo após salvar todos os veículos
}
void carregarVeiculos() {
    FILE *f = fopen("veiculos.txt", "r"); // Abre o arquivo para leitura
    if (!f) return; // Se não existir ainda, apenas sai da função
    Veiculo temp; // Variável temporária para armazenar cada veículo
    char cpf[20];  // CPF do dono lido do arquivo

    // Lê cada linha no formato "placa;modelo;ano;cpf"
    while (fscanf(f, "%9[^;];%29[^;];%d;%19[^\n]\n", temp.placa, temp.modelo, &temp.ano, cpf) >= 3) {
        Cliente *dono = NULL; // Ponteiro para o dono do veículo

        // Se o CPF não estiver vazio, tenta encontrar o cliente correspondente
        if (cpf[0] != '\0') {
            for (int j = 0; j < qtdClientes; j++) {
                if (strcmp(clientes[j].cpf, cpf) == 0) { // Compara CPFs
                    dono = &clientes[j]; // Associa o veículo ao dono encontrado
                    break;
                }
            }
        }
        temp.dono = dono;  // Guarda o ponteiro do dono (ou NULL se não achou)

        // Realoca o vetor de veículos para adicionar mais um
        Veiculo *novo = realloc(veiculos, (qtdVeiculos + 1) * sizeof(Veiculo));
        if (!novo) { fclose(f); return; }  // Se falhar a alocação, encerra
        veiculos = novo;

        // Adiciona o veículo lido ao vetor principal e incrementa o contador
        veiculos[qtdVeiculos++] = temp;
    }
    fclose(f);  // Fecha o arquivo após carregar todos os dados
}
// ---------- ORDENS DE SERVIÇO ----------
void salvarOrdens() {
    FILE *f = fopen("ordens.txt", "w");  // Abre (ou cria) o arquivo para escrita
    if (!f) return; // Se não conseguiu abrir, apenas sai da função

    // Percorre todas as ordens cadastradas e grava cada uma em uma linha
    for (int i = 0; i < qtdOrdens; i++) 
        fprintf(f, "%d;%s;%s;%s;%d\n",
            ordens[i].id,   // ID da ordem
            ordens[i].veiculo ? ordens[i].veiculo->placa : "",  // Placa do veículo (ou vazio se não tiver)
            ordens[i].dataEntrada,  // Data de entrada
            ordens[i].descricao, // Descrição do problema
            (int)ordens[i].status   // Status convertido para número
        );
    fclose(f); // Fecha o arquivo após salvar
}
void carregarOrdens() {
    FILE *f = fopen("ordens.txt", "r");  // Abre o arquivo de ordens para leitura
    if (!f) return; // Se o arquivo ainda não existir, apenas sai da função

    OrdemServico temp; // Estrutura temporária para armazenar dados lidos
    char placa[10];  // Variável para guardar a placa do veículo da ordem

    // Lê cada linha do arquivo no formato: id;placa;dataEntrada;descricao;status
    while (fscanf(f, "%d;%9[^;];%10[^;];%99[^;];%d\n",
            &temp.id, placa, temp.dataEntrada, temp.descricao, (int*)&temp.status) == 5) {

        // Procura o veículo correspondente à placa lida
        Veiculo *v = NULL;
        for (int j = 0; j < qtdVeiculos; j++)
            if (strcmp(veiculos[j].placa, placa) == 0) v = &veiculos[j];

        temp.veiculo = v;  // Associa o ponteiro do veículo (ou NULL, se não achar)

        // Realoca o vetor de ordens para adicionar a nova ordem lida
        OrdemServico *novo = realloc(ordens, (qtdOrdens + 1) * sizeof(OrdemServico));
        if (!novo) { fclose(f); return; } // Se faltar memória, sai da função
        ordens = novo;

        // Copia a ordem temporária para o vetor principal e atualiza o contador
        ordens[qtdOrdens++] = temp;
    }
    fclose(f);  // Fecha o arquivo após carregar todas as ordens
}

// ---------- Clientes ----------
void cadastrarCliente() {
    char cpfTemp[20];
    int existe;

    // --- Validação do CPF ---
    do {
        printf("CPF (ex: 123.456.789-00): ");
        scanf(" %[^\n]", cpfTemp); // Lê o CPF informado pelo usuário
        existe = 0;

        // Verifica se o CPF é válido
        if (!cpfValido(cpfTemp)) printf("CPF inválido!\n");
        else
        // Verifica se já existe um cliente com o mesmo CPF
        for(int i=0; i<qtdClientes; i++)
            if(strcmp(clientes[i].cpf, cpfTemp)==0) {
                printf("Já existe cliente com esse CPF!\n");
                existe = 1;
                break;
            }
    } while (!cpfValido(cpfTemp) || existe);  // Repete até CPF válido e não repetido

    // Realoca memória para adicionar o novo cliente
    Cliente *novo = realloc(clientes, (qtdClientes + 1) * sizeof(Cliente));
    if (!novo) { printf("Erro de memória.\n"); return; }
    clientes = novo;
 
    // Copia o CPF validado para o novo cliente
    strcpy(clientes[qtdClientes].cpf, cpfTemp);

     // ---------- Validação do nome ----------
    do {
        printf("Nome: ");
        scanf(" %[^\n]", clientes[qtdClientes].nome);
        if (!nomeValido(clientes[qtdClientes].nome)) printf("Nome inválido!\n");
    } while (!nomeValido(clientes[qtdClientes].nome)); // Repete até ser válido


      // ---------- Validação do telefone ----------
    do {
        printf("Telefone: ");
        scanf(" %[^\n]", clientes[qtdClientes].telefone);
        if (!telefoneValido(clientes[qtdClientes].telefone)) printf("Telefone inválido!\n");
    } while (!telefoneValido(clientes[qtdClientes].telefone));  // Repete até ser válido

    // Atualiza a contagem e salva no arquivo
    qtdClientes++; salvarClientes();
    printf("Cliente cadastrado!\n");
}
// ---------- CLIENTES ----------
void atualizarCliente() {
    char cpf[20];
    printf("CPF do cliente para editar: ");
    scanf(" %[^\n]", cpf); // Lê o CPF do cliente que será atualizado
    for (int i = 0; i < qtdClientes; i++) {
        if (strcmp(clientes[i].cpf, cpf) == 0) { // Cliente encontrado

             // Permite atualizar nome e telefone
            printf("Novo nome: ");
            scanf(" %[^\n]", clientes[i].nome);

            printf("Novo telefone: ");
            scanf(" %[^\n]", clientes[i].telefone);

            // Salva as alterações no arquivo
            salvarClientes();
            printf("Cliente atualizado!\n");
            return;  // Sai da função após atualizar
        }
    }
     // Caso o CPF não exista no cadastro
    printf("CPF não encontrado.\n");
}
void removerCliente() {
    char cpf[20];
    printf("CPF para remover: ");
    scanf(" %[^\n]", cpf); // Lê o CPF do cliente a ser removido

    // Procura o cliente pelo CPF
    for (int i = 0; i < qtdClientes; i++) {
        if (strcmp(clientes[i].cpf, cpf) == 0) {  // Cliente encontrado

            // "Empurra" todos os elementos seguintes uma posição para trás
            for (int j = i; j < qtdClientes-1; j++)
                clientes[j] = clientes[j+1];

            // Diminui a contagem total de clientes
            qtdClientes--;

            // Realoca o vetor de clientes para liberar memória não usada
            Cliente *novo = realloc(clientes, qtdClientes * sizeof(Cliente));

            // Se não há mais clientes, libera completamente o vetor
            if (qtdClientes == 0) {
                free(clientes); 
                clientes = NULL; }
            else if (novo) 
            clientes = novo; // Atualiza o ponteiro caso a realocação dê certo

            // Salva as mudanças no arquivo
            salvarClientes();

            printf("Cliente removido!\n");
            return; // Sai da função
        }
    }
    printf("CPF não encontrado.\n");
}

// ---------- Veiculos ----------
void cadastrarVeiculo() {
    char placaTemp[10];
    int existe;
     // ---------- Validação da placa ----------
    do {
        printf("Placa: ");
        scanf(" %[^\n]", placaTemp);
        existe = 0;

        // Verifica se já existe veículo com a mesma placa
        for (int i = 0; i < qtdVeiculos; i++)
            if (strcmp(veiculos[i].placa, placaTemp) == 0) {
                printf("Já existe veículo com essa placa!\n");
                existe = 1;
                break;
            }
    } while (!placaTemp[0] || existe);  // Repete se a placa estiver vazia ou repetida

    // Realoca o vetor para incluir o novo veículo
    Veiculo *novo = realloc(veiculos, (qtdVeiculos + 1) * sizeof(Veiculo));
    if (!novo) { printf("Erro de memória.\n"); return; }
    veiculos = novo;

    // Copia os dados básicos do veículo
    strcpy(veiculos[qtdVeiculos].placa, placaTemp);
    printf("Modelo: ");
    scanf(" %[^\n]", veiculos[qtdVeiculos].modelo);
    printf("Ano: ");
    scanf("%d", &veiculos[qtdVeiculos].ano);

     // ---------- Associação com o dono ----------
    char cpf[20];
    printf("CPF do dono: ");
    scanf(" %[^\n]", cpf);
    Cliente *dono = NULL;

    // Procura o cliente com o CPF informado
    for (int i = 0; i < qtdClientes; i++)
        if (strcmp(clientes[i].cpf, cpf) == 0) dono = &clientes[i];

    // Se não encontrar o dono, cancela o cadastro    
    if (!dono) {
        printf("Cliente não encontrado!\n");
        return;
    }
    // Associa o dono ao veículo
    veiculos[qtdVeiculos].dono = dono;

    // Atualiza a contagem e salva os dados no arquivo
    qtdVeiculos++;
    salvarVeiculos();
    printf("Veículo cadastrado!\n");
}
void atualizarVeiculo() {
    char placa[10];
    printf("Placa do veículo para editar: ");
    scanf(" %[^\n]", placa);  // Lê a placa do veículo a ser atualizado

    // Procura o veículo com a placa informada
    for (int i = 0; i < qtdVeiculos; i++) {
        if (strcmp(veiculos[i].placa, placa) == 0) {  // Veículo encontrado

            // Permite editar o modelo e o ano
            printf("Novo modelo: ");
            scanf(" %[^\n]", veiculos[i].modelo);

            printf("Novo ano: ");
            scanf("%d", &veiculos[i].ano);

            // Salva as alterações no arquivo
            salvarVeiculos();
            printf("Veículo atualizado!\n");
            return;  // Sai da função
        }
    }
    // Caso a placa não esteja cadastrada
    printf("Placa não encontrada.\n");
}
// ---------- Remover Veículo ----------
void removerVeiculo() {
    char placa[10]; // Variável para armazenar a placa digitada pelo usuário

    // Solicita ao usuário a placa do veículo que ele deseja remover
    printf("Placa para remover: ");
    scanf(" %[^\n]", placa);

    // Percorre a lista de veículos cadastrados
    for (int i = 0; i < qtdVeiculos; i++) {
        // Compara a placa informada com a placa de cada veículo
        if (strcmp(veiculos[i].placa, placa) == 0) {
            // Caso encontre o veículo, realiza a "remoção lógica"
            // movendo todos os veículos seguintes uma posição para trás
            for (int j = i; j < qtdVeiculos-1; j++)
                veiculos[j] = veiculos[j+1];
            // Diminui o contador total de veículos
            qtdVeiculos--;
            // Realoca o vetor para liberar a memória não utilizada
            Veiculo *novo = realloc(veiculos, qtdVeiculos * sizeof(Veiculo));

            // Se não há mais veículos, libera completamente o espaço da lista
            if (qtdVeiculos == 0) { 
            free(veiculos); 
            veiculos = NULL; }
            // Caso contrário, atualiza o ponteiro se a realocação deu certo
            else if (novo) veiculos = novo;

            // Salva a nova lista atualizada no arquivo de persistência
            salvarVeiculos();

            // Confirma ao usuário que o veículo foi removido
            printf("Veículo removido!\n");
            return;  // Sai da função, pois já encontrou e removeu o veículo
        }
    }
    // Se o laço terminar sem encontrar a placa, informa que ela não existe
    printf("Placa não encontrada.\n");
}

// ---------- Ordens ----------
void abrirOrdem() {
    // Realoca o vetor de ordens para adicionar mais uma nova ordem
    OrdemServico *novo = realloc(ordens, (qtdOrdens+1)*sizeof(OrdemServico));
    if (!novo) { // Se não conseguiu alocar memória, mostra erro e sai da função
         printf("Erro de memória.\n"); return; }
    ordens = novo; // Atualiza o ponteiro principal com o novo espaço de memória
    OrdemServico *nova = &ordens[qtdOrdens]; // Ponteiro para a nova posição (nova ordem)
    nova->id = qtdOrdens+1;  // Define o ID automaticamente com base na quantidade atual

    // ---------- Identificação do veículo ----------
    char placa[10];
    printf("Placa do veículo: ");
    scanf(" %[^\n]", placa); // Lê a placa do veículo (permite espaços, caso existam)
    Veiculo *v = NULL; // Ponteiro para armazenar o veículo encontrado

    // Procura no vetor de veículos o que tem a mesma placa informada
    for (int i = 0; i < qtdVeiculos; i++)
        if (strcmp(veiculos[i].placa, placa) == 0) v = &veiculos[i];

    // Se não encontrou a placa no cadastro
    if (!v) {
         printf("Veículo não encontrado!\n"); 
         return; // Sai da função, pois não é possível abrir uma ordem sem veículo
        }
    nova->veiculo = v;  // Associa o veículo encontrado à nova ordem

    // ---------- Validação da data de entrada ----------
    do {
        printf("Data de entrada (dd/mm/aaaa): ");
        scanf(" %[^\n]", nova->dataEntrada); // Lê a data informada
        if (!dataValida(nova->dataEntrada))
            printf("Data inválida! Tente novamente.\n");  // Se formato incorreto, pede de novo
    } while (!dataValida(nova->dataEntrada)); // Repete até a data ser válida

    // ---------- Descrição do problema ----------
    printf("Descrição do problema: ");
    scanf(" %[^\n]", nova->descricao);  // Lê a descrição completa do problema

    // Define o status inicial da ordem como "Aguardando Avaliação"
    nova->status = AGUARDANDO_AVALIACAO;

    // Atualiza a quantidade total de ordens e salva tudo em arquivo
    qtdOrdens++;
    salvarOrdens();

    // Confirma ao usuário que a ordem foi aberta com sucesso
    printf("Ordem aberta! ID: %d\n", nova->id);
}
//---------- Atualizar Ordem ----------
void atualizarOrdem() {
    int id;
    printf("ID da ordem para editar: ");
    scanf("%d",&id); // Lê o ID da ordem de serviço que o usuário quer atualizar

    // Percorre todas as ordens cadastradas para encontrar a que tem o ID informado
    for (int i = 0; i < qtdOrdens; i++) {
        if (ordens[i].id == id) {  // Se encontrou a ordem com o ID correspondente

            // Atualiza a descrição da ordem
            printf("Nova descrição: ");
            scanf(" %[^\n]", ordens[i].descricao); // Lê a nova descrição completa (incluindo espaços)

            // Alterar data (validação)
            char novaData[11];
            printf("Nova data de entrada (dd/mm/aaaa) ou ENTER para manter (%s): ", ordens[i].dataEntrada);

            getchar(); // Limpa o buffer do teclado para evitar que o ENTER anterior atrapalhe a leitura

             // Lê a nova data digitada (ou ENTER, se o usuário quiser manter a antiga)
            if (fgets(novaData, sizeof(novaData), stdin)) {

                // Remove o '\n' que o fgets adiciona ao final da string
                size_t L = strlen(novaData);
                if (L > 0 && novaData[L-1] == '\n') novaData[L-1] = '\0';

                // Se o usuário digitou algo (ou seja, não apenas ENTER)
                if (strlen(novaData) > 0) {

                    // Valida o formato da data antes de aceitar
                    while (!dataValida(novaData)) {
                        printf("Data inválida! Digite novamente (dd/mm/aaaa): ");
                        fgets(novaData, sizeof(novaData), stdin);
                        L = strlen(novaData);
                        if (L > 0 && novaData[L-1] == '\n') novaData[L-1] = '\0';
                    }
                    // Se for válida, substitui a data antiga pela nova
                    strcpy(ordens[i].dataEntrada, novaData);
                }
            }
            // ---------- Atualização do status da ordem ----------
            printf("Novo status (1-Aguardando 2-Reparo 3-Finalizado 4-Entregue): ");
            int s; scanf("%d",&s);

            // Valida o valor digitado (deve ser de 1 a 4)
            if (s >= 1 && s <= 4) 
            ordens[i].status = (Status)s; // Converte o número para o tipo Status
            else printf("Status inválido. Mantendo o anterior.\n");

            // Salva todas as alterações no arquivo
            salvarOrdens();
            printf("Ordem atualizada!\n");
            return; // Encerra a função após atualizar a ordem
        }
    }
    // Caso o ID não exista na lista de ordens
    printf("ID não encontrado.\n");
}
// Função: removerOrdem
// Objetivo: remover uma ordem de serviço do sistema com base no seu ID
void removerOrdem() {
    int id;

     // Solicita ao usuário o ID da ordem que ele deseja remover
    printf("ID para remover: ");
    scanf("%d",&id);

     // Percorre todas as ordens cadastradas
    for (int i = 0; i < qtdOrdens; i++) {

        // Verifica se o ID informado corresponde a uma das ordens existentes
        if (ordens[i].id == id) {

            // Desloca todas as ordens seguintes uma posição para trás,
            // sobrescrevendo a ordem removida (efeito de "apagar" ela do meio do vetor)
            for (int j = i; j < qtdOrdens-1; j++)
                ordens[j] = ordens[j+1];

            // Diminui o contador de ordens ativas    
            qtdOrdens--;

            // Realoca a memória para ajustar o tamanho do vetor 'ordens'
            OrdemServico *novo = realloc(ordens, qtdOrdens * sizeof(OrdemServico));
            if (qtdOrdens == 0) { free(ordens); ordens = NULL; }

            // Caso contrário, se o realloc foi bem-sucedido, atualiza o ponteiro principal
            else if (novo) ordens = novo;

            // Salva as alterações no arquivo, garantindo persistência dos dados
            salvarOrdens();

            // Informa ao usuário que a remoção foi concluída
            printf("Ordem removida!\n");
            return;
        }
    }
    // Se o loop terminar sem encontrar o ID, significa que a ordem não existe
    printf("ID não encontrado.\n");
}

// ---------- Listagens ----------

// Função: listarClientes
// Objetivo: exibir na tela todos os clientes cadastrados no sistema
void listarClientes() {
    printf("\n--- LISTA DE CLIENTES ---\n");
    for (int i = 0; i < qtdClientes; i++) {
        printf("%d - %s | CPF: %s | Tel: %s\n", i+1, clientes[i].nome, clientes[i].cpf, clientes[i].telefone);
    }
}
// Função: listarVeiculos
// Objetivo: exibir todos os veículos cadastrados e seus respectivos donos
void listarVeiculos() {
    printf("\n--- LISTA DE VEÍCULOS ---\n");
    // Percorre o vetor de veículos
    for (int i = 0; i < qtdVeiculos; i++) {
        // Exibe placa, modelo, ano e o nome do dono do veículo.
        // Caso o ponteiro 'dono' seja nulo, mostra "Desconhecido" (evita erro de acesso).
        printf("%d - %s | Modelo: %s | Ano: %d | Dono: %s\n", i+1,
            veiculos[i].placa, veiculos[i].modelo, veiculos[i].ano,
            veiculos[i].dono ? veiculos[i].dono->nome : "Desconhecido");
    }
}
void listarOrdens() {
    printf("\n--- ORDENS DE SERVIÇO ---\n");
    for (int i = 0; i < qtdOrdens; i++) {
        printf("ID %d | Veículo: %s | Data: %s | Status: %s | Problema: %s\n",
            ordens[i].id,
            ordens[i].veiculo ? ordens[i].veiculo->placa : "Desconhecido", // mostra a placa ou "Desconhecido" se não houver veículo associado
            ordens[i].dataEntrada, // mostra a data de entrada da OS
            statusTexto(ordens[i].status), // converte o status (enum) em texto legível
            ordens[i].descricao // mostra a descrição do problema
        );
    }
}

// ---------- Menu principal ----------
int main() {
    setlocale(LC_ALL,""); // usar local do sistema para mostrar as palavras acentuadas
    carregarClientes();
    carregarVeiculos();
    carregarOrdens();
    int opc;
    do {
        printf("\n===== OFICINA MECÂNICA =====\n");
        printf("1 - Cadastrar Cliente\n2 - Atualizar Cliente\n3 - Remover Cliente\n4 - Listar Clientes\n");
        printf("5 - Cadastrar Veículo\n6 - Atualizar Veículo\n7 - Remover Veículo\n8 - Listar Veículos\n");
        printf("9 - Abrir Ordem\n10 - Atualizar Ordem\n11 - Remover Ordem\n12 - Listar Ordens\n");
        printf("0 - Sair\nEscolha: ");
        if (scanf("%d",&opc) != 1) { // evita loop infinito em entrada inválida
            while (getchar() != '\n') {}
            opc = -1;
        }
        switch(opc) {
            case 1: cadastrarCliente();  // Chama a função que cadastra um novo cliente
            break;
            case 2: atualizarCliente();  // Permite editar os dados de um cliente existente
            break;
            case 3: removerCliente();    // Remove um cliente cadastrado pelo CPF
            break;
            case 4: listarClientes();    // Exibe todos os clientes cadastrados na oficina
            break;
            case 5: cadastrarVeiculo();  // Cadastra um novo veículo e associa a um cliente
            break;
            case 6: atualizarVeiculo();  // Altera dados do veículo (modelo, ano etc.)
            break;
            case 7: removerVeiculo();    // Exclui um veículo do sistema pela placa
            break;
            case 8: listarVeiculos();    // Mostra todos os veículos cadastrados
            break;
           // ---------- Ordens de Serviço ----------
            case 9: abrirOrdem();        // Abre uma nova ordem de serviço para um veículo     
            break;
            case 10: atualizarOrdem();   // Atualiza informações ou status de uma ordem existente
            break;
            case 11: removerOrdem();     // Exclui uma ordem de serviço do sistema
            break;
            case 12: listarOrdens();     // Lista todas as ordens cadastradas com seus status
            break;
           // ---------- Saída ----------
            case 0: printf("Encerrando...\n"); // Mensagem exibida ao encerrar o programa
            break;
            // ---------- Opção Inválida ----------
            default: printf("Opção inválida!\n");  // Caso o usuário digite um número fora das opções do menu
        }
    } while (opc != 0);

    // Libera toda a memoria alocada antes de sair
    free(clientes);
    free(veiculos);
    free(ordens);
    return 0;
}
