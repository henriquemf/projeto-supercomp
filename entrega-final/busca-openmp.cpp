#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <algorithm>
#include <bitset>
#include <omp.h> // OpenMP library

//==============================================================================
// Structs
//==============================================================================

struct Filme{
    int id;
    int inicio;
    int fim;
    int categoria;
};

struct Proc_Filme{
    int id;
    std::bitset<24> horario;
    int categoria;
};

struct Categoria{
    int id;
    int capacidade;
};

//==============================================================================
// Funtions
//==============================================================================

std::vector<Proc_Filme> get_horario(std::vector<Filme> Filmes, int N) {
    std::vector<Proc_Filme> filmes_processados(N);

    #pragma omp parallel for 
    for (int i = 0; i < N; i++) {
        int inicio = Filmes[i].inicio;
        int fim = Filmes[i].fim;

        std::bitset<24> horario;

        if (inicio > fim) {
            for (int i = inicio; i < 24; i++) horario.set(i);
            for (int i = 0; i < fim; i++) horario.set(i);
        }
        else if(inicio == fim) {
            horario.set(inicio);
        }
        else {
            for (int i = inicio; i < fim; i++) horario.set(i);
        }

        Proc_Filme proc_filme;
        proc_filme.id = Filmes[i].id;
        proc_filme.horario = horario;
        proc_filme.categoria = Filmes[i].categoria;
        filmes_processados[i] = proc_filme;
    }

    return filmes_processados;
};

//==============================================================================
// Main
//==============================================================================

int main() {
    int n_filmes, n_categorias;
    std::vector<Filme> filmes;
    std::vector<Categoria> categorias;

    std::cin >> n_filmes >> n_categorias;

    for(int i = 1; i <= n_categorias; i++){
        Categoria categoria;
        categoria.id = i;
        std::cin >> categoria.capacidade;
        categorias.push_back(categoria);
    }

    for(int i = 0; i < n_filmes; i++){
        Filme filme;
        filme.id = i;
        std::cin >> filme.inicio >> filme.fim >> filme.categoria;
        if (filme.fim == 0) filme.fim = 24;
        if (filme.inicio < 0 || filme.fim < 0 || filme.fim < filme.inicio) continue;
        filmes.push_back(filme);
    };

    n_filmes = filmes.size();

    std::vector<Proc_Filme> filmes_processados = get_horario(filmes, n_filmes);

    std::vector<std::vector<std::bitset<64>>> possibilidades_por_thread(omp_get_max_threads());

    long int max_filmes = pow(2, n_filmes);
    #pragma omp parallel for
    for(long int i = 0; i < max_filmes; i++) {
        std::vector<Categoria> copy_categorias = categorias;
        std::bitset<24> maratona;
        std::bitset<64> x(i);

        for (int j = 0; j < n_filmes; j++) {
            if (x[j] == 1) {
                std::bitset<24> disponivel = maratona & filmes_processados[j].horario;
                if(disponivel != 0) break;
                if(copy_categorias[filmes_processados[j].categoria-1].capacidade == 0) break;
                maratona = maratona | filmes_processados[j].horario;
                copy_categorias[filmes_processados[j].categoria-1].capacidade--;
            }
            if (j == n_filmes-1) {
                // Aqui nós adicionamos as possibilidades ao vetor correspondente ao thread atual
                possibilidades_por_thread[omp_get_thread_num()].push_back(x);
            }
        }
    }

    std::vector<std::bitset<64>> possibilidades;
    for(auto& possibilidades_thread : possibilidades_por_thread) {
        possibilidades.insert(possibilidades.end(), possibilidades_thread.begin(), possibilidades_thread.end());
    }

    int n_possibilidades = possibilidades.size();
    std::vector<int> n_filmes_possibilidades(n_possibilidades);
    
    for(int i = 0; i < n_possibilidades; i++) {
        for (int j = 0; j < 64; j++) {
            if (possibilidades[i][j] == 1) n_filmes_possibilidades[i]++;
        }
    }

    int max = *std::max_element(n_filmes_possibilidades.begin(), n_filmes_possibilidades.end());
    std::cout << max << std::endl;

    return 0;
}