#include "ghost.h"
#include <ctime>
#include <cstdlib>
#include <cmath>

#include "interface.h"

__BEGIN_API

/*
    Criador de fantasma
    @param: x e y coordenadas em pixels do 
    local de nacimento do fantasma e local onde será
    reincarnado
*/
Ghost::Ghost(int x, int y, type_ghosts tipo) {
    _x_pixels = x;
    _y_pixels = y;

    _x_maze = y/15.5357;
    _y_maze = x/15.6452;

    _dir = 3; 
    last_mov = _dir;
    

    // salva a posicao de nascimento do fantasma em pixels
    xo_pixel = x;
    yo_pixel = y;

    xo_maze = _x_maze;
    yo_maze = _y_maze;

    if ( tipo == RED){
        lock_inicial = 0;
        _estado = ATACANTE;
    }
    else{
        lock_inicial = 1;
        //_estado = SLEEP;
        _estado = INIT;
    }
    _tipo = tipo;

    debouncerGhost = 0;
};


Ghost::~Ghost(){}

/*
    Reseta os atributos do fantasma
*/
void Ghost::reset()
{
    // Reseta o estado
    _estado = ATACANTE;
    // Pixels
    _x_pixels = xo_pixel;
    _y_pixels = yo_pixel;
    // Maze
    _x_maze = xo_maze;
    _y_maze = yo_maze;

    _dir = 3;
    last_mov = _dir;
    if ( _tipo == RED){
        lock_inicial = 0;
        _estado = ATACANTE;
    }
    else{
        lock_inicial = 1;
        _estado = SLEEP;
    }
}

/*
    Seta o comportamento do fantasma
    @param: entr enum de estado_ghost onded
        1 -> Ataca pacman
        2 -> Foge de pacman
        3 -> Vai até o local de invocação
*/
int Ghost::set_estado_fantasma(const estado_ghost entr)
{
    _estado = entr;
    return 1;
}

Ghost::estado_ghost Ghost::get_estado_fantasma(void)
{
    return _estado;
}

/*
    Implementa a personalidade do fantasma vermelho
    @param: _x_maze_pacman e _y_maze_pacman são a posicao
    do pacman em maze
    @return: Retorna a direção que o fantasma irá se movimentar
    ** Estrátegia **
    - Seguir o pacman, usando o menor caminho da hipotenusa até ele,
    pode ser que de a impressão de encuralar
    - O caso não definido, ele vai para a esquerda;
    - Essa função trata o estado do fantasma, pois define
    a direção do mesmo
    ** Tratamento dos estados **
    - Preciso ir para o caminho mais curto 
    o contrario dessa lógica é o caminho mais longo
*/
int Ghost::logica_ghost_red(int _x_maze_pacman, int _y_maze_pacman)
{
    int delta_x = _x_maze - _x_maze_pacman;
    int delta_y = _y_maze - _y_maze_pacman;

    int i,k = 0,ref_menor_valor;
    int vet[4] = {-1,-1,-1,-1};
    /*
        
    */
    if ( _estado == ATACANTE || _estado == MORTO){
        ref_menor_valor = 20000;
    } else {
        ref_menor_valor = 0;
    }

    /* preciso verificar se a posicão deles é valida
    para não gastar tempo desnecessário com conta
    */
    if (posicao_valida(_x_maze - 1,_y_maze)){
        // Esq
        vet[0] = hipotenusa_estrategica(delta_x-1,delta_y);
    }
    if (posicao_valida(_x_maze+1,_y_maze)){
        // Dir
        vet[1] = hipotenusa_estrategica(delta_x+1,delta_y);
    }
    if (posicao_valida(_x_maze,_y_maze-1)){
        // Down
        vet[2] = hipotenusa_estrategica(delta_x,delta_y-1);
    }
    if (posicao_valida(_x_maze,_y_maze+1)){ 
        // Up
        vet[3] = hipotenusa_estrategica(delta_x,delta_y+1);
    }

    for (i = 0; i < 4; i++){
        //std::cout << "VET [" << i << "] = " << vet[i] << "\n";
        // Escolhe o menor caminho até o alvo
        if ( _estado == ATACANTE || _estado == MORTO){
            if ( (vet[i] < ref_menor_valor) && vet[i] != -1 ){
                ref_menor_valor = vet[i];
                k = i;
            }
        }
        // Escolhe o maior caminho até o alvo
        if ( _estado == FUGITIVO){
            if ( (vet[i] > ref_menor_valor) && vet[i] != -1 ){
                ref_menor_valor = vet[i];
                k = i;
            }
        }
        
    }
    return k;
}

/*
    Calcula a hipotenuza
    @param: Recebe cateto oposto e adjacente
    @return: Retorna o valor da hipotenuza do triângulo
*/
int Ghost::hipotenusa_estrategica(int CO, int CA)
{
    return (int) sqrt((CO*CO + CA*CA));
}

/*
    Atualiza os movimentos do fantasma em maze e pixels
    @param: _x_maze_pacman e _y_maze_pacman 
    são as posições em MAZE do pacman
*/
int Ghost::atualiza_movimento_fantasma(int _x_maze_pacman, int _y_maze_pacman)
{
    _dir = logica_ghost_red(_x_maze_pacman, _y_maze_pacman);
    return mecanica_movimento(_dir);
}


/*
    Trata os movimentos do fantasma
    Atualiza as informações em maze e pixel do fantasma
    @param: novaDirecao -> nova posicao que o fantasma
    vai se deslocar
    @return: 1 -> Se conseguiu realizar o movimento
             0 -> Se não realizou o movimento ( desligado )
    ** Estado de dir para movimentar **
    esq = 0
    dir = 1
    down =2
    up =  3
*/
int Ghost::mecanica_movimento(int novaDirecao)
{
    // tratar problema de avanço proibido
    if ( novaDirecao > 3)
        return 0;

    // Para não sobrescrever a informação duvidosa
    int x_matriz = this->_x_maze;
    int y_matriz = this->_y_maze;
    
    // Atualizar a posicao na matriz
    if ( novaDirecao == 0 ) // esq
        x_matriz--;
    if ( novaDirecao == 1 ) // dir
        x_matriz++;
    if ( novaDirecao == 2 ) // down
        y_matriz--;
    if ( novaDirecao == 3) // up
        y_matriz++;

    // atualiza as informações
    //if (posicao_valida(_x_maze,_y_maze) || _estado == MORTO){
    this->_x_pixels = Interface::convert_matriz_pixel_x(y_matriz);
    this->_y_pixels = Interface::convert_matriz_pixel_y(x_matriz);
    _x_maze = x_matriz;
    _y_maze = y_matriz;
    return 1;
}

/*
    Verifica se a posição no mapa é valido
    @param: i,j localização em maze
    @return: 1 se for válida
             0 se for inválido
*/
int Ghost::posicao_valida(int i, int j)
{
    // Se não saiu do mapa
    if ( i < 0 || j < 0 || i > 31 || j > 28)
        return 0;
    if ( ((int)Interface::maze[i][j] >= 3 && (int)Interface::maze[i][j] <= 8))
        return 1;
    else
        return 0;
}

/*
    Usado para restaurar a posicao
    inicial do fantasma no mapa
*/
void Ghost::volta_posicao_inicial()
{
    _x_pixels = xo_pixel;
    _y_pixels = yo_pixel;

    _dir = 3; 
    last_mov = _dir;
    if ( _tipo == RED)
        _estado = ATACANTE;
    else
        _estado = SLEEP;

    _x_maze = xo_maze;
    _y_maze = yo_maze;
}

/*
    Obtem a direção que o fantasma está indo
    @return: Retorna a direção int seguindo 
    o sistema de direção do fantasma
*/
int Ghost::getDir() const
{
    return _dir;
}

/*
    Função que acha o menor caminho até o respanw
    -> Ela não respeita as paredes
    segue a mesma ideia de logica_ghost_red
*/
int Ghost::movimenta_olhos(void)
{
    int delta_x = _x_maze - xo_maze;
    int delta_y = _y_maze - yo_maze;

    // RED é uma excessão
    if ( _tipo == RED){
        delta_x = _x_maze - 15;
        delta_y = _y_maze - 15;
    }

    int i,k = 0,ref_menor_valor = 20000;
    int vet[4] = {-1,-1,-1,-1};


    vet[0] = hipotenusa_estrategica(delta_x-1,delta_y);
    vet[1] = hipotenusa_estrategica(delta_x+1,delta_y);
    vet[2] = hipotenusa_estrategica(delta_x,delta_y-1);
    vet[3] = hipotenusa_estrategica(delta_x,delta_y+1);

    for (i = 0; i < 4; i++){
        if ( _estado == MORTO){
            if ( (vet[i] < ref_menor_valor) && vet[i] != -1 ){
                ref_menor_valor = vet[i];
                k = i;
            }
        }
    }
    return k;
}

/*
    Movimenta o fantasma gradualmente até a
    área de invocação ( respanw )
    @return: 1 -> chegou ao local de respanw
             0 -> ainda não chegou
    ** USO **
    Deve ser usada como iterada

    ** Consequencias **
    Quando o fantasma chegar no local de respanw,
    vai mudar seu estado para atacante novamente.

    -> Só pode ser usada se o seu estado estiver em 
    MORTO, ele não deve ser alterado pela Window
*/
int Ghost::retorno_gradual_ao_respanw(void)
{
    if ( _estado != MORTO)
        return 1;

    mecanica_movimento(movimenta_olhos());
    
    // Veficia se chegou no local desejado
    if ( xo_maze == _x_maze && yo_maze == _y_maze){
        _estado = WAITING;
        return 1;
    }

    // ainda não chegou no local desejado
    return 0;
}

int Ghost::get_x_pixel(void){ return _x_pixels; }
int Ghost::get_y_pixel(void){ return _y_pixels; }
int Ghost::get_x_maze(void) { return _x_maze; }
int Ghost::get_y_maze(void) { return _y_maze; }

/*
    Função para fazer o movimento inicial
    -> Precisa ser iterada
    @return: 1 -> Quando finalizou os passos iniciais
             0 -> Ainda não finalizou

    ** Estrátegia **
    Coloquei o objetivo sair da caixa, assim que sair,
    muda o seu estado de SLEEP para atacante
*/
int Ghost::movimentacao_inicial(void)
{
    if (_estado != SLEEP)
        return 1;

    int retorno_valido, my_dir;
    my_dir = logica_sair_da_cadeia();
    //std::cout << "DIR -> " << my_dir << "\n";
    retorno_valido = mecanica_movimento(my_dir);

    // Se já saiu da caixa, lock_inical já está livre
    if ( my_dir == 10){
        lock_inicial = 0;
        _estado = ATACANTE;
        return 1;
    } else 
        return 0;
}

/*
    Trata movimentos iniciais, com objetivo fixo,
    sair da cadeia
    @return: 10 -> quando saiu da cadeia
    -> Outros retornos são posições
*/
int Ghost::logica_sair_da_cadeia(void)
{
    int delta_x = _x_maze - 11;
    int delta_y = _y_maze - 15;


    // Se chegou no alvo, não tem pq decidir mais
    if ( _x_maze == 11) return 10;

    int i,k = 0,ref_menor_valor = 20000;
    int vet[4] = {-1,-1,-1,-1};


    if (posicao_valida_cadeia(_x_maze - 1,_y_maze)){
        // Esq
        vet[0] = hipotenusa_estrategica(delta_x-1,delta_y);
    }
    if (posicao_valida_cadeia(_x_maze+1,_y_maze)){
        // Dir
        vet[1] = hipotenusa_estrategica(delta_x+1,delta_y);
    }
    if (posicao_valida_cadeia(_x_maze,_y_maze-1)){
        // Down
        vet[2] = hipotenusa_estrategica(delta_x,delta_y-1);
    }
    if (posicao_valida_cadeia(_x_maze,_y_maze+1)){ 
        // Up
        vet[3] = hipotenusa_estrategica(delta_x,delta_y+1);
    }
    for (i = 0; i < 4; i++){
        if ( _estado == MORTO){
            if ( (vet[i] < ref_menor_valor) && vet[i] != -1 ){
                ref_menor_valor = vet[i];
                k = i;
            }
        }
    }
    return k;
}

int Ghost::posicao_valida_cadeia(int i, int j)
{
    if ( ((int)Interface::maze[i][j] >= 1 && (int)Interface::maze[i][j] <= 8))
        return 1;
    else
        return 0; 
}

/*
 * Movimenta os fantasmas apenas dentro da cadeia
 */
void Ghost::movimenta_cadeia()
{
    if(dentro_cadeia(_x_maze+1,_y_maze)) mecanica_movimento(1);
    else if(dentro_cadeia(_x_maze-1,_y_maze)) mecanica_movimento(0);
    else if(dentro_cadeia(_x_maze,_y_maze+1)) mecanica_movimento(3);
    else if(dentro_cadeia(_x_maze,_y_maze-1)) mecanica_movimento(2);
}

/*
 * Verifica se a posição está dentro da cadeia
 * @return: 1 -> está dentro
 *          0 -> não está
 * cadeia: [13][11] ---- [13][16]
 *             |             |
 *             |             |
 *         [15][11] ---- [15][16]
 */
int Ghost::dentro_cadeia(int i, int j)
{
    if( (i > 12 && i < 16) && (j > 10 && j < 17) ) return 1;
    else return 0;
}

Ghost::type_ghosts Ghost::get_type_ghosts(void)
{
    return _tipo;
}

void Ghost::set_debouncerGhost(int d)
{
    debouncerGhost = d;
}

int Ghost::get_debouncerGhost() const
{
    return debouncerGhost;
}

__END_API
