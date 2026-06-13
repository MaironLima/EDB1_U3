#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>
#include <random>
#include <algorithm>
#include <iomanip>
#include "locallyLinear.h"

using namespace std;

void executarSimulacaoCompleta()
{
    vector<int> tamanhos = {256, 4096, 65536, 1048576, 4194304};
    vector<double> alfas = {0.4, 0.9};

    const int numeroSimulacoes = 100;
    mt19937_64 rng(42);

    for (int n : tamanhos)
    {
        for (double alpha : alfas)
        {
            double loglog_n = log2(log2((double)n));
            if (loglog_n < 0)
                loglog_n = 0;

            int beta = floor(loglog_n / (1.0 - alpha) + 1.0);
            beta = max(beta, 1);

            int m = static_cast<int>(n * alpha);

            // ==========================================
            // Acumuladores de Tempo e Probes
            // ==========================================
            // Bloco 1: Inserção até M
            double tempoInsercaoAteM_Total = 0;          
            long long probesInsercaoAteM_Total = 0;
            int maxProbesInsercaoAteM = 0;

            // Bloco 2: Inserção + Unitária
            double tempoInsercaoPosPreenchimento_Total = 0; 
            double tempoInsercaoUnitario_Max = 0;   
            long long probesInsercaoPos_Total = 0;
            int maxProbesInsercaoPos = 0;
            
            // Bloco 3: Busca Total + Unitária
            double tempoBuscaTotal_Acumulado = 0;        
            double tempoBuscaUnitario_Max = 0;     
            long long probesBuscaTotal = 0;
            int maxProbesBusca = 0;

            for (int sim = 0; sim < numeroSimulacoes; sim++)
            {
                LocallyLinearHashTable ht(n, beta);
                vector<int> chaves(m);

                for (int i = 0; i < m; i++)
                    chaves[i] = (int)(rng() % 2147483646ULL) + 1;

                // ==========================================
                // 1. INSERÇÃO ATÉ M (Preenchimento Inicial)
                // ==========================================
                auto inicioInsertM = chrono::high_resolution_clock::now();

                for (int k : chaves)
                {
                    auto r = ht.insert(k);
                    probesInsercaoAteM_Total += r.probes;
                    maxProbesInsercaoAteM = max(maxProbesInsercaoAteM, r.probes);
                }

                auto fimInsertM = chrono::high_resolution_clock::now();
                tempoInsercaoAteM_Total += chrono::duration<double, milli>(fimInsertM - inicioInsertM).count();

                // ==========================================
                // 2. INSERÇÃO PÓS-PREENCHIMENTO + UNITÁRIA
                // ==========================================
                LocallyLinearHashTable htTemp = ht; 

                auto inicioPosPreenchimento = chrono::high_resolution_clock::now();

                for (int k : chaves)
                {
                    int novaChave = (int)(rng() % 2147483646ULL) + 1;

                    auto t1 = chrono::high_resolution_clock::now();
                    auto r = htTemp.insert(novaChave);
                    auto t2 = chrono::high_resolution_clock::now();

                    probesInsercaoPos_Total += r.probes;
                    maxProbesInsercaoPos = max(maxProbesInsercaoPos, r.probes);

                    double tempoOp = chrono::duration<double, micro>(t2 - t1).count();
                    tempoInsercaoUnitario_Max = max(tempoInsercaoUnitario_Max, tempoOp);
                }

                auto fimPosPreenchimento = chrono::high_resolution_clock::now();
                tempoInsercaoPosPreenchimento_Total += chrono::duration<double, milli>(fimPosPreenchimento - inicioPosPreenchimento).count();

                // ==========================================
                // 3. BUSCA TOTAL + UNITÁRIA
                // ==========================================
                auto inicioTodasBuscas = chrono::high_resolution_clock::now();

                for (int k : chaves)
                {
                    auto t1 = chrono::high_resolution_clock::now();
                    auto r = ht.search(k);
                    auto t2 = chrono::high_resolution_clock::now();

                    probesBuscaTotal += r.probes;
                    maxProbesBusca = max(maxProbesBusca, r.probes);

                    double tempoOp = chrono::duration<double, micro>(t2 - t1).count();
                    tempoBuscaUnitario_Max = max(tempoBuscaUnitario_Max, tempoOp);
                }

                auto fimTodasBuscas = chrono::high_resolution_clock::now();
                tempoBuscaTotal_Acumulado += chrono::duration<double, milli>(fimTodasBuscas - inicioTodasBuscas).count();
            }

            // ==========================================
            // CÁLCULO DAS MÉDIAS
            // ==========================================
            double totalOperacoes = numeroSimulacoes * m;

            // Bloco 1
            double mediaInsercaoAteM = tempoInsercaoAteM_Total / numeroSimulacoes;
            double mediaInsercaoAteMOp = tempoInsercaoAteM_Total / totalOperacoes;
            double mediaProbesInsercaoAteM = (double)probesInsercaoAteM_Total / totalOperacoes;
            
            // Bloco 2
            double mediaInsercaoPosPreenchimento = tempoInsercaoPosPreenchimento_Total / numeroSimulacoes;
            double mediaInsercaoPosOp = tempoInsercaoPosPreenchimento_Total / totalOperacoes;
            double mediaProbesInsercaoPos = (double)probesInsercaoPos_Total / totalOperacoes;
            
            // Bloco 3
            double mediaBuscaTotal = tempoBuscaTotal_Acumulado / numeroSimulacoes;
            double mediaBuscaTotalOp = tempoBuscaTotal_Acumulado / totalOperacoes;
            double mediaProbesBusca = (double)probesBuscaTotal / totalOperacoes;

            // ==========================================
            // OUTPUT REESTRUTURADO E FORMATAÇÃO DE TEXTO
            // ==========================================
            cout << "\n==================================================\n";
            cout << " CONFIGURACAO: n = " << n << " | alpha = " << alpha << " | beta = " << beta << "\n\n";

            // Evita notação científica (e-05) nos decimais pequenos
            cout << fixed << setprecision(7); 

            cout << "1. INSERCAO ATE FATOR DE CARGA (alpha)\n";
            cout << "   - Tempo medio do conjunto (total ate alpha) : " << mediaInsercaoAteM << " ms\n";
            cout << "   - Tempo medio por operacao                  : " << mediaInsercaoAteMOp << " ms/op\n";
            cout << "   - Probes medio                              : " << mediaProbesInsercaoAteM << "\n";
            cout << "   - Probes maximo                             : " << maxProbesInsercaoAteM << "\n\n";

            cout << "2. INSERCAO\n";
            cout << "   - Tempo medio do conjunto total rodado      : " << mediaInsercaoPosPreenchimento << " ms\n";
            cout << "   - Tempo medio por operacao                  : " << mediaInsercaoPosOp << " ms/op\n";
            cout << "   - Tempo maximo registrado em 1 op (Unit)    : " << (tempoInsercaoUnitario_Max / 1000.0) << " ms\n";
            cout << "   - Probes medio                              : " << mediaProbesInsercaoPos << "\n";
            cout << "   - Probes maximo                             : " << maxProbesInsercaoPos << "\n\n";

            cout << "3. BUSCA\n";
            cout << "   - Tempo medio do conjunto total rodado      : " << mediaBuscaTotal << " ms\n";
            cout << "   - Tempo medio por operacao                  : " << mediaBuscaTotalOp << " ms/op\n";
            cout << "   - Tempo maximo registrado em 1 op (Unit)    : " << (tempoBuscaUnitario_Max / 1000.0) << " ms\n";
            cout << "   - Probes medio                              : " << mediaProbesBusca << "\n";
            cout << "   - Probes maximo                             : " << maxProbesBusca << "\n";
            
            cout << defaultfloat; 
        }
    }
}

int main()
{
    executarSimulacaoCompleta();
    return 0;
}