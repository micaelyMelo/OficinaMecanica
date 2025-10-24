#include <stdio.h>       // Biblioteca padr�o para entrada e sa�da (printf, scanf)
#include <stdlib.h>      // Biblioteca para aloca��o din�mica (malloc, realloc, free)
#include <string.h>      // Biblioteca para manipula��o de strings (strcmp, strcpy)
#include <locale.h>      // Biblioteca para configurar idioma/acentua��o no terminal

#define MAX 100

// Enum de Status das ordens
typedef enum {
    AGUARDANDO_AVALIACAO = 1,
    EM_REPARO,
    FINALIZADO,
    ENTREGUE
} Status;

// Fun��o para imprimir o status como texto
const char* statusTexto(Status status) {
    switch(status) {
        case AGUARDANDO_AVALIACAO: return "Aguardando Avalia��o";
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

// Struct de Ordem de Servi�o
typedef struct {
    int id;
    Veiculo *veiculo;
    char dataEntrada[11];
    char descricao[100];
    Status status;
} OrdemServico;

// Vetores din�micos globais
Cliente *clientes = NULL;
Veiculo *veiculos = NULL;
OrdemServico *ordens = NULL;
int qtdClientes = 0, qtdVeiculos = 0, qtdOrdens = 0;

// ---------- Valida��es ----------

// Fun��o: nomeValido
// Objetivo: verificar se o nome digitado cont�m apenas letras e espa�os.
// Retorna 1 se for v�lido, ou 0 se encontrar algum caractere inv�lido.
int nomeValido(const char *str) {
    // Percorre todos os caracteres da string at� o final ('\0' indica o fim da string)
    for (int i = 0; str[i] != '\0'; i++) {
        unsigned char c = (unsigned char)str[i];  // Garante leitura correta de caracteres com acento

        // Verifica se o caractere � uma letra mai�scula, min�scula, espa�o ou caractere acentuado simples.
        // Caso contr�rio, a fun��o retorna 0 (nome inv�lido).
        if (!((c >= 'A' && c <= 'Z') ||  // Letras mai�sculas
              (c >= 'a' && c <= 'z') ||  // Letras min�sculas
              c == ' ' || // Espa�o
              (c >= 128)))  // Permite alguns acentos b�sicos (UTF-8 simplificado)
            return 0; // Encontrou um caractere inv�lido ? nome n�o � v�lido
    }
    // Se percorreu toda a string sem achar erro, o nome � v�lido
    return 1;
}
// Fun��o: cpfValido
// Objetivo: verificar se o CPF digitado cont�m apenas n�meros, pontos e tra�os.
// Retorna 1 se for v�lido, 0 caso contr�rio.
int cpfValido(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        // S� aceita caracteres num�ricos, '.' e '-'
        if (!((str[i] >= '0' && str[i] <= '9') || str[i] == '.' || str[i] == '-'))
    return 0; // Se encontrar algo fora disso, o CPF � inv�lido
    }
    return 1; // CPF cont�m apenas caracteres permitidos
}
// Fun��o: telefoneValido
// Objetivo: verificar se o telefone cont�m apenas n�meros e caracteres comuns em telefones.
// Retorna 1 se for v�lido, 0 caso contr�rio.
int telefoneValido(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
         // Permite apenas n�meros, par�nteses, h�fen, espa�o e fechamento de par�ntese
        if (!((str[i] >= '0' && str[i] <= '9') || // D�gitos de 0 a 9
              str[i] == '(' || // Par�ntese de abertura
              str[i] == ')' || // Par�ntese de fechamento
              str[i] == '-' || // H�fen (para separar partes do n�mero)
              str[i] == ' '))  // Espa�o (opcional)
        return 0; // Se encontrar algo fora dessa lista, � inv�lido
    }
    return 1;  // Telefone cont�m apenas caracteres permitidos
}

// Fun��o: dataValida
// Objetivo: verificar se uma data informada no formato "dd/mm/aaaa" � v�lida
// Retorno: 1 (verdadeiro) se a data for v�lida, ou 0 (falso) se for inv�lida.
int dataValida(const char *data) {
    int dia, mes, ano;
    // Usa scanf para "quebrar" a string e extrair dia, m�s e ano.
    // Se n�o conseguir ler exatamente 3 n�meros (dia, m�s, ano), j� retorna 0 (data inv�lida)
    if (sscanf(data, "%d/%d/%d", &dia, &mes, &ano) != 3) return 0;

    // Verifica se o ano est� dentro de um intervalo aceit�vel (1900 a 2100).
    // Isso evita datas absurdas como 1500 ou 3000
    if (ano < 1900 || ano > 2100) 
    return 0;

    // Verifica se o m�s � v�lido (de 1 a 12).
    if (mes < 1 || mes > 12) 
    return 0;

    // Verifica se o dia � positivo (n�o pode existir dia 0 ou negativo).
    if (dia < 1)
    return 0;

    int diasNoMes;
    // Define quantos dias tem o m�s informado.
    // Usa o "switch" para associar o n�mero do m�s � sua quantidade de dias.
    switch (mes) {
         // Meses com 31 dias
        case 1: case 3: case 5: case 7: case 8: case 10: case 12:
            diasNoMes = 31; break;
        // Meses com 30 dias   
        case 4: case 6: case 9: case 11:
            diasNoMes = 30; break;
        // Fevereiro (pode ter 28 ou 29 dias dependendo do ano bissexto)    
        case 2:
            // Verifica se o ano � bissexto:
            // Divis�vel por 4 e n�o por 100, ou divis�vel por 400.
            // Se for bissexto, fevereiro tem 29 dias; caso contr�rio, 28.
            diasNoMes = ((ano % 4 == 0 && ano % 100 != 0) || (ano % 400 == 0)) ? 29 : 28;
            break;
         // Caso o m�s n�o seja reconhecido (improv�vel, mas por seguran�a)    
        default:
            diasNoMes = 0;
    }
     // Se o n�mero do dia for maior que o total de dias do m�s, � inv�lido.
    if (dia > diasNoMes) return 0;
     // Se passou por todas as verifica��es, a data � v�lida.
    return 1;
}

// ---------- Persist�ncia ----------
// Essas fun��es cuidam da "persist�ncia" dos dados,
// ou seja, salvam e carregam as informa��es dos arquivos de texto.
// Assim, quando o programa � fechado e aberto novamente, os dados n�o se perdem.

// ----------- CLIENTES -----------
void salvarClientes() {
    FILE *f = fopen("clientes.txt", "w"); // Abre (ou cria) o arquivo para escrita
    if (!f) return;  // Se n�o conseguiu abrir o arquivo, sai da fun��o

    // Percorre todos os clientes cadastrados e grava suas informa��es no arquivo
    for (int i = 0; i < qtdClientes; i++)
        fprintf(f, "%s;%s;%s\n", clientes[i].nome, clientes[i].cpf, clientes[i].telefone);
        // Formato usado: nome;cpf;telefone

    fclose(f); // Fecha o arquivo ap�s a grava��o
}
void carregarClientes() {
    FILE *f = fopen("clientes.txt", "r");  // Abre o arquivo de clientes para leitura
    if (!f) return; // Se o arquivo n�o existir ainda, apenas sai da fun��o

    
    Cliente temp; // Vari�vel tempor�ria para armazenar cada cliente lido

    // L� cada linha do arquivo no formato "nome;cpf;telefone" 
    while (fscanf(f, "%49[^;];%19[^;];%19[^\n]\n", temp.nome, temp.cpf, temp.telefone) == 3) {
        // Realoca mem�ria para adicionar mais um cliente no vetor
        Cliente *novo = realloc(clientes, (qtdClientes+1)*sizeof(Cliente));
        if (!novo) { fclose(f); return; } // Se der erro de mem�ria, interrompe
        clientes = novo;

        // Copia o cliente lido para o vetor principal e incrementa o contador
        clientes[qtdClientes++] = temp;
    }
    fclose(f);  // Fecha o arquivo ap�s a leitura
}

// ----------- VE�CULOS -----------
void salvarVeiculos() {
    FILE *f = fopen("veiculos.txt", "w"); // Abre (ou cria) o arquivo de ve�culos
    if (!f) return;

    // Percorre todos os ve�culos cadastrados
    for (int i = 0; i < qtdVeiculos; i++) {
        // Grava placa, modelo e ano separados por ponto e v�rgula
        fprintf(f, "%s;%s;%d;", veiculos[i].placa, veiculos[i].modelo, veiculos[i].ano);

        // Se o ve�culo tiver um dono associado, grava o CPF do dono
        if (veiculos[i].dono) 
        fprintf(f, "%s\n", veiculos[i].dono->cpf);
        else 
        fprintf(f, ";\n"); // Se n�o tiver dono, grava CPF vazio
    }
    fclose(f); // Fecha o arquivo ap�s salvar todos os ve�culos
}
void carregarVeiculos() {
    FILE *f = fopen("veiculos.txt", "r"); // Abre o arquivo para leitura
    if (!f) return; // Se n�o existir ainda, apenas sai da fun��o
    Veiculo temp; // Vari�vel tempor�ria para armazenar cada ve�culo
    char cpf[20];  // CPF do dono lido do arquivo

    // L� cada linha no formato "placa;modelo;ano;cpf"
    while (fscanf(f, "%9[^;];%29[^;];%d;%19[^\n]\n", temp.placa, temp.modelo, &temp.ano, cpf) >= 3) {
        Cliente *dono = NULL; // Ponteiro para o dono do ve�culo

        // Se o CPF n�o estiver vazio, tenta encontrar o cliente correspondente
        if (cpf[0] != '\0') {
            for (int j = 0; j < qtdClientes; j++) {
                if (strcmp(clientes[j].cpf, cpf) == 0) { // Compara CPFs
                    dono = &clientes[j]; // Associa o ve�culo ao dono encontrado
                    break;
                }
            }
        }
        temp.dono = dono;  // Guarda o ponteiro do dono (ou NULL se n�o achou)

        // Realoca o vetor de ve�culos para adicionar mais um
        Veiculo *novo = realloc(veiculos, (qtdVeiculos + 1) * sizeof(Veiculo));
        if (!novo) { fclose(f); return; }  // Se falhar a aloca��o, encerra
        veiculos = novo;

        // Adiciona o ve�culo lido ao vetor principal e incrementa o contador
        veiculos[qtdVeiculos++] = temp;
    }
    fclose(f);  // Fecha o arquivo ap�s carregar todos os dados
}
// ---------- ORDENS DE SERVI�O ----------
void salvarOrdens() {
    FILE *f = fopen("ordens.txt", "w");  // Abre (ou cria) o arquivo para escrita
    if (!f) return; // Se n�o conseguiu abrir, apenas sai da fun��o

    // Percorre todas as ordens cadastradas e grava cada uma em uma linha
    for (int i = 0; i < qtdOrdens; i++) 
        fprintf(f, "%d;%s;%s;%s;%d\n",
            ordens[i].id,   // ID da ordem
            ordens[i].veiculo ? ordens[i].veiculo->placa : "",  // Placa do ve�culo (ou vazio se n�o tiver)
            ordens[i].dataEntrada,  // Data de entrada
            ordens[i].descricao, // Descri��o do problema
            (int)ordens[i].status   // Status convertido para n�mero
        );
    fclose(f); // Fecha o arquivo ap�s salvar
}
void carregarOrdens() {
    FILE *f = fopen("ordens.txt", "r");  // Abre o arquivo de ordens para leitura
    if (!f) return; // Se o arquivo ainda n�o existir, apenas sai da fun��o

    OrdemServico temp; // Estrutura tempor�ria para armazenar dados lidos
    char placa[10];  // Vari�vel para guardar a placa do ve�culo da ordem

    // L� cada linha do arquivo no formato: id;placa;dataEntrada;descricao;status
    while (fscanf(f, "%d;%9[^;];%10[^;];%99[^;];%d\n",
            &temp.id, placa, temp.dataEntrada, temp.descricao, (int*)&temp.status) == 5) {

        // Procura o ve�culo correspondente � placa lida
        Veiculo *v = NULL;
        for (int j = 0; j < qtdVeiculos; j++)
            if (strcmp(veiculos[j].placa, placa) == 0) v = &veiculos[j];

        temp.veiculo = v;  // Associa o ponteiro do ve�culo (ou NULL, se n�o achar)

        // Realoca o vetor de ordens para adicionar a nova ordem lida
        OrdemServico *novo = realloc(ordens, (qtdOrdens + 1) * sizeof(OrdemServico));
        if (!novo) { fclose(f); return; } // Se faltar mem�ria, sai da fun��o
        ordens = novo;

        // Copia a ordem tempor�ria para o vetor principal e atualiza o contador
        ordens[qtdOrdens++] = temp;
    }
    fclose(f);  // Fecha o arquivo ap�s carregar todas as ordens
}

// ---------- Clientes ----------
void cadastrarCliente() {
    char cpfTemp[20];
    int existe;

    // --- Valida��o do CPF ---
    do {
        printf("CPF (ex: 123.456.789-00): ");
        scanf(" %[^\n]", cpfTemp); // L� o CPF informado pelo usu�rio
        existe = 0;

        // Verifica se o CPF � v�lido
        if (!cpfValido(cpfTemp)) printf("CPF inv�lido!\n");
        else
        // Verifica se j� existe um cliente com o mesmo CPF
        for(int i=0; i<qtdClientes; i++)
            if(strcmp(clientes[i].cpf, cpfTemp)==0) {
                printf("J� existe cliente com esse CPF!\n");
                existe = 1;
                break;
            }
    } while (!cpfValido(cpfTemp) || existe);  // Repete at� CPF v�lido e n�o repetido

    // Realoca mem�ria para adicionar o novo cliente
    Cliente *novo = realloc(clientes, (qtdClientes + 1) * sizeof(Cliente));
    if (!novo) { printf("Erro de mem�ria.\n"); return; }
    clientes = novo;
 
    // Copia o CPF validado para o novo cliente
    strcpy(clientes[qtdClientes].cpf, cpfTemp);

     // ---------- Valida��o do nome ----------
    do {
        printf("Nome: ");
        scanf(" %[^\n]", clientes[qtdClientes].nome);
        if (!nomeValido(clientes[qtdClientes].nome)) printf("Nome inv�lido!\n");
    } while (!nomeValido(clientes[qtdClientes].nome)); // Repete at� ser v�lido


      // ---------- Valida��o do telefone ----------
    do {
        printf("Telefone: ");
        scanf(" %[^\n]", clientes[qtdClientes].telefone);
        if (!telefoneValido(clientes[qtdClientes].telefone)) printf("Telefone inv�lido!\n");
    } while (!telefoneValido(clientes[qtdClientes].telefone));  // Repete at� ser v�lido

    // Atualiza a contagem e salva no arquivo
    qtdClientes++; salvarClientes();
    printf("Cliente cadastrado!\n");
}
// ---------- CLIENTES ----------
void atualizarCliente() {
    char cpf[20];
    printf("CPF do cliente para editar: ");
    scanf(" %[^\n]", cpf); // L� o CPF do cliente que ser� atualizado
    for (int i = 0; i < qtdClientes; i++) {
        if (strcmp(clientes[i].cpf, cpf) == 0) { // Cliente encontrado

             // Permite atualizar nome e telefone
            printf("Novo nome: ");
            scanf(" %[^\n]", clientes[i].nome);

            printf("Novo telefone: ");
            scanf(" %[^\n]", clientes[i].telefone);

            // Salva as altera��es no arquivo
            salvarClientes();
            printf("Cliente atualizado!\n");
            return;  // Sai da fun��o ap�s atualizar
        }
    }
     // Caso o CPF n�o exista no cadastro
    printf("CPF n�o encontrado.\n");
}
void removerCliente() {
    char cpf[20];
    printf("CPF para remover: ");
    scanf(" %[^\n]", cpf); // L� o CPF do cliente a ser removido

    // Procura o cliente pelo CPF
    for (int i = 0; i < qtdClientes; i++) {
        if (strcmp(clientes[i].cpf, cpf) == 0) {  // Cliente encontrado

            // "Empurra" todos os elementos seguintes uma posi��o para tr�s
            for (int j = i; j < qtdClientes-1; j++)
                clientes[j] = clientes[j+1];

            // Diminui a contagem total de clientes
            qtdClientes--;

            // Realoca o vetor de clientes para liberar mem�ria n�o usada
            Cliente *novo = realloc(clientes, qtdClientes * sizeof(Cliente));

            // Se n�o h� mais clientes, libera completamente o vetor
            if (qtdClientes == 0) {
                free(clientes); 
                clientes = NULL; }
            else if (novo) 
            clientes = novo; // Atualiza o ponteiro caso a realoca��o d� certo

            // Salva as mudan�as no arquivo
            salvarClientes();

            printf("Cliente removido!\n");
            return; // Sai da fun��o
        }
    }
    printf("CPF n�o encontrado.\n");
}

// ---------- Veiculos ----------
void cadastrarVeiculo() {
    char placaTemp[10];
    int existe;
     // ---------- Valida��o da placa ----------
    do {
        printf("Placa: ");
        scanf(" %[^\n]", placaTemp);
        existe = 0;

        // Verifica se j� existe ve�culo com a mesma placa
        for (int i = 0; i < qtdVeiculos; i++)
            if (strcmp(veiculos[i].placa, placaTemp) == 0) {
                printf("J� existe ve�culo com essa placa!\n");
                existe = 1;
                break;
            }
    } while (!placaTemp[0] || existe);  // Repete se a placa estiver vazia ou repetida

    // Realoca o vetor para incluir o novo ve�culo
    Veiculo *novo = realloc(veiculos, (qtdVeiculos + 1) * sizeof(Veiculo));
    if (!novo) { printf("Erro de mem�ria.\n"); return; }
    veiculos = novo;

    // Copia os dados b�sicos do ve�culo
    strcpy(veiculos[qtdVeiculos].placa, placaTemp);
    printf("Modelo: ");
    scanf(" %[^\n]", veiculos[qtdVeiculos].modelo);
    printf("Ano: ");
    scanf("%d", &veiculos[qtdVeiculos].ano);

     // ---------- Associa��o com o dono ----------
    char cpf[20];
    printf("CPF do dono: ");
    scanf(" %[^\n]", cpf);
    Cliente *dono = NULL;

    // Procura o cliente com o CPF informado
    for (int i = 0; i < qtdClientes; i++)
        if (strcmp(clientes[i].cpf, cpf) == 0) dono = &clientes[i];

    // Se n�o encontrar o dono, cancela o cadastro    
    if (!dono) {
        printf("Cliente n�o encontrado!\n");
        return;
    }
    // Associa o dono ao ve�culo
    veiculos[qtdVeiculos].dono = dono;

    // Atualiza a contagem e salva os dados no arquivo
    qtdVeiculos++;
    salvarVeiculos();
    printf("Ve�culo cadastrado!\n");
}
void atualizarVeiculo() {
    char placa[10];
    printf("Placa do ve�culo para editar: ");
    scanf(" %[^\n]", placa);  // L� a placa do ve�culo a ser atualizado

    // Procura o ve�culo com a placa informada
    for (int i = 0; i < qtdVeiculos; i++) {
        if (strcmp(veiculos[i].placa, placa) == 0) {  // Ve�culo encontrado

            // Permite editar o modelo e o ano
            printf("Novo modelo: ");
            scanf(" %[^\n]", veiculos[i].modelo);

            printf("Novo ano: ");
            scanf("%d", &veiculos[i].ano);

            // Salva as altera��es no arquivo
            salvarVeiculos();
            printf("Ve�culo atualizado!\n");
            return;  // Sai da fun��o
        }
    }
    // Caso a placa n�o esteja cadastrada
    printf("Placa n�o encontrada.\n");
}
// ---------- Remover Ve�culo ----------
void removerVeiculo() {
    char placa[10]; // Vari�vel para armazenar a placa digitada pelo usu�rio

    // Solicita ao usu�rio a placa do ve�culo que ele deseja remover
    printf("Placa para remover: ");
    scanf(" %[^\n]", placa);

    // Percorre a lista de ve�culos cadastrados
    for (int i = 0; i < qtdVeiculos; i++) {
        // Compara a placa informada com a placa de cada ve�culo
        if (strcmp(veiculos[i].placa, placa) == 0) {
            // Caso encontre o ve�culo, realiza a "remo��o l�gica"
            // movendo todos os ve�culos seguintes uma posi��o para tr�s
            for (int j = i; j < qtdVeiculos-1; j++)
                veiculos[j] = veiculos[j+1];
            // Diminui o contador total de ve�culos
            qtdVeiculos--;
            // Realoca o vetor para liberar a mem�ria n�o utilizada
            Veiculo *novo = realloc(veiculos, qtdVeiculos * sizeof(Veiculo));

            // Se n�o h� mais ve�culos, libera completamente o espa�o da lista
            if (qtdVeiculos == 0) { 
            free(veiculos); 
            veiculos = NULL; }
            // Caso contr�rio, atualiza o ponteiro se a realoca��o deu certo
            else if (novo) veiculos = novo;

            // Salva a nova lista atualizada no arquivo de persist�ncia
            salvarVeiculos();

            // Confirma ao usu�rio que o ve�culo foi removido
            printf("Ve�culo removido!\n");
            return;  // Sai da fun��o, pois j� encontrou e removeu o ve�culo
        }
    }
    // Se o la�o terminar sem encontrar a placa, informa que ela n�o existe
    printf("Placa n�o encontrada.\n");
}

// ---------- Ordens ----------
void abrirOrdem() {
    // Realoca o vetor de ordens para adicionar mais uma nova ordem
    OrdemServico *novo = realloc(ordens, (qtdOrdens+1)*sizeof(OrdemServico));
    if (!novo) { // Se n�o conseguiu alocar mem�ria, mostra erro e sai da fun��o
         printf("Erro de mem�ria.\n"); return; }
    ordens = novo; // Atualiza o ponteiro principal com o novo espa�o de mem�ria
    OrdemServico *nova = &ordens[qtdOrdens]; // Ponteiro para a nova posi��o (nova ordem)
    nova->id = qtdOrdens+1;  // Define o ID automaticamente com base na quantidade atual

    // ---------- Identifica��o do ve�culo ----------
    char placa[10];
    printf("Placa do ve�culo: ");
    scanf(" %[^\n]", placa); // L� a placa do ve�culo (permite espa�os, caso existam)
    Veiculo *v = NULL; // Ponteiro para armazenar o ve�culo encontrado

    // Procura no vetor de ve�culos o que tem a mesma placa informada
    for (int i = 0; i < qtdVeiculos; i++)
        if (strcmp(veiculos[i].placa, placa) == 0) v = &veiculos[i];

    // Se n�o encontrou a placa no cadastro
    if (!v) {
         printf("Ve�culo n�o encontrado!\n"); 
         return; // Sai da fun��o, pois n�o � poss�vel abrir uma ordem sem ve�culo
        }
    nova->veiculo = v;  // Associa o ve�culo encontrado � nova ordem

    // ---------- Valida��o da data de entrada ----------
    do {
        printf("Data de entrada (dd/mm/aaaa): ");
        scanf(" %[^\n]", nova->dataEntrada); // L� a data informada
        if (!dataValida(nova->dataEntrada))
            printf("Data inv�lida! Tente novamente.\n");  // Se formato incorreto, pede de novo
    } while (!dataValida(nova->dataEntrada)); // Repete at� a data ser v�lida

    // ---------- Descri��o do problema ----------
    printf("Descri��o do problema: ");
    scanf(" %[^\n]", nova->descricao);  // L� a descri��o completa do problema

    // Define o status inicial da ordem como "Aguardando Avalia��o"
    nova->status = AGUARDANDO_AVALIACAO;

    // Atualiza a quantidade total de ordens e salva tudo em arquivo
    qtdOrdens++;
    salvarOrdens();

    // Confirma ao usu�rio que a ordem foi aberta com sucesso
    printf("Ordem aberta! ID: %d\n", nova->id);
}
//---------- Atualizar Ordem ----------
void atualizarOrdem() {
    int id;
    printf("ID da ordem para editar: ");
    scanf("%d",&id); // L� o ID da ordem de servi�o que o usu�rio quer atualizar

    // Percorre todas as ordens cadastradas para encontrar a que tem o ID informado
    for (int i = 0; i < qtdOrdens; i++) {
        if (ordens[i].id == id) {  // Se encontrou a ordem com o ID correspondente

            // Atualiza a descri��o da ordem
            printf("Nova descri��o: ");
            scanf(" %[^\n]", ordens[i].descricao); // L� a nova descri��o completa (incluindo espa�os)

            // Alterar data (valida��o)
            char novaData[11];
            printf("Nova data de entrada (dd/mm/aaaa) ou ENTER para manter (%s): ", ordens[i].dataEntrada);

            getchar(); // Limpa o buffer do teclado para evitar que o ENTER anterior atrapalhe a leitura

             // L� a nova data digitada (ou ENTER, se o usu�rio quiser manter a antiga)
            if (fgets(novaData, sizeof(novaData), stdin)) {

                // Remove o '\n' que o fgets adiciona ao final da string
                size_t L = strlen(novaData);
                if (L > 0 && novaData[L-1] == '\n') novaData[L-1] = '\0';

                // Se o usu�rio digitou algo (ou seja, n�o apenas ENTER)
                if (strlen(novaData) > 0) {

                    // Valida o formato da data antes de aceitar
                    while (!dataValida(novaData)) {
                        printf("Data inv�lida! Digite novamente (dd/mm/aaaa): ");
                        fgets(novaData, sizeof(novaData), stdin);
                        L = strlen(novaData);
                        if (L > 0 && novaData[L-1] == '\n') novaData[L-1] = '\0';
                    }
                    // Se for v�lida, substitui a data antiga pela nova
                    strcpy(ordens[i].dataEntrada, novaData);
                }
            }
            // ---------- Atualiza��o do status da ordem ----------
            printf("Novo status (1-Aguardando 2-Reparo 3-Finalizado 4-Entregue): ");
            int s; scanf("%d",&s);

            // Valida o valor digitado (deve ser de 1 a 4)
            if (s >= 1 && s <= 4) 
            ordens[i].status = (Status)s; // Converte o n�mero para o tipo Status
            else printf("Status inv�lido. Mantendo o anterior.\n");

            // Salva todas as altera��es no arquivo
            salvarOrdens();
            printf("Ordem atualizada!\n");
            return; // Encerra a fun��o ap�s atualizar a ordem
        }
    }
    // Caso o ID n�o exista na lista de ordens
    printf("ID n�o encontrado.\n");
}
// Fun��o: removerOrdem
// Objetivo: remover uma ordem de servi�o do sistema com base no seu ID
void removerOrdem() {
    int id;

     // Solicita ao usu�rio o ID da ordem que ele deseja remover
    printf("ID para remover: ");
    scanf("%d",&id);

     // Percorre todas as ordens cadastradas
    for (int i = 0; i < qtdOrdens; i++) {

        // Verifica se o ID informado corresponde a uma das ordens existentes
        if (ordens[i].id == id) {

            // Desloca todas as ordens seguintes uma posi��o para tr�s,
            // sobrescrevendo a ordem removida (efeito de "apagar" ela do meio do vetor)
            for (int j = i; j < qtdOrdens-1; j++)
                ordens[j] = ordens[j+1];

            // Diminui o contador de ordens ativas    
            qtdOrdens--;

            // Realoca a mem�ria para ajustar o tamanho do vetor 'ordens'
            OrdemServico *novo = realloc(ordens, qtdOrdens * sizeof(OrdemServico));
            if (qtdOrdens == 0) { free(ordens); ordens = NULL; }

            // Caso contr�rio, se o realloc foi bem-sucedido, atualiza o ponteiro principal
            else if (novo) ordens = novo;

            // Salva as altera��es no arquivo, garantindo persist�ncia dos dados
            salvarOrdens();

            // Informa ao usu�rio que a remo��o foi conclu�da
            printf("Ordem removida!\n");
            return;
        }
    }
    // Se o loop terminar sem encontrar o ID, significa que a ordem n�o existe
    printf("ID n�o encontrado.\n");
}

// ---------- Listagens ----------

// Fun��o: listarClientes
// Objetivo: exibir na tela todos os clientes cadastrados no sistema
void listarClientes() {
    printf("\n--- LISTA DE CLIENTES ---\n");
    for (int i = 0; i < qtdClientes; i++) {
        printf("%d - %s | CPF: %s | Tel: %s\n", i+1, clientes[i].nome, clientes[i].cpf, clientes[i].telefone);
    }
}
// Fun��o: listarVeiculos
// Objetivo: exibir todos os ve�culos cadastrados e seus respectivos donos
void listarVeiculos() {
    printf("\n--- LISTA DE VE�CULOS ---\n");
    // Percorre o vetor de ve�culos
    for (int i = 0; i < qtdVeiculos; i++) {
        // Exibe placa, modelo, ano e o nome do dono do ve�culo.
        // Caso o ponteiro 'dono' seja nulo, mostra "Desconhecido" (evita erro de acesso).
        printf("%d - %s | Modelo: %s | Ano: %d | Dono: %s\n", i+1,
            veiculos[i].placa, veiculos[i].modelo, veiculos[i].ano,
            veiculos[i].dono ? veiculos[i].dono->nome : "Desconhecido");
    }
}
void listarOrdens() {
    printf("\n--- ORDENS DE SERVI�O ---\n");
    for (int i = 0; i < qtdOrdens; i++) {
        printf("ID %d | Ve�culo: %s | Data: %s | Status: %s | Problema: %s\n",
            ordens[i].id,
            ordens[i].veiculo ? ordens[i].veiculo->placa : "Desconhecido", // mostra a placa ou "Desconhecido" se n�o houver ve�culo associado
            ordens[i].dataEntrada, // mostra a data de entrada da OS
            statusTexto(ordens[i].status), // converte o status (enum) em texto leg�vel
            ordens[i].descricao // mostra a descri��o do problema
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
        printf("\n===== OFICINA MEC�NICA =====\n");
        printf("1 - Cadastrar Cliente\n2 - Atualizar Cliente\n3 - Remover Cliente\n4 - Listar Clientes\n");
        printf("5 - Cadastrar Ve�culo\n6 - Atualizar Ve�culo\n7 - Remover Ve�culo\n8 - Listar Ve�culos\n");
        printf("9 - Abrir Ordem\n10 - Atualizar Ordem\n11 - Remover Ordem\n12 - Listar Ordens\n");
        printf("0 - Sair\nEscolha: ");
        if (scanf("%d",&opc) != 1) { // evita loop infinito em entrada inv�lida
            while (getchar() != '\n') {}
            opc = -1;
        }
        switch(opc) {
            case 1: cadastrarCliente();  // Chama a fun��o que cadastra um novo cliente
            break;
            case 2: atualizarCliente();  // Permite editar os dados de um cliente existente
            break;
            case 3: removerCliente();    // Remove um cliente cadastrado pelo CPF
            break;
            case 4: listarClientes();    // Exibe todos os clientes cadastrados na oficina
            break;
            case 5: cadastrarVeiculo();  // Cadastra um novo ve�culo e associa a um cliente
            break;
            case 6: atualizarVeiculo();  // Altera dados do ve�culo (modelo, ano etc.)
            break;
            case 7: removerVeiculo();    // Exclui um ve�culo do sistema pela placa
            break;
            case 8: listarVeiculos();    // Mostra todos os ve�culos cadastrados
            break;
           // ---------- Ordens de Servi�o ----------
            case 9: abrirOrdem();        // Abre uma nova ordem de servi�o para um ve�culo     
            break;
            case 10: atualizarOrdem();   // Atualiza informa��es ou status de uma ordem existente
            break;
            case 11: removerOrdem();     // Exclui uma ordem de servi�o do sistema
            break;
            case 12: listarOrdens();     // Lista todas as ordens cadastradas com seus status
            break;
           // ---------- Sa�da ----------
            case 0: printf("Encerrando...\n"); // Mensagem exibida ao encerrar o programa
            break;
            // ---------- Op��o Inv�lida ----------
            default: printf("Op��o inv�lida!\n");  // Caso o usu�rio digite um n�mero fora das op��es do menu
        }
    } while (opc != 0);

    // Libera toda a memoria alocada antes de sair
    free(clientes);
    free(veiculos);
    free(ordens);
    return 0;
}
