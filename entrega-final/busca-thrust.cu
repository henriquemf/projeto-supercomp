#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <chrono>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <random>
// Importações do thrust
#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include <thrust/generate.h>
#include <thrust/functional.h>
#include <thrust/copy.h>

using namespace std;


struct analisa_configuracao {
    int n_filmes;
    int n_categorias;
    int *start_time;
    int *end_time;
    int *cat_id;
    int *categorias;

    analisa_configuracao(int _n_filmes,
                         int _n_categorias,
                         int* _start_time,
                         int* _end_time,
                         int* _cat_id,
                         int* _categorias
                        ): n_filmes(_n_filmes), 
                           n_categorias(_n_categorias),
                           start_time(_start_time),
                           end_time(_end_time),
                           cat_id(_cat_id),
                           categorias(_categorias)
                        {};

    __device__
    int operator()(const int& x){
        int disponibilidade[24];
        int copy_categorias[10];

        for (int k = 0; k < 24; k++){
            disponibilidade[k] = 0;
        }

        for (int l = 0; l < n_categorias; l++){
            copy_categorias[l] = categorias[l];
        }

        int max_count = 0;

        for (int i = 0; i < n_filmes; i++){
            if (x & (1<<i)){
                if (copy_categorias[cat_id[i] - 1] <= 0) return -1;
                for (int j = start_time[i]; j < end_time[i]; j++){
                    if (disponibilidade[j] == 1) return -1;
                    disponibilidade[j] = 1;
                }
                copy_categorias[cat_id[i] - 1]--;
                max_count++;
            }
        }
        return max_count;
    }
};


int main(){
    int n_filmes, n_categorias;

    cin >> n_filmes >> n_categorias;

    thrust::host_vector<int> categorias(n_categorias);
    thrust::host_vector<int> start_time(n_filmes);
    thrust::host_vector<int> end_time(n_filmes);
    thrust::host_vector<int> cat_id(n_filmes);

    thrust::host_vector<int> result(1);

    for (int i = 0; i < n_categorias; i++){
        cin >> categorias[i];
    }

    int n_start_time, n_end_time;

    for (int i = 0; i < n_filmes; i++){
        cin >> n_start_time >> n_end_time;
        if (n_end_time == 0) n_end_time = 24;
        if (n_start_time < 0) n_start_time = 0;
        if (n_end_time < 0) n_end_time = 0;
        if (n_end_time < n_start_time) n_end_time = 24;
        start_time[i] = n_start_time;
        end_time[i] = n_end_time;
        cin >> cat_id[i];
    }

    cout << "Carregou as entradas" << endl;

    thrust::device_vector<int> device_categorias = categorias;
    thrust::device_vector<int> device_start_time = start_time;
    thrust::device_vector<int> device_end_time = end_time;
    thrust::device_vector<int> device_cat_id = cat_id;

    cout << "Copiou para o device" << endl;

    thrust::device_vector<int> device_configuration(pow(2, n_filmes));
    thrust::sequence(device_configuration.begin(), device_configuration.end());

    cout << "Criou o vetor de configurações" << endl;


    thrust::transform(device_configuration.begin(), device_configuration.end(), device_configuration.begin(), 
                      analisa_configuracao(n_filmes, 
                                           n_categorias,
                                           thrust::raw_pointer_cast(device_start_time.data()), 
                                           thrust::raw_pointer_cast(device_end_time.data()),
                                           thrust::raw_pointer_cast(device_cat_id.data()),
                                           thrust::raw_pointer_cast(device_categorias.data())
                                          )
                     );

    cout << "Aplicou o Transform" << endl;

    thrust::device_vector<int> device_result(1);

    device_result[0] = *thrust::max_element(device_configuration.begin(), device_configuration.end());
    thrust::copy(device_result.begin(), device_result.end(), result.begin());

    cout << "Max Filmes: " << result[0] << endl;

    return 0;
}