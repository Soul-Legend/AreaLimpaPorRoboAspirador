// Dupla:
// Matheus Fernandes Bigolin (22200371)
// Pedro Henrique De Sena Trombini Taglialenha (22203674)

#include <array>
#include <cstddef>
#include <fstream>
#include <ios>
#include <iostream>
#include <iterator>
#include <queue>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

bool esta_valido_xml(const std::string xml) {
  // Pilha de identificadores.
  std::stack<std::string> idents{};

  auto inicio{ xml.find("<", 0) };
  // Enquanto existir marcadores de abertura ("<") no XML.
  while (inicio != std::string::npos) {
    // Marcador de fim.
    auto fim{ xml.find(">", inicio + 1) };
    auto prox_inicio{ xml.find("<", inicio + 1) };

    // Não foi encontrado ">" ou ">" está após próximo "<".
    if (fim == std::string::npos || fim > prox_inicio) {
      return false;
    }

    bool esta_fechando{ xml[inicio + 1] == '/' };

    // Obtém o identificador do marcador.
    auto ident{ xml.substr(inicio + 1 + esta_fechando,
                           fim - inicio - 1 - esta_fechando) };

    if (esta_fechando) {
      if (idents.empty() || idents.top() != ident) {
        return false;
      }

      idents.pop();
    } else {
      idents.push(ident);
    }

    inicio = prox_inicio;
  }

  return idents.empty();
}

// Retorna o corpo do próximo marcador dado pelo identificador.
std::string encontra_proximo(const std::string xml, const std::string ident) {
  const auto abertura{ "<" + ident + ">" };
  const auto fechamento{ "</" + ident + ">" };

  auto inicio{ xml.find(abertura, 0) + abertura.size() };
  auto fim{ xml.find(fechamento, inicio) };

  auto corpo{ xml.substr(inicio, fim - inicio) };

  return corpo;
}

template <typename T>
using Matriz = std::vector<std::vector<T>>;

struct Cenario {
  std::string nome;
  Matriz<bool> matriz;
  std::size_t x;
  std::size_t y;
};

// Retorna todos os cenários contidos no XML.
std::vector<Cenario> obter_cenarios(std::string xml) {
  std::vector<Cenario> cenarios{};

  auto inicio{ xml.find("<cenario>", 0) };
  while (inicio != std::string::npos) {
    auto fim{ xml.find("</cenario>", inicio + 1) };
    auto prox_inicio{ xml.find("<cenario>", inicio + 1) };

    auto cenario{ xml.substr(inicio + 9, fim - inicio - 9) };

    auto nome{ encontra_proximo(cenario, "nome") };

    std::size_t x{}, y{};
    std::stringstream(encontra_proximo(cenario, "x")) >> x;
    std::stringstream(encontra_proximo(cenario, "y")) >> y;

    std::size_t altura{}, largura{};
    std::stringstream(encontra_proximo(cenario, "altura")) >> altura;
    std::stringstream(encontra_proximo(cenario, "largura")) >> largura;

    std::stringstream matriz_str(encontra_proximo(cenario, "matriz"));

    Matriz<bool> matriz(altura, std::vector<bool>(largura));

    // Obtém a matriz da string.
    for (std::size_t i{ 0 }; i < altura; ++i) {
      std::string linha{};
      matriz_str >> linha;

      for (std::size_t j{ 0 }; j < largura; ++j) {
        matriz[i][j] = linha[j] == '1';
      }
    }

    cenarios.push_back({ nome, matriz, x, y });

    inicio = prox_inicio;
  }

  return cenarios;
}

// Variações de x e y para obter cada um dos quatro vizinhos de um ponto.
constexpr std::array<std::pair<int, int>, 4> DIRECOES{
  { { 0, 1 }, { 1, 0 }, { -1, 0 }, { 0, -1 } }
};

std::size_t calcular_area_limpa(Matriz<bool> matriz, std::size_t x,
                                std::size_t y) {
  // Fila de pontos (x, y).
  std::queue<std::pair<std::size_t, std::size_t>> fila{};

  std::size_t cont{ 0 };
  // Verifica se o primeiro elemento será limpo na matriz.
  if (matriz[x][y]) {
    fila.push({ x, y });
    matriz[x][y] = false;
    ++cont;
  }

  const auto m{ matriz.size() }, n{ matriz[0].size() };

  while (!fila.empty()) {
    auto [x, y]{ fila.front() };
    fila.pop();

    // Visita todos os vizinhos de (x, y).
    for (auto [dx, dy] : DIRECOES) {
      auto nx{ x + dx };
      auto ny{ y + dy };

      // Verifica se o vizinho está dentro da matriz.
      if ((dx == -1 && x == 0) || nx >= m || (dy == -1 && y == 0) || ny >= n) {
        continue;
      }

      if (matriz[nx][ny]) {
        fila.push({ nx, ny });
        matriz[nx][ny] = false;
        ++cont;
      }
    }
  }

  return cont;
}

// Lê um arquivo e retorna o seu conteúdo.
std::string ler_arquivo(std::string nome_arq) {
  std::ifstream arq(nome_arq, std::ios_base::binary | std::ios_base::in);

  if (!arq.is_open()) {
    throw std::runtime_error("Não foi possível abrir " + nome_arq);
  }

  const std::istreambuf_iterator<char> it{ arq }, fim;
  const std::string conteudo(it, fim);

  if (!arq) {
    throw std::runtime_error("Não foi possível ler " + nome_arq);
  }

  arq.close();
  return conteudo;
}

int main(void) {
  char xmlfilename[100];

  std::cin >> xmlfilename;

  auto xml{ ler_arquivo(xmlfilename) };

  if (!esta_valido_xml(xml)) {
    std::cerr << "erro\n";

    return EXIT_FAILURE;
  }

  auto cenarios{ obter_cenarios(xml) };

  for (auto &cenario : cenarios) {
    auto area{ calcular_area_limpa(cenario.matriz, cenario.x, cenario.y) };

    std::cout << cenario.nome << ' ' << area << '\n';
  }

  return EXIT_SUCCESS;
}
