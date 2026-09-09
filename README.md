# Implementa-o-3-escalonamento-CPU

# Escalonador de CPU (Rate Monotonic & EDF)

Simulador de escalonamento de processos em tempo real utilizando os algoritmos Rate Monotonic (RM) e Earliest Deadline First (EDF).

---

## Sistema Operacional
* **Desenvolvido e testado em:** Linux(Ubuntu)

---

## Descrição dos Arquivos .c

* **main.c**: Arquivo principal contendo:
  * Parsing e tratamento do arquivo de entrada (remoção de cabeçalhos, espaços e vírgulas).
  * Validação das restrições de tempo real ($C \le D \le P$).
  * Lógica dos algoritmos de escalonamento Rate Monotonic e EDF.
  * Gerenciamento de preempção, encerramento de tarefas e contagem de deadlines perdidos.

## Como Compilar

Para compilar o programa com o Makefile:
```bash
make
