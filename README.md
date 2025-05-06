# VmCastle 🐧
  
  Um gerenciador de histórico da área de transferência (clipboard manager) com interface gráfica intuitiva, construído com C++ e GTKmm. Feito para ArchLinux visando leveza e produtividade (em breve em todas as distros).
  
  ![Badge em Desenvolvimento](http://img.shields.io/static/v1?label=STATUS&message=EM%20DESENVOLVIMENTO&color=GREEN&style=for-the-badge)

 ## 🧰 Tecnologias e Ferramentas Utilizadas

- **C++17** — Linguagem principal utilizada no desenvolvimento do aplicativo  
- **GTK4** — Toolkit gráfico usado para a interface do usuário  
- **GLib e Glibmm** — Bibliotecas de utilitários fundamentais e suas bindings C++  
- **xclip** — Ferramenta usada para interagir com o clipboard no ambiente Linux  
- **CMake** — Sistema de build utilizado para gerar Makefiles  
- **Make** — Utilitário para compilar e gerar os binários do projeto  
- **GCC ou Clang** — Compiladores compatíveis com o projeto  
- **Shell Scripts** — Scripts de instalação com suporte para múltiplos ambientes gráficos (Hyprland, i3, Sway, KDE, GNOME)

## 📦 Dependências

Certifique-se de que os seguintes pacotes estão instalados no seu sistema:

### Arch Linux (ou derivados como Manjaro)
```bash
sudo pacman -S cmake make gtk4 glib2 xclip gcc
````
  
Outros sistemas (não testado):
  
  - Ubuntu/Debian:
    
```bash
sudo apt install build-essential cmake libgtk-4-dev libglib2.0-dev xclip
```
  
  ## Estrutura de pastas atual explicada 
  
```bash
VmCastle/
├── assets/              # Ícones e imagens usados na interface
├── build/               # Diretório de saída da compilação (gerado pelo CMake)
├── include/             # Headers personalizados do projeto
├── scripts/             # Scripts auxiliares (.desktop, instalação, etc)
├── src/                 # Código-fonte principal em C++
├── CMakeLists.txt       # Script principal do CMake
└── README.md            # Este arquivo :)
```
  
  ## Como Compilar

   ...
  
  Depois de compilado, execute com:

   ...
 
 ## Licença
 
 Este projeto está licenciado sob a Licença Pública Geral GNU v3.  
 Consulte o arquivo [LICENSE](./LICENSE) para mais detalhes.
 
 © 2025 Vinícius (VmCastle)
