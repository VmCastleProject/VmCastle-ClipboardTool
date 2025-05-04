# VmCastle 🐧
 
 Um gerenciador de histórico da área de transferência (clipboard manager) com interface gráfica intuitiva, construído com C++ e GTK 4. Feito para ArchLinux visando leveza e produtividade (em breve em todas as distros).
 
 ![Badge em Desenvolvimento](http://img.shields.io/static/v1?label=STATUS&message=EM%20DESENVOLVIMENTO&color=GREEN&style=for-the-badge)
 
 ## Tecnologias Utilizadas
 
 - **C++17** — Linguagem principal do projeto
 - **GTK 4** — Bindings em C++ para GTK+3 (GUI)
 - **xclip** — Utilitário de acesso ao clipboard no Linux
 - **CMake** — Sistema de build
 - **Make** — Automação da compilação
 - **Glibmm** — Utilitários complementares ao GTKmm
 - **GCC / Clang** — Compiladores suportados
 
 ## Dependências
 
 Certifique-se de ter os seguintes pacotes instalados no seu sistema (ex: Arch Linux):
 
 ```bash
 sudo pacman -S cmake gcc gtk4 xclip pkg-config gnome-desktop
 ```
 
 ## Estrutura de pastas atual explicada 
 
...
 
 ## Como Compilar e rodar!
 
 ```bash
mkdir -p build
cd build
cmake ..
make
./clipboard_manager
 ```