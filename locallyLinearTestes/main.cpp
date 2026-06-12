#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>
#include <random>
#include <algorithm>
#include "locallyLinear.h"

using namespace std;

void executarSimulacaoCompleta() {
    vector<int> tamanhos = { 256, 4096, 65536, 1048576, 4194304 };
    vector<double> alfas = { 0.4, 0.9 };

    const int numeroSimulacoes = 100;
    mt19937_64 rng(42);

    for(int n : tamanhos) {
        for(double alpha : alfas) {
            double loglog_n = log2(log2((double)n));
            if (loglog_n < 0) loglog_n = 0;

            int beta = floor(loglog_n / (1.0 - alpha) + 1.0);
            beta = max(beta, 1);

            int m = static_cast<int>(n * alpha); // preenchimento da tabela

            double tempoInsercaoTotal = 0;
            double tempoBuscaTotal = 0;
            double tempoInsercaoBuscaTotal = 0; 

            long long probesInsercaoTotal = 0;
            long long probesBuscaTotal = 0;

            int maxProbesInsercao = 0;
            int maxProbesBusca = 0;

            for(int sim = 0; sim < numeroSimulacoes; sim++) {
                LocallyLinearHashTable ht(n, beta);
                vector<int> chaves(m);
                
                // usa o SplitMix64 (rng()) e transforma em uma chave útil para preencher o vetor de testes (chaves[i]).
                // faz parte do conceito da pesquisa sobre o SplitMix
                for(int i = 0; i < m; i++) {
                    chaves[i] = (int)(rng() % 2147483646ULL) + 1; 
                }

                auto inicioCombinado = chrono::high_resolution_clock::now();

                auto inicioInsert = chrono::high_resolution_clock::now();
                for(int k : chaves) {
                    auto retornoDaTabela = ht.insert(k);
                    probesInsercaoTotal += retornoDaTabela.probes;
                    maxProbesInsercao = max(maxProbesInsercao, retornoDaTabela.probes);
                }
                auto fimInsert = chrono::high_resolution_clock::now();
                tempoInsercaoTotal += chrono::duration<double, milli>(fimInsert - inicioInsert).count();

                auto inicioBusca = chrono::high_resolution_clock::now();
                for(int k : chaves) {
                    auto retornoDaTabela = ht.search(k);
                    probesBuscaTotal += retornoDaTabela.probes;
                    maxProbesBusca = max(maxProbesBusca, retornoDaTabela.probes);
                }
                auto fimBusca = chrono::high_resolution_clock::now();
                tempoBuscaTotal += chrono::duration<double, milli>(fimBusca - inicioBusca).count();

                auto fimCombinado = chrono::high_resolution_clock::now();

                tempoInsercaoBuscaTotal += chrono::duration<double, milli>(fimCombinado - inicioCombinado).count();
            }

            double mediaTempoInsercao = tempoInsercaoTotal / numeroSimulacoes;
            double mediaTempoBusca = tempoBuscaTotal / numeroSimulacoes;
            double mediaTempoCombinado = tempoInsercaoBuscaTotal / numeroSimulacoes;

            double mediaTempoInsercaoOp = tempoInsercaoTotal / (numeroSimulacoes * m);
            double mediaTempoBuscaOp = tempoBuscaTotal / (numeroSimulacoes * m);
            double mediaTempoCombinadoOp = tempoInsercaoBuscaTotal / (numeroSimulacoes * m);

            double mediaProbesInsercao = (double)probesInsercaoTotal / (numeroSimulacoes * m);
            double mediaProbesBusca = (double)probesBuscaTotal / (numeroSimulacoes * m);

            cout << "\n=====================================\n";
            cout << "n = " << n << " alpha = " << alpha << " beta = " << beta << "\n\n";

            cout << "INSERCAO\n";
            cout << "Tempo medio conjunto : " << mediaTempoInsercao << " ms\n";
            cout << "Tempo medio operacao : " << mediaTempoInsercaoOp << " ms/op\n";
            cout << "Average probes       : " << mediaProbesInsercao << "\n";
            cout << "Maximum probes       : " << maxProbesInsercao << "\n\n";

            cout << "BUSCA\n";
            cout << "Tempo medio conjunto : " << mediaTempoBusca << " ms\n";
            cout << "Tempo medio operacao : " << mediaTempoBuscaOp << " ms/op\n";
            cout << "Average probes       : " << mediaProbesBusca << "\n";
            cout << "Maximum probes       : " << maxProbesBusca << "\n\n";

            cout << "INSERCAO + BUSCA\n";
            cout << "Tempo medio conjunto : " << mediaTempoCombinado << " ms\n";
            cout << "Tempo medio operacao : " << mediaTempoCombinadoOp << " ms/op\n";
        }
    }
}

int main() {
    srand(42);
    executarSimulacaoCompleta();
    return 0;
}