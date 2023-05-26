#include <iostream>
#include <iomanip>      // std::setprecision
#include <cmath>        // std::pow
#include <vector>       // std::vector
#include <algorithm>    // std::sort
#include <bitset>

//==============================================================================
// Structs
//==============================================================================

struct Filme{
    int id;
    int inicio;
    int fim;
    int categoria;
    std::bitset<24> horario;
};

struct Categoria{
    int id;
    int capacidade;
};

struct Maratona{
    std::bitset<24> disponibilidade;
    std::vector<Filme> filmes;
};

//==============================================================================
// Funções
//==============================================================================

std::bitset<24> get_horario(int inicio, int fim){
    std::bitset<24> horario;
    if (inicio > fim) {
        for (int i = inicio; i < 24; i++) horario.set(i);
        for (int i = 0; i < fim; i++) horario.set(i);
    }
    else if(inicio == fim) horario.set(inicio);
    else {
        for (int i = inicio; i < fim; i++) horario.set(i);
    }
    return horario;
}

int fill_maratona(Maratona *maratona , std::vector<Filme> *filmes, std::vector<Categoria> *categorias, int n_filmes, int n_categorias){
    int max_filmes = 0;

    for(int i = 0; i < n_filmes; i++){
        std::bitset<24> disponivel = maratona->disponibilidade & filmes->at(i).horario;
        if(disponivel != 0) continue;
        if(categorias->at(filmes->at(i).categoria-1).capacidade == 0) continue;
        maratona->disponibilidade = maratona->disponibilidade | filmes->at(i).horario;
        maratona->filmes.push_back(filmes->at(i));
        categorias->at(filmes->at(i).categoria-1).capacidade--;
        max_filmes++;
    }

    return max_filmes;
}

//==============================================================================
// Main
//==============================================================================
int main(){
    int n_filmes, n_categorias;
    int max_filmes;
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
        filme.horario = get_horario(filme.inicio, filme.fim);
        filmes.push_back(filme);
    };

    n_filmes = filmes.size();

    std::sort(filmes.begin(), filmes.end(), [](const Filme& a, const Filme& b) {
        return a.fim < b.fim;
    });

    Maratona maratona;

    max_filmes = fill_maratona(&maratona, &filmes, &categorias, n_filmes, n_categorias);

    // std::cout << maratona.disponibilidade << std::endl;
    // int size_maratona = maratona.filmes.size();
    // for(int i = 0; i < size_maratona; i++){
    //     std::cout << maratona.filmes[i].id << " " << maratona.filmes[i].inicio << " " << maratona.filmes[i].fim << " " << maratona.filmes[i].categoria << std::endl;
    // }

    std::cout << max_filmes << std::endl;
}