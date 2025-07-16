#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <chrono>
#include <thread> // Essencial para o paralelismo

using namespace std;
namespace fs = filesystem;

// A função de ordenação não muda.
void bubble_sort(int lista[], int tamanho_da_lista) {
    for (int i = 0; i < tamanho_da_lista - 1; i++) {
        for (int j = 0; j < tamanho_da_lista - i - 1; j++) {
            if (lista[j] > lista[j + 1]) {
                int variavel_temporaria = lista[j];
                lista[j] = lista[j + 1];
                lista[j + 1] = variavel_temporaria;
            }
        }
    }
}

/**
 * @brief Processa todos os arquivos de uma pasta, ordenando-os e salvando os resultados.
 * Esta função será executada por cada thread.
 * @param pasta_entrada A pasta contendo os arquivos .txt (ex: "Entradas/Aleatorios/").
 * @param nome_cenario O nome do cenário para o arquivo de saída (ex: "Aleatorios").
 */
void processar_pasta(const string& pasta_entrada, const string& nome_cenario) {
    string arquivo_saida_nome = "Resultado_bubble_sort_" + nome_cenario + ".txt";
    ofstream saida(arquivo_saida_nome);

    if (!saida.is_open()) {
        cerr << "[" << nome_cenario << "] Erro: Nao foi possivel criar o arquivo de saida " << arquivo_saida_nome << endl;
        return;
    }

    cout << "[" << nome_cenario << "] Iniciando processamento da pasta " << pasta_entrada << "..." << endl;

    try {
        for (const auto& entry : fs::directory_iterator(pasta_entrada)) {
            if (entry.is_regular_file()) {
                string caminho = entry.path().string();
                ifstream arquivo(caminho);

                if (!arquivo.is_open()) {
                    cerr << "[" << nome_cenario << "] Erro ao abrir: " << caminho << endl;
                    continue;
                }
                
                // Conta os números para alocar memória
                int count = 0, temp;
                while (arquivo >> temp) count++;
                arquivo.clear();
                arquivo.seekg(0);

                // Aloca e preenche a lista
                int* lista = new int[count];
                for (int i = 0; i < count; i++) arquivo >> lista[i];
                arquivo.close();

                // Mede o tempo de ordenação
                auto inicio_sort = chrono::high_resolution_clock::now();
                bubble_sort(lista, count);
                auto fim_sort = chrono::high_resolution_clock::now();
                auto tempo_sort = chrono::duration<double, milli>(fim_sort - inicio_sort).count();

                // Salva o resultado
                saida << entry.path().filename().string() << " | tempo_sort: " << tempo_sort << " ms" << endl;
                cout << "[" << nome_cenario << "] Processado: " << entry.path().filename() << endl;

                delete[] lista;
            }
        }
    } catch (const fs::filesystem_error& e) {
        cerr << "[" << nome_cenario << "] Erro de filesystem: " << e.what() << endl;
    }

    saida.close();
    cout << "[" << nome_cenario << "] ✅ Concluido! Resultados salvos em " << arquivo_saida_nome << endl;
}

int main() {
    // Lista dos cenários que vamos processar em paralelo
    vector<string> cenarios = {"Aleatorios", "Decrescentes", "Ordenados", "ParcialmenteOrdenados"};
    
    // Vetor para guardar nossas threads
    vector<thread> threads;

    cout << "Disparando 4 threads para processar as pastas em paralelo..." << endl;

    // Dispara uma thread para cada cenário
    for (const string& cenario : cenarios) {
        string pasta = "Entradas/" + cenario + "/";
        threads.emplace_back(processar_pasta, pasta, cenario);
    }

    // Espera todas as threads terminarem seu trabalho
    for (auto& th : threads) {
        if (th.joinable()) {
            th.join();
        }
    }

    cout << "\n🚀 Todos os cenarios foram processados com sucesso!" << endl;
    
    return 0;
}