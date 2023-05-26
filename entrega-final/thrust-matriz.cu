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

int N, M;

std::cin >> N >> M;

thrust::host_vector<int> hv_start_times(N+1);
thrust::host_vector<int> hv_end_times(N+1);
thrust::host_vector<int> hv_categories(N+1);
thrust::host_vector<int> hv_L(M+1);

for (int i = 0; i < M; i++) {
    std::cin >> hv_L[i];
}

// Ler os dados do arquivo de entrada
for (int i = 0; i < N; i++) {
    std::cin >> hv_start_times[i] >> hv_end_times[i] >> hv_categories[i];
}
// Carregar os dados do arquivo de entrada na memória da GPU
thrust::device_vector<int> start_times(hv_start_times);
thrust::device_vector<int> end_times(hv_end_times);
thrust::device_vector<int> categories(hv_categories);
thrust::device_vector<int> L(hv_L);


// Criar a matriz de programação dinâmica
thrust::device_vector<int> dp((N+1) * (M+1), 0);

// Inicializar a primeira linha da matriz com zeros
thrust::fill(dp.begin(), dp.begin() + M + 1, 0);

// Preencher a matriz com as soluções para subproblemas menores
for (int i = 1; i <= N; i++) {
    for (int j = 1; j <= M; j++) {
        // Encontrar o número máximo de filmes que podem ser assistidos até o filme i e categoria j
        int max_count = 0;
        for (int k = 0; k < i; k++) {
            if (categories[k] == j && end_times[k] <= start_times[i] && dp[(k*(M+1)) + j-1] + 1 <= L[j]) {
                max_count = max(max_count, dp[(k*(M+1)) + j-1] + 1);
            } else {
                max_count = max(max_count, dp[(k*(M+1)) + j]);
            }
        }
        dp[(i*(M+1)) + j] = max_count;
    }
}

// Encontrar o número máximo de filmes que podem ser assistidos
int max_count = 0;
for (int j = 1; j <= M; j++) {
    max_count = max(max_count, dp[(N*(M+1)) + j]);
}

// Escrever o resultado no arquivo de saída
std::cout << max_count << std::endl;