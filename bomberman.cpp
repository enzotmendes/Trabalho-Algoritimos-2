/*
 * BOMBERMAN - Trabalho M1
 * Algoritmos e Programacao II (22817) - UNIVALI
 * Enzo Tortelli Mendes, Lucas Machado e Teylor Sagaz
 */

#include <iostream>
#include <windows.h>
#include <conio.h>
#include <cstdlib>
#include <ctime>

using namespace std;

const int LINHAS = 15;
const int COLUNAS = 13;

const int CAMINHO    = 0;
const int PAREDE_S   = 1;
const int PAREDE_F   = 2;
const int BOMBA_CELL = 3;
const int EXPLOSAO   = 4;

const int TIMER_EXPLOSAO = 80;
const int TIMER_LIMPAR   = 130;
const int VELOCIDADE_INIMIGO = 50;
const int NUM_INIMIGOS = 3;

// Cores do console
const int COR_PADRAO   = 7;
const int COR_JOGADOR  = 10;
const int COR_INIMIGO  = 12;
const int COR_PAREDE_S = 8;
const int COR_PAREDE_F = 6;
const int COR_BOMBA    = 15;
const int COR_EXPLOSAO = 14;
const int COR_TEXTO    = 11;

struct Inimigo {
    int x;
    int y;
    bool vivo;
};

void reposicionarCursor() {
    COORD coord;
    coord.X = 0;
    coord.Y = 0;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void ocultarCursor() {
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(out, &cursorInfo);
}

void setCor(int cor) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), cor);
}

void desenhaMapa(int m[LINHAS][COLUNAS], int x, int y,
                 Inimigo inimigos[], int vidas) {

    int inimigosVivos = 0;
    for (int k = 0; k < NUM_INIMIGOS; k++) {
        if (inimigos[k].vivo) inimigosVivos++;
    }

    setCor(COR_TEXTO);
    cout << " VIDAS: " << vidas
         << "   INIMIGOS: " << inimigosVivos
         << "          \n";

    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {

            if (i == x && j == y) {
                setCor(COR_JOGADOR);
                cout << char(254) << " ";
            }
            else {
                bool inimigoAqui = false;
                for (int k = 0; k < NUM_INIMIGOS; k++) {
                    if (inimigos[k].vivo &&
                        inimigos[k].x == i &&
                        inimigos[k].y == j) {
                        setCor(COR_INIMIGO);
                        cout << "* ";
                        inimigoAqui = true;
                        break;
                    }
                }

                if (!inimigoAqui) {
                    switch (m[i][j]) {
                        case CAMINHO:
                            cout << "  ";
                            break;
                        case PAREDE_S:
                            setCor(COR_PAREDE_S);
                            cout << char(219) << char(219);
                            break;
                        case PAREDE_F:
                            setCor(COR_PAREDE_F);
                            cout << char(178) << char(178);
                            break;
                        case BOMBA_CELL:
                            setCor(COR_BOMBA);
                            cout << char(232) << " ";
                            break;
                        case EXPLOSAO:
                            setCor(COR_EXPLOSAO);
                            cout << char(177) << char(177);
                            break;
                    }
                }
            }
        }
        cout << "\n";
    }
    setCor(COR_PADRAO);
}

void moveJogador(int m[LINHAS][COLUNAS], int &x, int &y, char tecla) {
    int nx = x, ny = y;

    switch (tecla) {
        case 72: case 'w': case 'W': nx--; break;
        case 80: case 's': case 'S': nx++; break;
        case 75: case 'a': case 'A': ny--; break;
        case 77: case 'd': case 'D': ny++; break;
    }

    if (nx < 0 || nx >= LINHAS || ny < 0 || ny >= COLUNAS) return;

    if (m[nx][ny] == CAMINHO || m[nx][ny] == BOMBA_CELL) {
        x = nx;
        y = ny;
    }
}

void colocarBomba(int m[LINHAS][COLUNAS], bool &bAtiva,
                  int &bX, int &bY, int &timer, int x, int y) {
    if (!bAtiva) {
        bAtiva = true;
        bX = x;
        bY = y;
        m[bX][bY] = BOMBA_CELL;
        timer = 0;
    }
}

void gerenciarBomba(int m[LINHAS][COLUNAS], bool &bAtiva,
                    int &bX, int &bY, int &timer) {
    if (!bAtiva) return;

    timer++;

    if (timer == TIMER_EXPLOSAO) {
        m[bX][bY] = EXPLOSAO;

        // cima
        if (m[bX-1][bY] == PAREDE_S) { }
        else if (m[bX-1][bY] == PAREDE_F) m[bX-1][bY] = CAMINHO;
        else m[bX-1][bY] = EXPLOSAO;

        // baixo
        if (m[bX+1][bY] == PAREDE_S) { }
        else if (m[bX+1][bY] == PAREDE_F) m[bX+1][bY] = CAMINHO;
        else m[bX+1][bY] = EXPLOSAO;

        // esquerda
        if (m[bX][bY-1] == PAREDE_S) { }
        else if (m[bX][bY-1] == PAREDE_F) m[bX][bY-1] = CAMINHO;
        else m[bX][bY-1] = EXPLOSAO;

        // direita
        if (m[bX][bY+1] == PAREDE_S) { }
        else if (m[bX][bY+1] == PAREDE_F) m[bX][bY+1] = CAMINHO;
        else m[bX][bY+1] = EXPLOSAO;
    }

    if (timer == TIMER_LIMPAR) {
        m[bX][bY] = CAMINHO;
        if (m[bX-1][bY] == EXPLOSAO) m[bX-1][bY] = CAMINHO;
        if (m[bX+1][bY] == EXPLOSAO) m[bX+1][bY] = CAMINHO;
        if (m[bX][bY-1] == EXPLOSAO) m[bX][bY-1] = CAMINHO;
        if (m[bX][bY+1] == EXPLOSAO) m[bX][bY+1] = CAMINHO;
        bAtiva = false;
        timer  = 0;
    }
}

void moveInimigo(int m[LINHAS][COLUNAS], Inimigo &ini) {
    if (!ini.vivo) return;

    int direcao = rand() % 4;
    int passos  = (rand() % 3) + 1;

    for (int p = 0; p < passos; p++) {
        int nx = ini.x, ny = ini.y;

        if      (direcao == 0) nx--;
        else if (direcao == 1) nx++;
        else if (direcao == 2) ny--;
        else if (direcao == 3) ny++;

        if (m[nx][ny] == CAMINHO) {
            ini.x = nx;
            ini.y = ny;
        } else {
            break;
        }
    }
}

void verificarMorteInimigos(int m[LINHAS][COLUNAS], Inimigo inimigos[]) {
    for (int k = 0; k < NUM_INIMIGOS; k++) {
        if (inimigos[k].vivo && m[inimigos[k].x][inimigos[k].y] == EXPLOSAO) {
            inimigos[k].vivo = false;
        }
    }
}

bool todosInimigosMortos(Inimigo inimigos[]) {
    for (int k = 0; k < NUM_INIMIGOS; k++) {
        if (inimigos[k].vivo) return false;
    }
    return true;
}

bool algumInimigoNaPosicao(Inimigo inimigos[], int x, int y) {
    for (int k = 0; k < NUM_INIMIGOS; k++) {
        if (inimigos[k].vivo && inimigos[k].x == x && inimigos[k].y == y) {
            return true;
        }
    }
    return false;
}

void respawnJogador(int m[LINHAS][COLUNAS], int &x, int &y,
                    bool &bombaAtiva, int &bombaX, int &bombaY, int &timerBomba) {
    x = 1;
    y = 1;
    if (bombaAtiva) {
        m[bombaX][bombaY] = CAMINHO;
        if (m[bombaX-1][bombaY] == EXPLOSAO) m[bombaX-1][bombaY] = CAMINHO;
        if (m[bombaX+1][bombaY] == EXPLOSAO) m[bombaX+1][bombaY] = CAMINHO;
        if (m[bombaX][bombaY-1] == EXPLOSAO) m[bombaX][bombaY-1] = CAMINHO;
        if (m[bombaX][bombaY+1] == EXPLOSAO) m[bombaX][bombaY+1] = CAMINHO;
        bombaAtiva = false;
        timerBomba = 0;
    }
}

int main() {

    SetConsoleOutputCP(437);
    SetConsoleCP(437);
    ocultarCursor();
    srand((unsigned)time(NULL));

    int m[LINHAS][COLUNAS] = {
        1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,0,0,0,2,0,2,0,2,0,2,0,1,
        1,0,1,2,1,2,1,2,1,2,1,2,1,
        1,0,2,0,2,0,2,0,2,0,2,0,1,
        1,2,1,2,1,2,1,2,1,2,1,2,1,
        1,0,2,0,0,0,0,0,2,0,2,0,1,
        1,2,1,2,1,2,1,2,1,2,1,2,1,
        1,0,2,0,2,0,0,0,2,0,2,0,1,
        1,2,1,2,1,2,1,2,1,2,1,2,1,
        1,0,2,0,2,0,2,0,2,0,2,0,1,
        1,2,1,2,1,2,1,2,1,2,1,2,1,
        1,0,2,0,2,0,2,0,2,0,2,0,1,
        1,2,1,2,1,2,1,2,1,2,1,2,1,
        1,0,2,0,2,0,2,0,2,0,2,0,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1
    };

    int x = 1, y = 1;
    int vidas = 3;

    bool bombaAtiva = false;
    int  bombaX     = 0;
    int  bombaY     = 0;
    int  timerBomba = 0;

    Inimigo inimigos[NUM_INIMIGOS];
    inimigos[0] = {9,  9,  true};
    inimigos[1] = {11, 5,  true};
    inimigos[2] = {3,  9,  true};

    int timerInimigo[NUM_INIMIGOS] = {0, 33, 66};

    char tecla = 0;

    while (true) {

        if (_kbhit()) {
            tecla = getch();

            if (tecla == 0 || tecla == (char)224) {
                tecla = getch();
            }

            moveJogador(m, x, y, tecla);

            if (tecla == ' ') {
                colocarBomba(m, bombaAtiva, bombaX, bombaY, timerBomba, x, y);
            }

            tecla = 0;
        }

        gerenciarBomba(m, bombaAtiva, bombaX, bombaY, timerBomba);

        for (int k = 0; k < NUM_INIMIGOS; k++) {
            timerInimigo[k]++;
            if (timerInimigo[k] > VELOCIDADE_INIMIGO && inimigos[k].vivo) {
                moveInimigo(m, inimigos[k]);
                timerInimigo[k] = 0;
            }
        }

        verificarMorteInimigos(m, inimigos);

        reposicionarCursor();
        desenhaMapa(m, x, y, inimigos, vidas);

        if (todosInimigosMortos(inimigos)) {
            Sleep(500);
            system("cls");
            setCor(COR_EXPLOSAO);
            cout << "\n\n";
            cout << "  ************************************\n";
            cout << "  *                                  *\n";
            cout << "  *   VITORIA! Voce eliminou todos   *\n";
            cout << "  *        os inimigos!              *\n";
            cout << "  *                                  *\n";
            cout << "  ************************************\n";
            setCor(COR_PADRAO);
            break;
        }

        bool jogadorDanificado = false;

        if (algumInimigoNaPosicao(inimigos, x, y)) {
            jogadorDanificado = true;
        }

        if (m[x][y] == EXPLOSAO) {
            jogadorDanificado = true;
        }

        if (jogadorDanificado) {
            Sleep(600);
            vidas--;

            if (vidas <= 0) {
                system("cls");
                setCor(COR_INIMIGO);
                cout << "\n\n";
                cout << "  ************************************\n";
                cout << "  *                                  *\n";
                cout << "  *   GAME OVER! Voce foi derrotado. *\n";
                cout << "  *                                  *\n";
                cout << "  ************************************\n";
                setCor(COR_PADRAO);
                break;
            } else {
                respawnJogador(m, x, y, bombaAtiva, bombaX, bombaY, timerBomba);
            }
        }

        Sleep(1);
    }

    cout << "\n  Pressione qualquer tecla para sair...";
    getch();
    return 0;
}
