#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>
#include <random>
#include <algorithm>
#include <iomanip>
#include "locallyLinear.h"
#include "simulacaoNumerosSequenciais.h"
#include "relatorio.h"

using namespace std;

void simulacaoNumerosSequenciais()
{
    cout << "\033[32m=====================Simulacao com Numeros Sequenciais=====================\033[0m" << endl;
    vector<int> tamanhos = {256, 4096, 65536, 1048576, 4194304};
    vector<double> alfas = {0.4, 0.9};

    const int numeroSimulacoes = 100;
    mt19937_64 rng(42);

    for (int n : tamanhos)
    {
        for (double alpha : alfas)
        {
            double loglog_n = log2(log2((double)n));

            int beta = floor(loglog_n / (1.0 - alpha));

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
                vector<uint64_t> chaves(m);
                LocallyLinearHashTable ht(n, beta);

                for (uint64_t i = 1; i < m; i++)
                    chaves[i] = i;

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

                // tirei o "for" pois estava servindo como contador para adicionar novas chaves. A cada chave de chaves, estava adicionando uma nova chave na tabela
                // tanto que o probes máximo baixou drasticamente, estava dando o tamanho da tabela

                uint64_t novaChave = m + 1;

                auto t1_inserir = chrono::high_resolution_clock::now();
                auto r_inserir = htTemp.insert(novaChave);
                auto t2_inserir = chrono::high_resolution_clock::now();

                probesInsercaoPos_Total += r_inserir.probes;
                maxProbesInsercaoPos = max(maxProbesInsercaoPos, r_inserir.probes);

                double tempoOp_inserir = chrono::duration<double, micro>(t2_inserir - t1_inserir).count();
                tempoInsercaoUnitario_Max = max(tempoInsercaoUnitario_Max, tempoOp_inserir);

                auto fimPosPreenchimento = chrono::high_resolution_clock::now();
                tempoInsercaoPosPreenchimento_Total += chrono::duration<double, milli>(fimPosPreenchimento - inicioPosPreenchimento).count();

                // ==========================================
                // 3. BUSCA TOTAL + UNITÁRIA
                // ==========================================
                auto inicioTodasBuscas = chrono::high_resolution_clock::now();

                random_device indiceAleatorioChaves;
                mt19937 gerador(indiceAleatorioChaves());
                uniform_int_distribution<size_t> distribuicao(0, chaves.size() - 1);
                size_t indiceAleatorio = distribuicao(gerador);

                auto t1_busca = chrono::high_resolution_clock::now();
                auto r_busca = ht.search(chaves[indiceAleatorio]);
                auto t2_busca = chrono::high_resolution_clock::now();

                probesBuscaTotal += r_busca.probes;
                maxProbesBusca = max(maxProbesBusca, r_busca.probes);

                double tempoOp_busca = chrono::duration<double, micro>(t2_busca - t1_busca).count();
                tempoBuscaUnitario_Max = max(tempoBuscaUnitario_Max, tempoOp_busca);

                auto fimTodasBuscas = chrono::high_resolution_clock::now();
                tempoBuscaTotal_Acumulado += chrono::duration<double, milli>(fimTodasBuscas - inicioTodasBuscas).count();
            }

               // ==========================================
               // CÁLCULO DAS MÉDIAS
               // ==========================================

               double totalOperacoes = numeroSimulacoes * m;
               
               double totalOperacoesInsercaoAteM = numeroSimulacoes * m;
               double totalOperacoesInsercaoPos = numeroSimulacoes;
               double totalOperacoesBusca = numeroSimulacoes;
               
               double mediaInsercaoAteM = tempoInsercaoAteM_Total / totalOperacoesInsercaoAteM;
               double mediaInsercaoAteMOp = tempoInsercaoAteM_Total / totalOperacoesInsercaoAteM;
               double mediaProbesInsercaoAteM =
               (double)probesInsercaoAteM_Total / totalOperacoesInsercaoAteM;
               
               double mediaInsercaoPosPreenchimento =
               tempoInsercaoPosPreenchimento_Total / numeroSimulacoes;
               
               double mediaInsercaoPosOp =
               tempoInsercaoPosPreenchimento_Total / totalOperacoesInsercaoPos;
               
               double mediaProbesInsercaoPos =
               (double)probesInsercaoPos_Total / totalOperacoesInsercaoPos;
               
               double mediaBuscaTotal =
               tempoBuscaTotal_Acumulado / numeroSimulacoes;
               
               double mediaBuscaTotalOp =
               tempoBuscaTotal_Acumulado / totalOperacoesBusca;
               
               double mediaProbesBusca =
               (double)probesBuscaTotal / totalOperacoesBusca;
               
               MetricasSimulacao dados;
               dados.alpha = alpha;
               dados.n = n;
               dados.beta = beta;
               
               dados.totalOperacoes = totalOperacoes;
               dados.totalOperacoesInsercaoAteM = totalOperacoesInsercaoAteM;
               dados.totalOperacoesInsercaoPos = totalOperacoesInsercaoPos;
               dados.totalOperacoesBusca = totalOperacoesBusca;

               dados.tempoInsercaoAteM_Total = tempoInsercaoAteM_Total;
               dados.mediaInsercaoAteM = mediaInsercaoAteM;
               dados.mediaInsercaoAteMOp = mediaInsercaoAteMOp;
               dados.mediaProbesInsercaoAteM = mediaProbesInsercaoAteM;
               dados.maxProbesInsercaoAteM = maxProbesInsercaoAteM;

               dados.tempoInsercaoPosPreenchimento_Total = tempoInsercaoPosPreenchimento_Total;
               dados.mediaInsercaoPosPreenchimento = mediaInsercaoPosPreenchimento;
               dados.mediaInsercaoPosOp = mediaInsercaoPosOp;
               dados.mediaProbesInsercaoPos = mediaProbesInsercaoPos;
               dados.tempoInsercaoUnitario_Max = tempoInsercaoUnitario_Max;
               dados.maxProbesInsercaoPos = maxProbesInsercaoPos;

               dados.tempoBuscaTotal_Acumulado = tempoBuscaTotal_Acumulado;
               dados.mediaBuscaTotal = mediaBuscaTotal;
               dados.mediaBuscaTotalOp = mediaBuscaTotalOp;
               dados.mediaProbesBusca = mediaProbesBusca;
               dados.tempoBuscaUnitario_Max = tempoBuscaUnitario_Max;
               dados.maxProbesBusca = maxProbesBusca;

               // ==========================================
               // OUTPUT REESTRUTURADO E FORMATAÇÃO DE TEXTO
               // ==========================================
               imprimirRelatorioMetricas(dados);

        }
    }
}