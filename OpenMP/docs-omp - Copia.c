#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

int C, D, S;
typedef struct Doc{
    int ID;
    double* S;
    int numArmario;
} Documento;

typedef struct Armario{
    int numArmario;
    double* mediaAssuntos;
} Armario;

Documento* lerFicheiro();

Documento* lerFicheiro(char str[]){
	FILE* fp;
    fp = fopen(str, "r");
    fscanf(fp, "%d %d %d", &C, &D, &S);

   Documento* vetorDocumentos = (Documento*) malloc(D * sizeof(Documento));

    for (int i = 0; i < D; i++) {
        int ID;
        double* valoresS = (double*) malloc(S * sizeof(double));
        fscanf(fp, "%d", &ID);
        for (int j = 0; j < S; j++) {
            fscanf(fp, "%lf", &valoresS[j]);
        }

		vetorDocumentos[i].ID = ID;
		vetorDocumentos[i].numArmario = i % C;
		vetorDocumentos[i].S = valoresS;
    }

    fclose(fp);
	return vetorDocumentos;
}

void imprimirDocumentos(Documento* vetorDocumentos) {
    for (int i = 0; i < D; i++) {
        printf("%d  %d ", vetorDocumentos[i].ID, vetorDocumentos[i].numArmario);
        printf("\n");
    }
}


Armario* calcularMedias(Documento* doc) {
    Armario* vetorArmarios = (Armario*) malloc(C * sizeof(Armario));
    for (int i = 0; i < C; i++) {
        vetorArmarios[i].numArmario = i;
        vetorArmarios[i].mediaAssuntos = (double*) malloc(S * sizeof(double)); 
    }
    
    int num_threads = 4; // número de threads otimizado para o Core i5 da 7ª geração
    omp_set_num_threads(num_threads);
    
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < C; i++){
        double* somaAssuntos = (double*) malloc(S * sizeof(double)); // Vetor para armazenar a soma de cada assunto
        for (int j = 0; j < S; j++) {
            somaAssuntos[j] = 0.00000; // Inicialize a soma de cada assunto como zero
        }
        int numDocumentos = 0; // Variável para contar o número total de documentos no armário
        for (int j = 0; j < D; j++){
            if(doc[j].numArmario == vetorArmarios[i].numArmario){
                for(int k=0; k<S; k++){
                    somaAssuntos[k] += floor(doc[j].S[k]*10)/10; // Adiciona os valores do assunto ao vetor de soma
                }
                numDocumentos++; // Incrementa o contador de documentos
            }
        }
        for (int j = 0; j < S; j++) {
            vetorArmarios[i].mediaAssuntos[j] = somaAssuntos[j] / numDocumentos; // Calcula a média de cada assunto
        }
        free(somaAssuntos); // Libera a memória do vetor de soma
    }
    return vetorArmarios;
}


Documento* moverDocumento(Armario* arm, Documento* doc){
    #pragma omp parallel for schedule(static) num_threads(4)
    for (int i = 0; i < D; i++) {
        double menorDistancia = INFINITY;
        int armarioDestino = doc[i].numArmario;
        for (int j = 0; j < C; j++) {
            double distancia = 0.0;
            for (int k = 0; k < S; k++) {
                distancia = distancia + pow(doc[i].S[k] - arm[j].mediaAssuntos[k], 2);
            }
            if (distancia < menorDistancia) {
                menorDistancia = distancia;
                armarioDestino = j;
            }
        }
        doc[i].numArmario = armarioDestino;
    }
    return doc;
}


void gravarSaida(Documento* vetorDocumentos) {
    FILE* fp;
    fp = fopen("docs.out", "w");
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo\n");
        return;
    }

    #pragma omp parallel for
    for (int i = 0; i < D; i++) {
        fprintf(fp, "%d %d\n", vetorDocumentos[i].ID, vetorDocumentos[i].numArmario);
    }

    #pragma omp critical
    {
        fclose(fp);
    }
}

int main(int argc, char *argv[]) {
    Documento *doc;
    Armario* arm = NULL; // inicializa listaArmarios como NULL

    int num_threads = omp_get_num_procs();
    omp_set_num_threads(num_threads);

    doc = lerFicheiro(argv[1]);

    arm = calcularMedias(doc);
    doc = moverDocumento(arm, doc);

    imprimirDocumentos(doc);
    gravarSaida(doc);

    return 0;
}