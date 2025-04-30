# VmCastle 🐧

Um gerenciador de histórico da área de transferência (clipboard manager) com interface gráfica intuitiva, construído com C++ e GTKmm. Feito para ArchLinux visando leveza e produtividade (em breve em todas as distros).

![Badge em Desenvolvimento](http://img.shields.io/static/v1?label=STATUS&message=EM%20DESENVOLVIMENTO&color=GREEN&style=for-the-badge)

## Tecnologias Utilizadas

- **C++17** — Linguagem principal do projeto
- **GTKmm 3** — Bindings em C++ para GTK+3 (GUI)
- **xclip** — Utilitário de acesso ao clipboard no Linux
- **CMake** — Sistema de build
- **Make** — Automação da compilação
- **Glibmm** — Utilitários complementares ao GTKmm
- **GCC / Clang** — Compiladores suportados

## Dependências

Certifique-se de ter os seguintes pacotes instalados no seu sistema (ex: Arch Linux):

```bash
sudo pacman -S cmake make gtkmm3 xclip gcc
```

Outros sistemas (não testado):

- Ubuntu/Debian:
  
  ```bash
  sudo apt install build-essential cmake libgtkmm-3.0-dev xclip
  ```

## Estrutura de pastas atual explicada 

```bash
VmCastle/
├── src/
│   ├── clipboard_manager.cpp   # Lógica para monitorar e armazenar a área de transferência
│   ├── clipboard_manager.h     # Cabeçalho da classe de monitoramento
│   ├── gui.cpp                 # Implementação da interface gráfica
│   ├── gui.h                   # Cabeçalho da janela GUI
│   └── main.cpp                # Função principal e inicialização da janela
├── CMakeLists.txt              # Script de build com CMake
├── install.sh                  # Script de instalação e compilação automatizada
└── README.md                   # Documentação do projeto
```

## Como Compilar

```bash
cd VmCastle
cmake -B build
cmake --build build
```

Depois de compilado, execute com:

```bash
./build/vmcastle
```
Se preferir altere o "install.sh" para adequá-lo a sua S.O ou só rode (se usa ArchLinux)


