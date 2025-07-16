#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <chrono>
#include <cstdlib> // Para EXIT_SUCCESS

using namespace std;
namespace fs = filesystem;

/**
 * rdena um array de inteiros utilizando o algoritmo BubbleSort.
 * * param lista: O array de inteiros a ser ordenado.
 * param tamanho_da_lista: O número de elementos no array.
 */

void bubble_sort(int lista[], int tamanho_da_lista) {
    // Loop externo percorre a lista. A cada iteração, o maior elemento
    // da seção não ordenada é "flutuado" para sua posição correta no final.
    for (int i = 0; i < tamanho_da_lista - 1; i++) {
        
        // Loop interno compara os elementos adjacentes.
        // O "- i" é uma otimização, pois os elementos no final da lista já estão ordenados.
        for (int j = 0; j < tamanho_da_lista - i - 1; j++) {
            
            // Se o elemento na posição 'j' for maior que o próximo ('j+1'), troca eles.
            if (lista[j] > lista[j + 1]) {
                int variavel_temporaria = lista[j];
                lista[j] = lista[j + 1];
                lista[j + 1] = variavel_temporaria;
            }
        }
    }
}

int main() {
    // Pasta onde os arquivos de entrada estão localizados
    string pasta = "Entradas/Aleatorios/";  
    
    // Nome do arquivo onde os resultados serão salvos
    string arquivo_saida = "Resultado_bubble_sort.txt";

    ofstream saida(arquivo_saida);
    if (!saida.is_open()) {
        cerr << "Erro ao abrir o arquivo de saída." << endl;
        return 1;
    }

    // Itera sobre cada arquivo na pasta especificada
    for (const auto& entry : fs::directory_iterator(pasta)) {
        if (entry.is_regular_file()) {
            string caminho = entry.path().string();
            ifstream arquivo(caminho);

            if (!arquivo.is_open()) {
                cerr << "Erro ao abrir: " << caminho << endl;
                continue;
            }

            // 1ª Leitura: Conta quantos números existem no arquivo para alocar a memória
            int count = 0, variavel_temporaria_leitura;
            while (arquivo >> variavel_temporaria_leitura) {
                count++;
            }
            
            // "Rebobina" o arquivo para o início para a segunda leitura
            arquivo.clear();
            arquivo.seekg(0);

            // Aloca dinamicamente o array com o tamanho exato
            int* lista = new int[count];

            // 2ª Leitura: Preenche o array com os números do arquivo
            for (int i = 0; i < count; i++) {
                arquivo >> lista[i];
            }
            arquivo.close();

            // Mede o tempo de execução apenas da função de ordenação
            auto inicio_sort = chrono::high_resolution_clock::now();
            
            bubble_sort(lista, count); // <--- AQUI A CHAMADA PARA O BUBBLE SORT
            
            auto fim_sort = chrono::high_resolution_clock::now();
            auto tempo_sort = chrono::duration<double, milli>(fim_sort - inicio_sort).count();

            // Escreve o resultado no arquivo de saída
            saida << entry.path().filename().string()
                  << " | tempo_sort: " << tempo_sort << " ms" << endl;

            // Libera a memória alocada para a lista
            delete[] lista;
            
            cout << "Processado: " << entry.path().filename() << endl;
        }
    }

    saida.close();
    cout << "Resultados salvos em " << arquivo_saida << endl;
    return EXIT_SUCCESS;
}