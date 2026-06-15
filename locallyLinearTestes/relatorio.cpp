#include "relatorio.h"
#include <iostream>
#include <iomanip>

using namespace std;

void imprimirRelatorioMetricas(const MetricasSimulacao& a)
{
    cout << "\n==================================================\n";
    cout << " CONFIGURACAO: n = " << a.n
         << " | alpha = " << a.alpha
         << " | beta = " << a.beta << "\n\n";

    cout << fixed << setprecision(7);

    cout << "1. INSERCAO ATE FATOR DE CARGA (alpha)\n";
    cout << "   - Tempo do conjunto (total ate alpha) (ms)  : "
         << a.tempoInsercaoAteM_Total << " ms\n"; 
    cout << "   - Tempo do conjunto (total ate alpha)       : "
         << a.tempoInsercaoAteM_Total * 1000.0 << " us\n"; 
    cout << "   - Tempo medio por operacao                  : "
         << a.mediaInsercaoAteMOp * 1000.0 << " us/op\n";
    cout << "   - Probes medio                              : "
         << a.mediaProbesInsercaoAteM << "\n";
    cout << "   - Probes maximo                             : "
         << a.maxProbesInsercaoAteM << "\n\n";

    cout << "2. INSERCAO\n";
    cout << "   - Tempo do conjunto                         : "
         << a.tempoInsercaoPosPreenchimento_Total * 1000.0 << " us\n"; 
    cout << "   - Tempo medio por operacao                  : "
         << a.mediaInsercaoPosOp * 1000.0 << " us/op\n";
    cout << "   - Tempo maximo registrado em 1 op (Unit)    : "
         << a.tempoInsercaoUnitario_Max << " us\n"; 
    cout << "   - Probes medio                              : "
         << a.mediaProbesInsercaoPos << "\n";
    cout << "   - Probes maximo                             : "
         << a.maxProbesInsercaoPos << "\n\n";

    cout << "3. BUSCA\n";
    cout << "   - Tempo do conjunto                         : "
         << a.tempoBuscaTotal_Acumulado * 1000.0 << " us\n"; 
    cout << "   - Tempo medio por operacao                  : "
         << a.mediaBuscaTotalOp * 1000.0 << " us/op\n";
    cout << "   - Tempo maximo registrado em 1 op (Unit)    : "
         << a.tempoBuscaUnitario_Max << " us\n"; 
    cout << "   - Probes medio                              : "
         << a.mediaProbesBusca << "\n";
    cout << "   - Probes maximo                             : "
         << a.maxProbesBusca << "\n";

    cout << defaultfloat;
}