#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <chrono>
#include <vector>
#include <thread>
#include <mutex>

using namespace std;
namespace fs = filesystem;

mutex mtx; // para proteger escrita no arquivo CSV

void insertionSort(int* vet, int n) {
    for (int i = 1; i < n; i++) {
        int aux = vet[i];
        int j = i - 1;
        while (j >= 0 && vet[j] > aux) {
            vet[j + 1] = vet[j];
            j--;
        }
        vet[j + 1] = aux;
    }
}

double executarOrdenacao(const vector<int>& dados) {
    int* vetor = new int[dados.size()];
    copy(dados.begin(), dados.end(), vetor);

    auto inicio = chrono::high_resolution_clock::now();
    insertionSort(vetor, dados.size());
    auto fim = chrono::high_resolution_clock::now();

    delete[] vetor;

    chrono::duration<double, milli> duracao = fim - inicio;
    return duracao.count();
}

void processarArquivo(const fs::directory_entry& entry, ofstream& csv) {
    ifstream arquivo(entry.path());
    if (!arquivo.is_open()) {
        cerr << "Erro ao abrir o arquivo: " << entry.path().filename() << endl;
        return;
    }

    vector<int> dados;
    int valor;
    while (arquivo >> valor) {
        dados.push_back(valor);
    }
    arquivo.close();

    vector<double> tempos(6);
    vector<thread> threads;

    // Criar 6 threads para executar a ordenação em paralelo
    for (int i = 0; i < 6; ++i) {
        threads.emplace_back([&dados, &tempos, i]() {
            tempos[i] = executarOrdenacao(dados);
        });
    }

    // Aguardar todas as threads
    for (auto& t : threads) {
        t.join();
    }

    // Escrever resultados no CSV (com mutex)
    lock_guard<mutex> lock(mtx);
    csv << entry.path().filename() << "," << dados.size();
    for (int i = 0; i < 6; ++i) {
        csv << "," << tempos[i];
    }
    csv << endl;
    csv.flush();
}

int main() {
    string pasta = "./Entradas/Ordenados";  // pasta com seus arquivos .txt
    string arquivo_saida = "temposO.csv";

    ofstream csv(arquivo_saida);
    if (!csv.is_open()) {
        cerr << "Erro ao criar o arquivo de saída." << endl;
        return 1;
    }

    csv << "Arquivo,Tamanho,V1 (ms),V2 (ms),V3 (ms),V4 (ms),V5 (ms),V6 (ms)" << endl;

    vector<thread> arquivos_threads;

    // Criar uma thread para cada arquivo
    for (const auto& entry : fs::directory_iterator(pasta)) {
        if (entry.path().extension() == ".txt") {
            arquivos_threads.emplace_back(processarArquivo, entry, ref(csv));
        }
    }

    // Aguardar todas as threads que processam arquivos
    for (auto& t : arquivos_threads) {
        t.join();
    }

    csv.close();
    cout << "Todos os tempos foram registrados em '" << arquivo_saida << "'." << endl;

    return 0;
}