# Museu 3D do Sistema Solar

Projeto desenvolvido para a disciplina de **Computação Gráfica**, com o objetivo de criar um museu tridimensional interativo sobre o Sistema Solar, utilizando **C**, **OpenGL clássico** e **FreeGLUT**.

O visitante pode caminhar pela sala, observar o Sol e os planetas e alternar entre as projeções perspectiva e ortográfica.

> **Uso de inteligência artificial:** todos os comentários do código-fonte foram gerados por inteligência artificial (ChatGPT).

## Funcionalidades implementadas — Encontro 2

- Ambiente 3D com piso, paredes e teto.
- Sol centralizado, oito planetas e Lua, representados por esferas.
- Planetas distribuídos alternadamente à direita e à esquerda do Sol, seguindo a ordem de afastamento do Sistema Solar.
- Tamanhos e distâncias adaptados à exposição, sem compromisso com a escala astronômica real.
- Transformações geométricas de translação, rotação e escala (`glTranslatef`, `glRotatef` e `glScalef`).
- Rotação dos astros em torno dos próprios eixos, com opção de pausar e retomar.
- Câmera em primeira pessoa, com movimentação contínua por teclado e orientação pelo mouse.
- Modo de voo opcional.
- Projeções perspectiva e ortográfica, com ajuste da visualização da sala no modo ortográfico.

**A translação orbital dos planetas ao redor do Sol ainda não foi implementada.** A transformação de translação usada nesta etapa serve para posicionar os objetos na cena.

## Tecnologias

- Linguagem **C**
- **OpenGL** (pipeline fixo)
- **GLU** e **FreeGLUT**
- Compilador **GCC**
- **Visual Studio Code** no Windows

## Estrutura do projeto

```text
CG-Museu-sistema-solar/
├── main.c       # Janela, teclado, mouse e atualização do programa
├── camera.c     # Posição, movimento, orientação e projeções da camera
├── camera.h     # Declarações relacionadas a camera
├── scene.c      # Ambiente, astros, transformacoes e animação
├── scene.h      # Declarações relacionadas a cena
├── .gitignore
└── README.md
```

## Como compilar e executar

O projeto foi preparado para **Windows**, usando o terminal **PowerShell integrado ao VS Code**, com GCC e FreeGLUT instalados no ambiente **MSYS2 UCRT64**.

> **Importante:** desative o ambiente Conda antes de compilar, pois sua ativacao pode interferir no GCC e nas bibliotecas do MSYS2.

No terminal, dentro da pasta do repositorio:

```powershell
conda deactivate
```

Se o Conda não estiver ativo, esse passo não é necessário.

Compile os arquivos:

```powershell
gcc main.c camera.c scene.c -o museu.exe -lfreeglut -lopengl32 -lglu32 -lm
```

Execute:

```powershell
.\museu.exe
```

O arquivo `museu.exe` é gerado localmente e não é enviado ao GitHub, conforme o `.gitignore`.

## Controles

| Tecla / comando | Ação |
|---|---|
| `W` / `S` | Caminhar para frente / trás |
| `A` / `D` | Caminhar para esquerda / direita |
| `W` + `A` ou `D` | Caminhar na diagonal |
| Mouse | Mover a direção do olhar |
| Setas | Alternativa para orientar a camera |
| `V` | Ativar / desativar o modo de voo |
| `Q` / `Z` | Subir / descer durante o voo |
| `P` | Projeção perspectiva |
| `O` | Projeção ortografica |
| `Espaço` | Pausar / retomar a rotacao dos astros |
| `R` | Restaurar a camera inicial |
| `Esc` | Liberar o cursor do mouse |
| Clique esquerdo | Capturar o mouse novamente |

## Próximas etapas

- Implementar a translação orbital dos planetas ao redor do Sol.
- Exibir informações sobre um planeta quando o visitante se aproximar e interagir.
- Adicionar texturas e aprimorar a aparência do Sol, dos planetas e da Lua.
- Aperfeiçoar a iluminação e o acabamento da exposição.

## Observações

Este projeto utiliza como ponto de partida os arquivos e os conceitos apresentados nas aulas de Computação Gráfica. A implementação prioriza comandos básicos do OpenGL, organização simples e código comentado.
