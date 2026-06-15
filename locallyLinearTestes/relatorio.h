#ifndef RELATORIO_H
#define RELATORIO_H

// Estrutura para agrupar todas as métricas coletadas na simulação
struct MetricasSimulacao {
    // Dados de Configuração
    int n;
    double alpha;
    int beta;

    // Totais de Operações
    double totalOperacoes;
    double totalOperacoesInsercaoAteM;
    double totalOperacoesInsercaoPos;
    double totalOperacoesBusca;

    // Bloco 1: Inserção até M (Fator de carga)
    double tempoInsercaoAteM_Total;
    double mediaInsercaoAteM;
    double mediaInsercaoAteMOp;
    double mediaProbesInsercaoAteM;
    int maxProbesInsercaoAteM;

    // Bloco 2: Inserção pós-preenchimento
    double tempoInsercaoPosPreenchimento_Total;
    double mediaInsercaoPosPreenchimento;
    double mediaInsercaoPosOp;
    double mediaProbesInsercaoPos;
    double tempoInsercaoUnitario_Max;
    int maxProbesInsercaoPos;

    // Bloco 3: Busca
    double tempoBuscaTotal_Acumulado;
    double mediaBuscaTotal;
    double mediaBuscaTotalOp;
    double mediaProbesBusca;
    double tempoBuscaUnitario_Max;
    int maxProbesBusca;
};

// Declaração da função de impressão padronizada
void imprimirRelatorioMetricas(const MetricasSimulacao& metricas);

#endif // RELATORIO_H