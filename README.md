# VmCastle Clipboard Manager 🐧

Um gerenciador de histórico da área de transferência (clipboard manager) com interface gráfica intuitiva, construído com C++ e GTK4. Feito para ArchLinux visando leveza e produtividade (em breve em todas as distros).

![Badge em Desenvolvimento](https://img.shields.io/badge/status-em%20desenvolvimento-brightgreen)

## 🧰 Tecnologias e Ferramentas Utilizadas

- **C++17** — Linguagem principal utilizada no desenvolvimento do aplicativo
- **GTK4** — Toolkit gráfico usado para a interface do usuário
- **GLib** — Biblioteca de utilitários fundamentais
- **xclip** — Ferramenta usada para interagir com o clipboard no ambiente Linux
- **CMake** — Sistema de build utilizado para gerar Makefiles
- **Make** — Utilitário para compilar e gerar os binários do projeto
- **GCC ou Clang** — Compiladores compatíveis com o projeto
- **Shell Scripts** — Scripts de instalação com suporte para múltiplos ambientes gráficos (Hyprland, i3, Sway, KDE, GNOME)

## 📋 Funcionalidades

- ✅ Interface única e intuitiva (sem ícone de bandeja)
- ✅ Armazena histórico de itens copiados (texto e imagens)
- ✅ Atalho global SUPER+V para abrir o gerenciador
- ✅ Tecla ESC para fechar rapidamente
- ✅ Seção "Itens Recentes" para acesso rápido
- ✅ Persistência do histórico entre sessões
- ✅ Execução como serviço em segundo plano
- ✅ Suporte a diversos ambientes desktop (Hyprland, i3, GNOME, KDE, Sway)

## 📦 Dependências

Certifique-se de que os seguintes pacotes estão instalados no seu sistema:

### Arch Linux (ou derivados como Manjaro)

```bash
sudo pacman -S cmake make gtk4 glib2 xclip gcc
```

### Outros sistemas (não testado):

#### Ubuntu/Debian:
```bash
sudo apt install build-essential cmake libgtk-4-dev libglib2.0-dev xclip
```

## 🗂️ Estrutura de pastas

```
ClipboardManager/
├── build/               # Diretório de saída da compilação (gerado pelo CMake)
├── resources/           # Recursos como ícones e arquivos de desktop
├── scripts/             # Scripts de instalação para diferentes ambientes
│   ├── hyprland.sh      # Configuração para Hyprland
│   ├── i3.sh            # Configuração para i3
│   ├── kde.sh           # Configuração para KDE Plasma
│   ├── gnome.sh         # Configuração para GNOME
│   └── sway.sh          # Configuração para Sway
├── src/                 # Código-fonte principal em C++
├── CMakeLists.txt       # Script principal do CMake
├── install.sh           # Script principal de instalação
├── PKGBUILD             # Para instalação via AUR (futuramente)
└── README.md            # Este arquivo :)
```

## 🚀 Como Instalar

### Método 1: Usando o script de instalação (Recomendado)

```bash
git clone https://github.com/VmCastleProject/VmCastle-ClipboardTool.git
cd clipboard-manager
chmod +x install.sh
./install.sh
```

O script detectará automaticamente seu ambiente desktop e configurará o atalho SUPER+V.

### Método 2: Instalação manual

1. Clone o repositório:
```bash
git clone https://github.com/VmCastleProject/VmCastle-ClipboardTool.git
cd clipboard-manager
```

2. Compile o projeto:
```bash
mkdir -p build
cd build
cmake ..
make
```

3. Configure o serviço:
```bash
mkdir -p ~/.config/systemd/user/
cp ../resources/systemd/clipboard-manager.service ~/.config/systemd/user/
systemctl --user daemon-reload
systemctl --user enable --now clipboard-manager.service
```

4. Configure o atalho de teclado para seu ambiente:

#### Para Hyprland:
Adicione ao seu arquivo `~/.config/hypr/configs/Keybinds.conf`:
```
bind = $mainMod, V, exec, sh -c "if pidof clipboard_manager > /dev/null; then pkill -USR1 -f clipboard_manager; else /caminho/para/build/clipboard_manager &; fi"
```

#### Para outros ambientes, consulte os scripts em `scripts/`.

## Se preferir pode rodá-lo via editor de código (vs code, por exemplo):
```bash
mkdir -p build
cd build
cmake ..
make
./clipboard_manager
```

## 💻 Como Usar

1. Pressione **SUPER+V** (tecla Windows + V) para abrir o gerenciador de área de transferência
2. Selecione um item para colá-lo na aplicação ativa
3. Pressione **ESC** para fechar a janela

O histórico da área de transferência é salvo automaticamente em `~/.clipboard_history`.

## 🔧 Solução de Problemas

Se o atalho SUPER+V não estiver funcionando:

1. Verifique se o serviço está em execução:
```bash
systemctl --user status clipboard-manager.service
```

2. Verifique os logs para diagnóstico:
```bash
journalctl --user -u clipboard-manager.service
```

3. Verifique se há conflitos com outros atalhos em seu ambiente desktop

## 📄 Licença

Este projeto está licenciado sob a Licença Pública Geral GNU v3.
Consulte o arquivo LICENSE para mais detalhes.

© 2025 Vinícius (VmCastle)
