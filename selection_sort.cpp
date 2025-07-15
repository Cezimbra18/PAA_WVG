#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <chrono>
#include <cstdlib>

using namespace std;
namespace fs = filesystem;

int retorna_minimo(int lista[], int tamanho_da_lista){
    int lista_minimo = lista[0];
    for(int i = 1; i < tamanho_da_lista; i++){
        if(lista_minimo > lista[i]){
            lista_minimo = lista[i];
        }
    }
    return lista_minimo;
}

// Código de referencia
void selection_sort(int lista[], int tamanho_da_lista){
    for (int i = 0; i < tamanho_da_lista - 1; i++) {
        int indice_de_minimo = i;
        for (int j = i + 1; j < tamanho_da_lista; j++) {
            if (lista[j] < lista[indice_de_minimo]) {
                indice_de_minimo = j;
            }
        }
        if (indice_de_minimo != i) {
            int variavel_temporaria = lista[i];
            lista[i] = lista[indice_de_minimo];
            lista[indice_de_minimo] = variavel_temporaria;
        }
    }
}

int main() {
    string pasta = "arquivos_lista/";  
    string arquivo_saida = "Resultado_selection_sort.txt";

    ofstream saida(arquivo_saida);
    if (!saida.is_open()) {
        cerr << "Erro ao abrir o arquivo de saída." << endl;
        return 1;
    }

    for (const auto& entry : fs::directory_iterator(pasta)) {
        if (entry.is_regular_file()) {
            string caminho = entry.path().string();
            ifstream arquivo(caminho);

            if (!arquivo.is_open()) {
                cerr << "Erro ao abrir: " << caminho << endl;
                continue;
            }

            int count = 0, variavel_temporaria;
            while (arquivo >> variavel_temporaria) count++;
            arquivo.clear();
            arquivo.seekg(0);

            int* lista = new int[count];
            for (int i = 0; i < count; i++) {
                arquivo >> lista[i];
            }
            arquivo.close();

            int minimo = retorna_minimo(lista, count);
            (void)minimo;  

            auto inicio_sort = chrono::high_resolution_clock::now();
            selection_sort(lista, count);
            auto fim_sort = chrono::high_resolution_clock::now();
            auto tempo_sort = chrono::duration<double, milli>(fim_sort - inicio_sort).count();

            saida << entry.path().filename().string()
                  << " | tempo_sort: " << tempo_sort << " ms" << endl;

            delete[] lista;
            cout << "Processado: " << entry.path().filename() << endl;
        }
    }

    saida.close();
    cout << "Resultados salvos em " << arquivo_saida << endl;
    return EXIT_SUCCESS;
}
