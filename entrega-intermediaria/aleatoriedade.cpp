#include <iostream>     // std::cout, std::cin, std::endl
#include <iomanip>      // std::setprecision
#include <cmath>        // std::pow
#include <vector>       // std::vector
#include <algorithm>    // std::sort
#include <bitset>       // std::bitset
#include <map>          // std::map
#include <random>       // std::default_random_engine

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
    if(inicio == fim) horario.set(inicio);
    else {
        for (int i = inicio; i < fim; i++) horario.set(i);
    }
    return horario;
}

int fill_maratona(Maratona *maratona , std::map<int, std::vector<Filme>> *filmes, std::vector<int> *keys, std::vector<Categoria> *categorias, int n_filmes, int n_categorias){
    int max_filmes = 0;
    int n_keys = keys->size();

    for(int i = 0; i < n_keys; i++){
        if (rand() % 4 != 0) {
            int n_filmes = filmes->at(keys->at(i)).size();
            for (int j = 0; j < n_filmes; j++)
            {
                std::bitset<24> disponivel = maratona->disponibilidade & filmes->at(keys->at(i))[j].horario;
                if(disponivel != 0) continue;
                if(categorias->at(filmes->at(keys->at(i))[j].categoria-1).capacidade == 0) continue;
                maratona->disponibilidade = maratona->disponibilidade | filmes->at(keys->at(i))[j].horario;
                maratona->filmes.push_back(filmes->at(keys->at(i))[j]);
                categorias->at(filmes->at(keys->at(i))[j].categoria-1).capacidade--;
                max_filmes++;
                break;
            }
        } else {
            int n_filmes = filmes->at(keys->at(i)).size() - 1;
            std::vector<Filme> filmes_disponiveis;
            for (int j = 1; j < n_filmes; j++)
            {
                std::bitset<24> disponivel = maratona->disponibilidade & filmes->at(keys->at(i))[j].horario;
                if(disponivel != 0) continue;
                if(categorias->at(filmes->at(keys->at(i))[j].categoria-1).capacidade == 0) continue;
                filmes_disponiveis.push_back(filmes->at(keys->at(i))[j]);
            }
            if (filmes_disponiveis.size() > 0) {
                int random_index = rand() % filmes_disponiveis.size();
                maratona->disponibilidade = maratona->disponibilidade | filmes_disponiveis[random_index].horario;
                maratona->filmes.push_back(filmes_disponiveis[random_index]);
                categorias->at(filmes_disponiveis[random_index].categoria-1).capacidade--;
                max_filmes++;
            }
        }
    }

    return max_filmes;
}

//==============================================================================
// Main
//==============================================================================

int main(){
    int n_filmes, n_categorias;
    int max_filmes;
    std::map<int, std::vector<Filme>> filmes;
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
        filme.id = i+1;
        std::cin >> filme.inicio >> filme.fim >> filme.categoria;
        if (filme.inicio < 0 || filme.fim < 0 || filme.fim < filme.inicio) continue;
        filme.horario = get_horario(filme.inicio, filme.fim);
        filmes[filme.fim].push_back(filme);
    };

    std::vector<int> dict_keys;

    for (auto element : filmes)
    {
        std::sort(element.second.begin(), element.second.end(), [](const Filme& a, const Filme& b) {
            return a.inicio > b.inicio;
        });
        dict_keys.push_back(element.first);
    }

    std::sort(dict_keys.begin(), dict_keys.end(), [](const int& a, const int& b) {
        return a < b;
    });

    Maratona maratona;

    max_filmes = fill_maratona(&maratona, &filmes, &dict_keys, &categorias, n_filmes, n_categorias);

    // std::cout << maratona.disponibilidade << std::endl;
    // int size_maratona = maratona.filmes.size();
    // for(int i = 0; i < size_maratona; i++){
    //     std::cout << maratona.filmes[i].id << " " << maratona.filmes[i].inicio << " " << maratona.filmes[i].fim << " " << maratona.filmes[i].categoria << std::endl;
    // }

    std::cout << max_filmes << std::endl;
}