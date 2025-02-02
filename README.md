# AreaLimpaPorRoboAspirador
Código desenvolvido para um trabalho da disciplina de Estrutura de Dados

Grupo: Pedro Taglialenha, Matheus Bigolin

## Enunciado

Este projeto consiste na utilização de estruturas lineares, vistas até o momento no curso, e aplicação de conceitos de pilha e/ou fila para o processamento de arquivos XML contendo matrizes binárias que representam cenários de ação de um robô aspirador. A implementação deverá resolver dois problemas (listados a seguir), e os resultados deverão ser formatados em saída padrão de tela de modo que possam ser automaticamente avaliados no VPL.

    Entradas:
        cenarios.zip
    Saídas esperadas:
        vpl_evaluate.cases
    Dica:
        utilize "avaliar" (e não "executar") para que as entradas sejam automaticamente carregadas

Materiais

De modo a exemplificar uma entrada para o seu programa, segue o arquivo XML utilizado no primeiro teste:

    cenarios1.xml, cenarios2.xml, cenarios3.xml, cenarios4.xml, cenarios5.xml, cenarios6.xml
        Arquivo compactado com todos os arquivos XML: cenarios.zip
    dicas sobre leitura e escrita com arquivos em C++
        http://www.cplusplus.com/doc/tutorial/files/
    para a criação e concatenação de palavras/caracteres, sugere-se o uso da classe string:
        http://www.cplusplus.com/reference/string/string/

Primeiro problema: validação de arquivo XML

Para esta parte, pede-se exclusivamente a verificação de aninhamento e fechamento das marcações (tags) no arquivo XML (qualquer outra fonte de erro pode ser ignorada). Se houver um erro de aninhamento, deve se impresso erro na tela. Um identificador constitui uma marcação entre os caracteres < e >, podendo ser de abertura (por exemplo: <cenario>) ou de fechamento com uma / antes do identificador (por exemplo: </cenario>). Como apresentando em sala de aula, o algoritmo para resolver este problema é baseado em pilha (LIFO):

    Ao encontrar uma marcação de abertura, empilha o identificador
    Ao encontrar uma marcação de fechamento, verifica se o topo da pilha tem o mesmo identificador e desempilha. Aqui duas situações de erro podem ocorrer:
        Ao consultar o topo, o identificador é diferente (ou seja, uma marcação aberta deveria ter sido fechada antes)
        Ao consultar o topo, a pilha encontra-se vazia (ou seja, uma marcação é fechada sem que tenha sido aberta antes)
    Ao finalizar a análise (parser) do arquivo, é necessário que a pilha esteja vazia. Caso não esteja, mais uma situação de erro ocorre, ou seja, há marcação sem fechamento

Segundo problema: determinação de área do espaço que o robô deve limpar

Cada XML, contém matrizes binárias, com altura e largura, definidas respectivamente pelas marcações <altura> e <largura>, e sequência dos pontos, em modo texto, na marcação <matriz>. Cada ponto corresponde a uma unidade de área, sendo 0 para não pertencente ou 1 para pertencente ao espaço que deve ser limpo, como passo mínimo do robô em uma de quatro direções possíveis (vizinhança-4),  Para cada uma dessas matrizes, pretende-se determinar a área (quantidade de pontos iguais a 1 na região do robô) que deve ser limpa, conforme a posição inicial, linha <x> e coluna <y>, do robô (primeira linha e primeira coluna são iguais a zero). Para isso, seguem algumas definições importantes:

    A vizinhança-4 de um ponto na linha x e coluna y, ou seja, na coordenada (x,y), é um conjunto de pontos adjacentes nas coordenadas:
        (x-1, y)
        (x+1, y)
        (x, y-1)
        (x, y+1)
    Um caminho entre um um ponto p1 e outro pn é em um sequência de pontos distintos <p1,p2,...,pn>, de modo que pi é vizinho-4 de pi+1., sendo i=1,2,...,n-1
    Um ponto p é conexo a um ponto q se existir um caminho de p a q (no contexto deste trabalho, só há interesse em pontos com valor 1, ou seja, pertencentes ao espaço a ser limpo)
    Um componente conexo é um conjunto maximal (não há outro maior que o contenha) C de pontos, no qual quaisquer dois pontos selecionados deste conjunto C são conexos

Para a determinação da área a ser limpa, é necessário identificar quantos pontos iguais a 1 estão na região em que o robô se encontra, ou seja, é preciso determinar a área do componente conexo. Conforme apresentado em aula, segue o algoritmo de reconstrução de componente conexo usando uma fila (FIFO):

    Criar uma matriz R de 0 (zeros) com o mesmo tamanho da matriz de entrada E lida
    Inserir (x,y) na fila
        na coordenada (x,y) da imagem R, atribuir 1
    Enquanto a fila não estiver vazia
        (x,y) ← remover da fila
        inserir na fila as coordenadas dos quatro vizinhos que estejam dentro do domínio da matriz (não pode ter coordenada negativa ou superar o número de linhas ou de colunas), com intensidade 1 (em E) e ainda não tenha sido visitado (igual a 0 em R)
            na coordenada de cada vizinho selecionado, na imagem R, atribuir 1

O conteúdo final da matriz R corresponde ao resultado da reconstrução. A quantidade de 1 (uns) deste único componente conexo é a resposta do segundo problema.

## Lógica do código
O código foi organizado em funções que abordam diferentes aspectos do processamento de arquivos XML e da determinação da área de limpeza do robô.

### VALIDAÇÃO DO ARQUIVO XML
A função esta_valido_xml é responsável por verificar se o arquivo XML está bem formado, ou seja, se as marcações estão aninhadas e fechadas corretamente.
	Uma pilha é utilizada para acompanhar as marcações encontradas no arquivo. Quando uma marcação de abertura é encontrada, seu identificador é empilhado. Quando uma marcação de fechamento é encontrada, o código verifica se o identificador corresponde ao identificador no topo da pilha. Erros são tratados, caso contrário.
	A função percorre o arquivo XML em busca de marcações de abertura e fechamento e retorna verdadeiro se o arquivo estiver bem formado e falso se houver problemas de aninhamento ou fechamento.

### EXTRAÇÃO DE CENÁRIOS
A função obter_cenarios é responsável por extrair informações sobre os cenários do arquivo XML, incluindo seus nomes, dimensões, matrizes e posições iniciais do robô.
	Ela procura por marcações <cenario> no XML, extrai o conteúdo de cada cenário e analisa os valores de altura, largura, x e y, além da matriz que representa o cenário.
	Os dados extraídos são organizados em uma estrutura de dados que facilita o processamento posterior.

### DETERMINAÇÃO DA ÁREA DE LIMPEZA
A função calcular_area_limpa é responsável por determinar a área que o robô deve limpar em um cenário.
	Ela utiliza uma fila (FIFO) para realizar uma busca em largura na matriz, começando pela posição inicial do robô.
	À medida que a busca se expande, os pontos visitados são marcados como já processados na matriz. A função continua até que todos os pontos conectados à posição inicial tenham sido visitados.
	A quantidade de pontos visitados representa a área que o robô deve limpar.

### LEITURA DE ARQUIVO
A função ler_arquivo lida com a leitura do conteúdo do arquivo XML. Ela recebe o nome do arquivo como entrada e retorna o conteúdo em formato de string.

### FLUXO PRINCIPAL 
O fluxo principal do programa está contido na função main. Ele lê o nome do arquivo XML da entrada padrão, lê o conteúdo do arquivo usando a função ler_arquivo e, em seguida, executa a validação com esta_valido_xml.
	Se o arquivo XML for válido, o código extrai informações dos cenários usando obter_cenarios e, para cada cenário, calcula a área de limpeza com calcular_area_limpa. Os resultados são impressos na saída padrão.

## Fluxogramas

### FLUXOGRAMA DA VALIDAÇÃO DO XML
![image](https://github.com/user-attachments/assets/f615e913-4070-4cc1-ab78-39d8a39cc7fa)

### FLUXOGRAMA DA DETERMINAÇÃO DA ÁREA DE LIMPEZA
![image](https://github.com/user-attachments/assets/1e1d055b-c629-4e38-85ac-54aceaf95a43)



