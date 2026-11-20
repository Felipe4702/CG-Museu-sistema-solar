# Museu 3D do Sistema Solar

Projeto da disciplina de **Computacao Grafica**, feito em **C com OpenGL classico, GLU e FreeGLUT**.

A ideia e ter um museu onde da para caminhar entre o **Sol, os 8 planetas e a Lua**. Os planetas ficam expostos em pedestais e podem entrar em translacao orbital ao apertar uma tecla. Tambem existe uma camera de cima para acompanhar a animacao.

## O que tem no museu

- **Exposicao:** Sol no centro, planetas alternados a direita e a esquerda, Lua perto da Terra e aneis de Saturno. Os tamanhos e distancias foram ajustados para a visualizacao, nao seguem escala astronomica real.
- **Sala:** piso, quatro paredes, teto, pedestais e placas fixas em 3D com nome e curiosidade dos astros.
- **Texturas:** imagens BMP aplicadas ao Sol, aos oito planetas e a Lua. As imagens ficam em `texturas/`; as esferas voltam para cores simples se alguma imagem estiver faltando.
- **Iluminacao:** duas fontes de luz: uma fonte pontual amarelada no centro do Sol e outra luz auxiliar da sala. O Sol tambem usa emissao para parecer brilhante.
- **Materiais:** objetos com reflexos e brilhos diferentes, configurados com `glMaterialfv` e `glMaterialf`.
- **Sombreamento:** alternancia entre Flat e Smooth Shading.
- **Camera:** visita em primeira pessoa com WASD + mouse, caminhada diagonal e voo opcional; ha ainda a visao superior do Sistema Solar.
- **Projecoes:** perspectiva e ortografica. Na ortografica, a sala se afasta da visualizacao; na superior, a camera mostra as orbitas de cima.
- **Animacao:** rotacao dos astros e translacao dos planetas em orbitas circulares. A Lua acompanha a Terra. Quando a translacao comeca, os pedestais descem para o piso; quando para, retornam.
- **Interacao:** aproximar-se de um astro e apertar E abre um pequeno painel com informacoes.

As orbitas e suas velocidades sao **ilustrativas**, para demonstrar a animacao e as transformacoes do OpenGL.

## Controles

| Tecla | O que faz |
|---|---|
| `W`, `A`, `S`, `D` | Caminhar (tambem na diagonal) |
| Mouse | Olhar em todas as direcoes |
| Setas | Girar a camera sem o mouse |
| `V` | Ativar/desativar voo |
| `Q` / `Z` | Subir / descer no voo |
| `P` | Voltar para perspectiva do visitante |
| `O` | Ativar projecao ortografica |
| `I` | Alternar para a visao superior e retornar |
| `T` | Iniciar/parar translacao orbital e recolher/mostrar pedestais |
| `Espaco` | Pausar/continuar animacoes |
| `E` | Mostrar/fechar informacoes do astro proximo |
| `F` / `G` | Flat / Smooth Shading |
| `L` | Ligar/desligar iluminacao |
| `R` | Restaurar camera inicial |
| `F11` | Alternar entre tela cheia e janela |
| `Esc` | Liberar o cursor |
| Clique esquerdo | Capturar o mouse novamente |

## Como rodar no Windows

Abra a pasta no **VS Code**. E preciso ter GCC e FreeGLUT do **MSYS2 UCRT64** instalados e configurados no PATH. Pode usar normalmente o **PowerShell integrado do VS Code**; nao precisa abrir um terminal MSYS2 separado.

Se o Conda estiver ativo no terminal (`(base)`), desative antes de compilar:

```powershell
conda deactivate
```

Compile a partir da **raiz do repositorio**:

```powershell
gcc main.c camera.c scene.c -o museu.exe -lfreeglut -lopengl32 -lglu32 -lm
```

Execute no mesmo diretorio (isso e importante para encontrar `texturas/`):

```powershell
.\museu.exe
```

O museu abre em tela cheia. Use `F11` para voltar ao modo janela e `Alt+F4` para fechar.

## Estrutura dos arquivos

```text
CG-Museu-sistema-solar/
|-- main.c           # Janela, teclado, mouse, timer e entrada do programa
|-- camera.c         # Posicao/orientacao da camera, movimento e projecoes
|-- camera.h         # Declaracoes das funcoes da camera
|-- scene.c          # Astros, sala, luzes, materiais, placas e animacoes
|-- scene.h          # Declaracoes das funcoes da cena
|-- texturas/        # Dez imagens BMP: Sol, 8 planetas e Lua
|-- README.md
`-- .gitignore
```

## Como funciona por dentro (para ajudar no relatorio)

**Modelagem e transformacoes:** usamos `glutSolidCube()` nos pedestais, `GL_QUADS` nas superficies da sala, `GL_QUAD_STRIP` nos aneis e `gluSphere()` nos astros. `glTranslatef()` posiciona cada objeto, `glRotatef()` gira, e `glScalef()` ajusta seus tamanhos. `glPushMatrix()` e `glPopMatrix()` isolam as transformacoes de cada elemento.

**Camera e projecoes:** `gluLookAt()` define a posicao, o ponto para onde a camera olha e o vetor *up*. `gluPerspective()` configura a perspectiva, enquanto `glOrtho()` configura a ortografica e a visao superior. A caminhada e calculada no plano XZ; o movimento diagonal e normalizado para manter a mesma velocidade.

**Luzes e materiais:** `GL_LIGHT0` esta no centro do Sol (`0, 6.5, 0`), e `GL_LIGHT1` ilumina o ambiente em (`0, 16, 34`). Usamos componentes ambiente, difusa e especular, alem de `GL_EMISSION` no Sol. Materiais diferentes usam valores de brilho especular e `GL_SHININESS`.

**Sombreamento:** as teclas F e G chamam `glShadeModel(GL_FLAT)` e `glShadeModel(GL_SMOOTH)`. No Flat a face tem um sombreamento uniforme; no Smooth, o resultado varia suavemente entre vertices.

**Texturas:** cada arquivo BMP de 24 bits e carregado e enviado ao OpenGL por `glTexImage2D()`. `gluQuadricTexture()` cria as coordenadas para as esferas, e `GL_MODULATE` combina a cor da imagem com a iluminacao. A esfera gira `-90` graus em X para deixar os polos alinhados verticalmente.

**Animacao orbital:** um angulo por planeta e atualizado com o tempo decorrido; seno e cosseno posicionam os planetas em orbitas circulares no plano XZ. A Lua orbita a Terra. A tecla T controla as orbitas e a descida dos pedestais, enquanto Espaco pausa os movimentos dos astros.

**Interacao:** o programa guarda teclas pressionadas, usa callbacks do FreeGLUT para mouse e teclado e atualiza periodicamente com `glutTimerFunc()`. As informacoes do astro aparecem quando o visitante chega perto e aperta E.

## Materiais estudados e uso de IA

O codigo foi desenvolvido a partir do **codigo-base fornecido na disciplina** e dos slides de Computacao Grafica:

- **Aulas 04 e 07:** primitivas, transformacoes, matrizes e funcoes de visualizacao do OpenGL.
- **Aulas 05 e 06:** projecoes e sistema de coordenadas da camera.
- **Aulas 09 e 9b:** iluminacao, luzes e materiais.
- **Aula 12:** teste de profundidade e superficies escondidas.
- **Aula 13:** conceitos de mapeamento de texturas e parametros de textura.

**Uso de inteligencia artificial:** as implementacoes relacionadas a **texturas**, **animacao de translacao orbital** (incluindo o movimento dos pedestais durante a transicao) e **movimentacao do visitante e da camera** foram desenvolvidas com auxilio do ChatGPT. Esses trechos estao identificados com `IA` nos comentarios. Os demais trechos possuem comentarios tecnicos explicando seu funcionamento, sem essa etiqueta. Essa marcacao nao afirma que o restante foi escrito sem ferramentas de IA.

As imagens BMP iniciais sao **texturas ilustrativas geradas para teste**, nao mapas oficiais da NASA. Se forem substituidas por imagens de outro site, e importante registrar a fonte e a licenca no relatorio.

## Para apresentar e entregar

Uma boa demonstracao e: passear pelo museu, ler informacoes com E, comparar F/G e L, mostrar P/O, apertar I para a visao superior e depois T para iniciar as orbitas. Pressione Espaco para pausar e T para retornar aos pedestais.

**Encontro 3 (25/11/2026):** pre-entrega da aplicacao praticamente completa. **Entrega final (02/12/2026):** codigo-fonte e relatorio em PDF. O relatorio deve explicar os objetos, as transformacoes, camera, projecoes, iluminacao, materiais, animacao, dificuldades, participacao da dupla, uso de IA e referencias.
