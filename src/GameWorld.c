/**
 * @file GameWorld.c
 * @author Prof. Dr. David Buzatto
 * @brief Implementação do GameWorld.
 *
 * @copyright Copyright (c) 2026
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "GameWorld.h"
#include "Jogador.h"
#include "Macros.h"
#include "Mapa.h"
#include "Obstaculo.h"
#include "Tipos.h"
#include "ResourceManager.h"

#include "raylib/raylib.h"

static void desenharFundo( GameWorld *gw );
static void atualizarCamera( GameWorld *gw );

static void inicializar( GameWorld *gw );
static void reiniciar( GameWorld *gw );

static void carregarFase( GameWorld *gw, int fase );
static void atualizarMusicaFase( GameWorld *gw );
static void verificarTransicaoFase( GameWorld *gw );
static Texture2D *getTexturaFundoAtual( GameWorld *gw );

static void desenharMensagemFase( GameWorld *gw );
static void desenharTelaMenu( GameWorld *gw );
static void desenharTelaContinue( GameWorld *gw );
static void desenharTelaTransicaoFase( GameWorld *gw );
static void desenharFundoTela( GameWorld *gw, Color tonalidade );
static void desenharPlacaChegada( GameWorld *gw );
static void iniciarTransicaoFase( GameWorld *gw, int proximaFase );
static void continuarJogo( GameWorld *gw );
static float calcularYSpawnJogador( Mapa *mapa, float xSpawn );
static float calcularYChaoPrincipal( Mapa *mapa );

/**
 * @brief Cria uma instância alocada dinamicamente da struct GameWorld.
 */
GameWorld *createGameWorld( void ) {

    GameWorld *gw = (GameWorld*) malloc( sizeof( GameWorld ) );
    inicializar( gw );

    return gw;

}

/**
 * @brief Destrói um objeto GameWorld e suas dependências.
 */
void destroyGameWorld( GameWorld *gw ) {

    if ( gw != NULL ) {

        if ( gw->mapa != NULL ) {
            destruirMapa( gw->mapa );
        }

        if ( gw->jogador != NULL ) {
            destruirJogador( gw->jogador );
        }

        free( gw );

    }

}

/**
 * @brief Lê a entrada do usuário e atualiza o estado do jogo.
 */
void updateGameWorld( GameWorld *gw, float delta ) {

    atualizarMusicaFase( gw );

    gw->tempoTela += delta;

    if ( IsKeyPressed( KEY_R ) ) {
        reiniciar( gw );
        return;
    }

    if ( gw->estado == ESTADO_GAMEWORLD_MENU ) {

        if ( IsKeyPressed( KEY_LEFT ) || IsKeyPressed( KEY_A ) ) {
            gw->faseSelecionada--;
            if ( gw->faseSelecionada < 1 ) gw->faseSelecionada = 3;
        }

        if ( IsKeyPressed( KEY_RIGHT ) || IsKeyPressed( KEY_D ) ) {
            gw->faseSelecionada++;
            if ( gw->faseSelecionada > 3 ) gw->faseSelecionada = 1;
        }

        if ( IsKeyPressed( KEY_ONE ) ) gw->faseSelecionada = 1;
        if ( IsKeyPressed( KEY_TWO ) ) gw->faseSelecionada = 2;
        if ( IsKeyPressed( KEY_THREE ) ) gw->faseSelecionada = 3;

        if ( IsKeyPressed( KEY_ENTER ) || IsKeyPressed( KEY_SPACE ) ) {
            if ( gw->faseAtual != gw->faseSelecionada ) {
                carregarFase( gw, gw->faseSelecionada );
            }
            gw->estado = ESTADO_GAMEWORLD_JOGANDO;
            gw->tempoTela = 0.0f;
            gw->mostrandoMensagemFase = true;
            gw->tempoMensagemFase = 1.65f;
        }

        return;

    }

    if ( gw->estado == ESTADO_GAMEWORLD_CONTINUE ) {

        gw->tempoContinue -= delta;

        if ( IsKeyPressed( KEY_ENTER ) || IsKeyPressed( KEY_SPACE ) ) {
            continuarJogo( gw );
            return;
        }

        if ( gw->tempoContinue <= 0.0f ) {
            reiniciar( gw );
            return;
        }

        return;

    }

    if ( gw->estado == ESTADO_GAMEWORLD_TRANSICAO_FASE ) {

        gw->duracaoTransicao -= delta;

        if ( gw->duracaoTransicao <= 0.0f || IsKeyPressed( KEY_ENTER ) || IsKeyPressed( KEY_SPACE ) ) {
            gw->jogador->pontuacao = gw->pontuacaoAntesTransicao + gw->bonusTotal;
            if ( gw->proximaFase == 0 ) {
                carregarFase( gw, 1 );
                gw->faseSelecionada = 1;
                gw->estado = ESTADO_GAMEWORLD_MENU;
            } else {
                carregarFase( gw, gw->proximaFase );
                gw->estado = ESTADO_GAMEWORLD_JOGANDO;
            }
            gw->tempoTela = 0.0f;
        }

        return;

    }

    gw->tempoFase += delta;

    if ( gw->mostrandoMensagemFase ) {

        gw->tempoMensagemFase -= delta;

        if ( gw->tempoMensagemFase <= 0.0f ) {
            gw->tempoMensagemFase = 0.0f;
            gw->mostrandoMensagemFase = false;
        }

    }

    Jogador *j = gw->jogador;

    atualizarMapa( gw->mapa, gw, delta );
    entradaJogador( j, delta );
    atualizarJogador( j, gw, delta );

    if ( j->quantidadeVidas < 0 ) {
        gw->estado = ESTADO_GAMEWORLD_CONTINUE;
        gw->tempoContinue = 10.0f;
        gw->tempoTela = 0.0f;
        return;
    }

    verificarTransicaoFase( gw );
    atualizarCamera( gw );

}

// desenha os numeros com escala
static void desenharNumerosCustomScaled(
    const char* buffer,
    float xStart,
    float yStart,
    int maxDigitos,
    Color corTexto,
    float scaleFactor
) {

    int numY = 3;
    int numW = 18;
    int numH = 36;
    int mapeamentoX[] = { 106, 144, 175, 212, 245, 280, 315, 350, 385, 420 };

    float scaledW = (float)numW * scaleFactor;
    float scaledH = (float)numH * scaleFactor;

    for ( int i = 0; i < maxDigitos; i++ ) {

        int digito = buffer[i] - '0';

        if ( digito < 0 || digito > 9 ) continue;

        int xCortar = mapeamentoX[digito];

        Rectangle sourceRect = {
            (float)xCortar,
            (float)numY,
            (float)numW,
            (float)numH
        };

        Rectangle destRect = {
            xStart + (i * scaledW),
            yStart,
            scaledW,
            scaledH
        };

        DrawTexturePro(
            rm.texturaHUD,
            sourceRect,
            destRect,
            (Vector2){ 0 },
            0.0f,
            corTexto
        );

    }

}

// desenha o hud inteiro
static void desenharHUD( GameWorld *gw ) {
    
    float mainHudScale = 0.9f;
    float livesHudScale = 0.5f;

    Vector2 posBase = { 16.0f, 16.0f };
    float labelHSpacing = 28.0f;
    float labelWidthScale = 0.8f;

    Rectangle recCortarScore = { 1.0f, 1.0f, 100.0f, 36.0f }; 
    Rectangle recCortarTime  = { 1.0f, 54.0f, 100.0f, 36.0f }; 
    Rectangle recCortarRings = { 2.0f, 108.0f, 90.0f, 36.0f }; 
    
    Vector2 posScoreTela = { posBase.x, posBase.y };
    Vector2 posTimeTela  = { posBase.x, posBase.y + labelHSpacing }; 
    Vector2 posRingsTela = { posBase.x, posBase.y + ( labelHSpacing * 2 ) }; 
    
    Color corRings = WHITE;

    if ( gw->jogador->quantidadeAneis == 0 ) {
        if ( (int)( gw->tempoFase * 4 ) % 2 == 0 ) { 
            corRings = RED; 
        }
    }

    DrawTexturePro(
        rm.texturaHUD, 
        recCortarScore,
        (Rectangle){ 
            posScoreTela.x, 
            posScoreTela.y, 
            recCortarScore.width * mainHudScale * labelWidthScale, 
            recCortarScore.height * mainHudScale 
        },
        (Vector2){ 0 }, 
        0.0f, 
        WHITE
    );
    
    DrawTexturePro(
        rm.texturaHUD, 
        recCortarTime,
        (Rectangle){ 
            posTimeTela.x, 
            posTimeTela.y, 
            recCortarTime.width * mainHudScale * labelWidthScale, 
            recCortarTime.height * mainHudScale 
        },
        (Vector2){ 0 }, 
        0.0f, 
        WHITE
    );
    
    DrawTexturePro(
        rm.texturaHUD, 
        recCortarRings,
        (Rectangle){ 
            posRingsTela.x, 
            posRingsTela.y, 
            recCortarRings.width * mainHudScale * labelWidthScale, 
            recCortarRings.height * mainHudScale 
        },
        (Vector2){ 0 }, 
        0.0f, 
        corRings
    );

    float xStartVal = posBase.x + ( 100.0f * mainHudScale * labelWidthScale ) + 16.0f;
    float scaledNumW = 18.0f * mainHudScale;
    float scaledNumH = 36.0f * mainHudScale;

    char bufScore[7];
    snprintf( bufScore, sizeof( bufScore ), "%06d", gw->jogador->pontuacao );
    desenharNumerosCustomScaled( bufScore, xStartVal, posScoreTela.y, 6, WHITE, mainHudScale );

    int minutos = (int)gw->tempoFase / 60;
    int segundos = (int)gw->tempoFase % 60;

    char bufMinutos[10];
    snprintf( bufMinutos, sizeof( bufMinutos ), "%d", minutos );
    desenharNumerosCustomScaled( bufMinutos, xStartVal, posTimeTela.y, 1, WHITE, mainHudScale );

    DrawText( ":", xStartVal + ( 1 * scaledNumW ) + 4, posTimeTela.y - 1, 28, YELLOW );

    char bufSegundos[10];
    snprintf( bufSegundos, sizeof( bufSegundos ), "%02d", segundos );
    desenharNumerosCustomScaled( bufSegundos, xStartVal + ( 1 * scaledNumW ) + 16, posTimeTela.y, 2, WHITE, mainHudScale );

    char bufRings[4];
    snprintf( bufRings, sizeof( bufRings ), "%03d", gw->jogador->quantidadeAneis );
    desenharNumerosCustomScaled( bufRings, xStartVal, posRingsTela.y, 3, corRings, mainHudScale ); 

    float marginX = 16.0f;
    float marginY = 16.0f;
    float chaoDaTela = (float)GetScreenHeight();
    
    float scaledIconW = rm.texturaHUDVidas.width * livesHudScale;
    float scaledIconH = rm.texturaHUDVidas.height * livesHudScale;

    Vector2 posVidasTela = {
        marginX,
        chaoDaTela - scaledIconH - marginY
    }; 

    DrawTexturePro(
        rm.texturaHUDVidas,
        (Rectangle){
            0,
            0,
            (float)rm.texturaHUDVidas.width,
            (float)rm.texturaHUDVidas.height
        },
        (Rectangle){
            posVidasTela.x,
            posVidasTela.y,
            scaledIconW,
            scaledIconH
        },
        (Vector2){ 0 }, 
        0.0f, 
        WHITE
    );

    float vidasNumX = posVidasTela.x + scaledIconW - 50.0f; 
    float vidasNumY = posVidasTela.y + scaledIconH - scaledNumH + 8.0f;

    char bufVidas[3];
    snprintf( bufVidas, sizeof( bufVidas ), "%d", gw->jogador->quantidadeVidas );
    desenharNumerosCustomScaled( bufVidas, vidasNumX, vidasNumY, 1, WHITE, mainHudScale );

    /*
     * Indicador pequeno e limpo da fase atual.
     */
    DrawText(
        TextFormat( "FASE %d", gw->faseAtual ),
        GetScreenWidth() - 118,
        18,
        22,
        Fade( WHITE, 0.9f )
    );

}

/**
 * @brief Desenha o estado do jogo.
 */
void drawGameWorld( GameWorld *gw ) {

    BeginDrawing();

    if ( gw->estado == ESTADO_GAMEWORLD_MENU ) {
        desenharTelaMenu( gw );
        EndDrawing();
        return;
    }

    if ( gw->faseAtual == 2 ) {
        ClearBackground( (Color) { 34, 58, 244, 255 } );
    } else if ( gw->faseAtual == 3 ) {
        ClearBackground( (Color) { 91, 113, 136, 255 } );
    } else {
        ClearBackground( (Color) { 36, 0, 180, 255 } );
    }

    BeginMode2D( gw->camera );
    desenharFundo( gw );
    desenharMapa( gw->mapa );
    desenharPlacaChegada( gw );
    desenharJogador( gw->jogador );
    EndMode2D();

    desenharHUD( gw );

    if ( gw->mostrandoMensagemFase ) {
        desenharMensagemFase( gw );
    }

    if ( gw->estado == ESTADO_GAMEWORLD_TRANSICAO_FASE ) {
        desenharTelaTransicaoFase( gw );
    } else if ( gw->estado == ESTADO_GAMEWORLD_CONTINUE ) {
        desenharTelaContinue( gw );
    }

    EndDrawing();

}

static void desenharFundoTela( GameWorld *gw, Color tonalidade ) {

    ClearBackground( (Color){ 20, 32, 96, 255 } );

    Texture2D *texturaFundoAtual = getTexturaFundoAtual( gw );

    if ( texturaFundoAtual != NULL && texturaFundoAtual->id > 0 ) {

        float escalaX = (float)GetScreenWidth() / texturaFundoAtual->width;
        float escalaY = (float)GetScreenHeight() / texturaFundoAtual->height;
        float escala = escalaX > escalaY ? escalaX : escalaY;

        Rectangle destino = {
            0.0f,
            0.0f,
            texturaFundoAtual->width * escala,
            texturaFundoAtual->height * escala
        };

        DrawTexturePro(
            *texturaFundoAtual,
            (Rectangle){ 0, 0, (float)texturaFundoAtual->width, (float)texturaFundoAtual->height },
            destino,
            (Vector2){ 0 },
            0.0f,
            tonalidade
        );

    }

}

static void desenharTexturaTelaCheia( Texture2D textura, Color tonalidade ) {

    if ( textura.id <= 0 ) return;

    float escalaX = (float)GetScreenWidth() / textura.width;
    float escalaY = (float)GetScreenHeight() / textura.height;
    float escala = escalaX > escalaY ? escalaX : escalaY;

    Rectangle destino = {
        0.0f,
        0.0f,
        textura.width * escala,
        textura.height * escala
    };

    DrawTexturePro(
        textura,
        (Rectangle){ 0.0f, 0.0f, (float)textura.width, (float)textura.height },
        destino,
        (Vector2){ 0.0f, 0.0f },
        0.0f,
        tonalidade
    );

}

static void desenharTelaMenu( GameWorld *gw ) {

    desenharFundoTela( gw, Fade( WHITE, 0.78f ) );
    desenharTexturaTelaCheia( rm.texturaTelaMenu, Fade( WHITE, 0.52f ) );

    int largura = GetScreenWidth();
    int altura = GetScreenHeight();
    float pulso = 0.5f + 0.5f * sinf( gw->tempoTela * 4.0f );

    DrawRectangleGradientV(
        0,
        0,
        largura,
        altura,
        Fade( (Color){ 0, 10, 40, 255 }, 0.30f ),
        Fade( (Color){ 0, 0, 0, 255 }, 0.72f )
    );

    DrawCircleLines( largura / 2, altura / 2 - 42, 190.0f + 10.0f * pulso, Fade( YELLOW, 0.80f ) );
    DrawCircleLines( largura / 2, altura / 2 - 42, 168.0f, Fade( SKYBLUE, 0.82f ) );
    DrawCircleLines( largura / 2, altura / 2 - 42, 134.0f, Fade( WHITE, 0.22f ) );

    const char *titulo = "SONIC RAYLIB";
    int fonteTitulo = largura < 900 ? 56 : 72;
    int larguraTitulo = MeasureText( titulo, fonteTitulo );

    int yTitulo = altura / 2 - 126;
    DrawText( titulo, largura / 2 - larguraTitulo / 2 + 5, yTitulo + 5, fonteTitulo, Fade( BLACK, 0.55f ) );
    DrawText( titulo, largura / 2 - larguraTitulo / 2, yTitulo, fonteTitulo, GOLD );

    const char *subtitulo = "Projeto de ORIN";
    int fonteSubtitulo = 30;
    int larguraSubtitulo = MeasureText( subtitulo, fonteSubtitulo );
    DrawText( subtitulo, largura / 2 - larguraSubtitulo / 2, yTitulo + 58, fonteSubtitulo, WHITE );

    const char *selecao = "SELECIONE A FASE";
    int fonteSelecao = 22;
    int larguraSelecao = MeasureText( selecao, fonteSelecao );
    DrawText( selecao, largura / 2 - larguraSelecao / 2, yTitulo + 116, fonteSelecao, Fade( WHITE, 0.82f ) );

    int larguraOpcao = 116;
    int alturaOpcao = 52;
    int espacamento = 18;
    int larguraGrupo = larguraOpcao * 3 + espacamento * 2;
    int xGrupo = largura / 2 - larguraGrupo / 2;
    int yOpcoes = yTitulo + 150;

    for ( int fase = 1; fase <= 3; fase++ ) {
        int x = xGrupo + ( fase - 1 ) * ( larguraOpcao + espacamento );
        bool selecionada = gw->faseSelecionada == fase;
        Color fundo = selecionada ? Fade( YELLOW, 0.90f ) : Fade( BLACK, 0.48f );
        Color texto = selecionada ? (Color){ 10, 32, 88, 255 } : WHITE;

        DrawRectangle( x, yOpcoes, larguraOpcao, alturaOpcao, fundo );
        DrawRectangleLinesEx(
            (Rectangle){ x, yOpcoes, larguraOpcao, alturaOpcao },
            selecionada ? 3.0f : 1.0f,
            selecionada ? WHITE : Fade( SKYBLUE, 0.65f )
        );

        const char *rotulo = TextFormat( "FASE %d", fase );
        int larguraRotulo = MeasureText( rotulo, 22 );
        DrawText( rotulo, x + larguraOpcao / 2 - larguraRotulo / 2, yOpcoes + 15, 22, texto );
    }

    const char *acao = "ENTER PARA INICIAR";
    int fonteAcao = 25;
    int larguraAcao = MeasureText( acao, fonteAcao );
    DrawText( acao, largura / 2 - larguraAcao / 2, yOpcoes + 72, fonteAcao, Fade( WHITE, 0.45f + 0.55f * pulso ) );

    DrawRectangleGradientV(
        0,
        altura - 150,
        largura,
        150,
        Fade( BLACK, 0.0f ),
        Fade( BLACK, 0.36f )
    );

    const char *controles = "SETAS/A-D: selecionar e mover    ESPACO/W: pular    R: reiniciar";
    int fonteControles = 22;
    int larguraControles = MeasureText( controles, fonteControles );
    DrawText( controles, largura / 2 - larguraControles / 2, altura - 70, fonteControles, Fade( WHITE, 0.78f ) );

    const char *autor = "feito por Kevyn Gandolfi";
    int fonteAutor = 20;
    int larguraAutor = MeasureText( autor, fonteAutor );
    DrawText( autor, largura - larguraAutor - 24, altura - 34, fonteAutor, Fade( WHITE, 0.72f ) );

}

static void desenharTelaContinue( GameWorld *gw ) {

    int largura = GetScreenWidth();
    int altura = GetScreenHeight();
    int segundos = (int)ceilf( gw->tempoContinue );
    float pulso = 0.5f + 0.5f * sinf( gw->tempoTela * 8.0f );

    DrawRectangle( 0, 0, largura, altura, Fade( BLACK, 0.70f ) );

    const char *texto = "CONTINUE?";
    int fonteTexto = 72;
    int larguraTexto = MeasureText( texto, fonteTexto );
    DrawText( texto, largura / 2 - larguraTexto / 2 + 4, altura / 2 - 145 + 4, fonteTexto, Fade( BLACK, 0.55f ) );
    DrawText( texto, largura / 2 - larguraTexto / 2, altura / 2 - 145, fonteTexto, GOLD );

    DrawCircleLines( largura / 2, altura / 2 + 10, 78.0f + 8.0f * pulso, Fade( RED, 0.88f ) );
    DrawCircleLines( largura / 2, altura / 2 + 10, 58.0f, Fade( WHITE, 0.88f ) );

    const char *contador = TextFormat( "%02d", segundos < 0 ? 0 : segundos );
    int fonteContador = 72;
    int larguraContador = MeasureText( contador, fonteContador );
    DrawText( contador, largura / 2 - larguraContador / 2, altura / 2 - 26, fonteContador, WHITE );

    const char *acao = "ENTER continua    R volta ao inicio";
    int fonteAcao = 26;
    int larguraAcao = MeasureText( acao, fonteAcao );
    DrawText( acao, largura / 2 - larguraAcao / 2, altura / 2 + 120, fonteAcao, Fade( WHITE, 0.90f ) );

}

static void desenharTelaTransicaoFase( GameWorld *gw ) {

    int largura = GetScreenWidth();
    int altura = GetScreenHeight();
    float progresso = 1.0f - gw->duracaoTransicao / 4.0f;

    if ( progresso < 0.0f ) progresso = 0.0f;
    if ( progresso > 1.0f ) progresso = 1.0f;

    int bonusMostrado = (int)( gw->bonusTotal * progresso );
    const char *rank = "C";

    if ( gw->bonusTotal >= 7000 ) {
        rank = "S";
    } else if ( gw->bonusTotal >= 5000 ) {
        rank = "A";
    } else if ( gw->bonusTotal >= 3000 ) {
        rank = "B";
    }

    ClearBackground( (Color){ 7, 24, 66, 255 } );
    DrawRectangle( 0, 0, largura, 14, YELLOW );
    DrawRectangle( 0, altura - 14, largura, 14, SKYBLUE );
    DrawCircleLines( largura / 2, altura / 2, 290.0f, Fade( SKYBLUE, 0.18f ) );
    DrawCircleLines( largura / 2, altura / 2, 230.0f, Fade( YELLOW, 0.14f ) );

    const char *titulo = "FASE CONCLUIDA";
    int fonteTitulo = 64;
    int larguraTitulo = MeasureText( titulo, fonteTitulo );
    DrawText( titulo, largura / 2 - larguraTitulo / 2 + 4, altura / 2 - 190 + 4, fonteTitulo, Fade( BLACK, 0.55f ) );
    DrawText( titulo, largura / 2 - larguraTitulo / 2, altura / 2 - 190, fonteTitulo, GOLD );

    DrawText( TextFormat( "FASE %d", gw->faseAtual ), largura / 2 - 275, altura / 2 - 102, 24, Fade( SKYBLUE, 0.90f ) );
    DrawLineEx(
        (Vector2){ largura / 2.0f - 275.0f, altura / 2.0f - 66.0f },
        (Vector2){ largura / 2.0f + 275.0f, altura / 2.0f - 66.0f },
        2.0f,
        Fade( WHITE, 0.30f )
    );

    DrawText( TextFormat( "RING BONUS    %05d", gw->bonusAneis ), largura / 2 - 225, altura / 2 - 38, 30, WHITE );
    DrawText( TextFormat( "TIME BONUS    %05d", gw->bonusTempo ), largura / 2 - 225, altura / 2 + 12, 30, WHITE );
    DrawText( TextFormat( "TOTAL         %05d", bonusMostrado ), largura / 2 - 225, altura / 2 + 72, 34, YELLOW );

    DrawText( "RANK", largura / 2 + 145, altura / 2 - 28, 22, Fade( WHITE, 0.70f ) );
    DrawText( rank, largura / 2 + 155, altura / 2 + 2, 82, rank[0] == 'S' ? YELLOW : SKYBLUE );

    const char *acao = "ENTER para avancar";
    int fonteAcao = 22;
    int larguraAcao = MeasureText( acao, fonteAcao );
    DrawText( acao, largura / 2 - larguraAcao / 2, altura / 2 + 170, fonteAcao, Fade( WHITE, 0.75f ) );

}

static void desenharMensagemFase( GameWorld *gw ) {

    const char *texto = "FASE 1";

    if ( gw->faseAtual == 2 ) {
        texto = "FASE 2";
    } else if ( gw->faseAtual == 3 ) {
        texto = "FASE 3";
    }

    int fontSize = 38;
    int larguraTexto = MeasureText( texto, fontSize );

    float alpha = gw->tempoMensagemFase;

    if ( alpha > 1.0f ) alpha = 1.0f;
    if ( alpha < 0.0f ) alpha = 0.0f;

    int centroY = GetScreenHeight() / 2;

    DrawRectangle(
        0,
        centroY - 46,
        GetScreenWidth(),
        92,
        Fade( BLACK, 0.42f * alpha )
    );

    DrawLineEx(
        (Vector2){ GetScreenWidth() / 2.0f - 90.0f, centroY - 24.0f },
        (Vector2){ GetScreenWidth() / 2.0f + 90.0f, centroY - 24.0f },
        2.0f,
        Fade( YELLOW, 0.85f * alpha )
    );

    DrawText(
        texto,
        GetScreenWidth() / 2 - larguraTexto / 2,
        centroY - fontSize / 2,
        fontSize,
        Fade( WHITE, alpha )
    );

}

static void desenharPlacaChegada( GameWorld *gw ) {

    if ( gw->mapa == NULL ) return;

    float x = calcularLarguraMapa( gw->mapa ) - 360.0f;
    float yChao = calcularYChaoPrincipal( gw->mapa );
    float topo = yChao - 104.0f;

    DrawLineEx( (Vector2){ x, topo }, (Vector2){ x, yChao + 2.0f }, 4.0f, LIGHTGRAY );
    DrawLineEx( (Vector2){ x + 5.0f, topo }, (Vector2){ x + 5.0f, yChao + 2.0f }, 2.0f, Fade( BLACK, 0.45f ) );

    int tamanho = 11;
    for ( int linha = 0; linha < 3; linha++ ) {
        for ( int coluna = 0; coluna < 4; coluna++ ) {
            Color cor = ( linha + coluna ) % 2 == 0 ? WHITE : (Color){ 20, 50, 120, 255 };
            DrawRectangle( (int)x + 4 + coluna * tamanho, (int)topo + linha * tamanho, tamanho, tamanho, cor );
        }
    }

    DrawCircleV( (Vector2){ x + 2.0f, yChao + 1.0f }, 6.0f, Fade( BLACK, 0.40f ) );

}

static Texture2D *getTexturaFundoAtual( GameWorld *gw ) {

    if ( gw->faseAtual == 2 && rm.texturaFundoFase02.id > 0 ) {
        return &rm.texturaFundoFase02;
    }

    if ( gw->faseAtual == 3 && rm.texturaFundoFase03.id > 0 ) {
        return &rm.texturaFundoFase03;
    }

    return &rm.texturaFundo;

}

static void desenharFundo( GameWorld *gw ) {

    Texture2D *texturaFundoAtual = getTexturaFundoAtual( gw );

    if ( texturaFundoAtual == NULL || texturaFundoAtual->id <= 0 ) return;

    int larguraMapa = calcularLarguraMapa( gw->mapa );

    if ( texturaFundoAtual->width <= 0 || larguraMapa <= 0 ) return;

    float escala = 2.0f;
    float larguraFundo = texturaFundoAtual->width * escala;
    float viewX = gw->camera.target.x - gw->camera.offset.x;
    float viewY = gw->camera.target.y - gw->camera.offset.y;
    float yChaoPrincipal = calcularYChaoPrincipal( gw->mapa );
    float yFundo = yChaoPrincipal + gw->mapa->dimensaoPadraoElementos - texturaFundoAtual->height * escala;

    if ( gw->faseAtual == 2 ) {
        float escalaX = (float)GetScreenWidth() / texturaFundoAtual->width;
        float escalaY = (float)GetScreenHeight() / texturaFundoAtual->height;
        escala = ( escalaX > escalaY ? escalaX : escalaY ) * 1.20f;
        larguraFundo = texturaFundoAtual->width * escala;
        yFundo = viewY - ( texturaFundoAtual->height * escala - GetScreenHeight() ) * 0.35f;
    } else if ( gw->faseAtual == 3 ) {
        float escalaX = (float)GetScreenWidth() / texturaFundoAtual->width;
        float escalaY = (float)GetScreenHeight() / texturaFundoAtual->height;
        escala = ( escalaX > escalaY ? escalaX : escalaY ) * 1.20f;
        larguraFundo = texturaFundoAtual->width * escala;
        yFundo = viewY - ( texturaFundoAtual->height * escala - GetScreenHeight() ) * 0.40f;
    }

    int deslocamentoParallax = (int)( ( gw->camera.target.x / (float)larguraMapa ) * 220 );
    int primeiroBloco = (int)floorf( ( viewX + deslocamentoParallax ) / larguraFundo ) - 1;
    int ultimoBloco = primeiroBloco + GetScreenWidth() / (int)larguraFundo + 4;

    for ( int i = primeiroBloco; i <= ultimoBloco; i++ ) {

        DrawTexturePro(
            *texturaFundoAtual,
            (Rectangle){ 0.0f, 0.0f, (float)texturaFundoAtual->width, (float)texturaFundoAtual->height },
            (Rectangle){ larguraFundo * i - deslocamentoParallax, yFundo, larguraFundo, texturaFundoAtual->height * escala },
            (Vector2){ 0.0f, 0.0f },
            0.0f,
            WHITE
        );

    }
}

static void atualizarCamera( GameWorld *gw ) {

    Jogador *j = gw->jogador;
    Camera2D *c = &gw->camera;
    int margemFimMapa = 220;

    c->offset.x = GetScreenWidth() / 2;
    c->offset.y = GetScreenHeight() / 2;

    c->target.x = roundf( j->ret.x + j->ret.width / 2.0f );
    c->target.y = roundf( j->ret.y + j->ret.height / 2.0f );

    int minX = GetScreenWidth() / 2;
    int maxX = calcularLarguraMapa( gw->mapa ) - GetScreenWidth() / 2 - margemFimMapa;
    int maxY = (int)( calcularYChaoPrincipal( gw->mapa ) + gw->mapa->dimensaoPadraoElementos * 2 - GetScreenHeight() / 2 );

    if ( maxX < minX ) {
        maxX = minX;
    }

    if ( c->target.x < minX ) {
        c->target.x = minX;
    } else if ( c->target.x > maxX ) {
        c->target.x = maxX;
    }

    if ( c->target.y > maxY ) {
        c->target.y = maxY;
    }

}

static void inicializar( GameWorld *gw ) {

    gw->mapa = NULL;
    gw->jogador = NULL;

    gw->camera = (Camera2D) {
        .offset = { 0 },
        .target = { 0 },
        .rotation = 0.0f,
        .zoom = 1.0f
    };

    gw->gravidade = 900;
    gw->tempoFase = 0.0f;

    gw->faseAtual = 1;
    gw->faseSelecionada = 1;
    gw->mostrandoMensagemFase = false;
    gw->tempoMensagemFase = 0.0f;
    gw->estado = ESTADO_GAMEWORLD_MENU;
    gw->tempoTela = 0.0f;
    gw->tempoContinue = 10.0f;
    gw->duracaoTransicao = 0.0f;
    gw->proximaFase = 1;
    gw->bonusAneis = 0;
    gw->bonusTempo = 0;
    gw->bonusTotal = 0;
    gw->pontuacaoAntesTransicao = 0;

    carregarFase( gw, 1 );

}

static void carregarFase( GameWorld *gw, int fase ) {

    int vidas = 3;
    int score = 0;

    if ( gw->jogador != NULL ) {
        vidas = gw->jogador->quantidadeVidas;
        score = gw->jogador->pontuacao;
    }

    if ( gw->mapa != NULL ) {
        destruirMapa( gw->mapa );
        gw->mapa = NULL;
    }

    if ( gw->jogador != NULL ) {
        destruirJogador( gw->jogador );
        gw->jogador = NULL;
    }

    gw->faseAtual = fase;

    if ( fase == 1 ) {
        gw->mapa = carregarMapa( "resources/mapas/mapa01.txt" );
    } else if ( fase == 2 ) {
        gw->mapa = carregarMapa( "resources/mapas/mapa02.txt" );
    } else {
        gw->mapa = carregarMapa( "resources/mapas/mapa03.txt" );
    }

    float xSpawn = 144.0f;

    gw->jogador = criarJogador(
        xSpawn,
        calcularYSpawnJogador( gw->mapa, xSpawn ),
        96,
        96
    );

    gw->jogador->quantidadeVidas = vidas;
    gw->jogador->pontuacao = score;
    gw->jogador->quantidadeAneis = 0;

    gw->tempoFase = 0.0f;

    gw->mostrandoMensagemFase = gw->estado == ESTADO_GAMEWORLD_JOGANDO;
    gw->tempoMensagemFase = gw->mostrandoMensagemFase ? 1.65f : 0.0f;

    atualizarCamera( gw );

}

static float calcularYSpawnJogador( Mapa *mapa, float xSpawn ) {

    float yChao = (float)calcularAlturaMapa( mapa );
    float centroSpawn = xSpawn + 48.0f;
    ElementoMapa *el = mapa->obstaculos;

    while ( el != NULL ) {

        Obstaculo *o = (Obstaculo*)el->objeto;

        if ( centroSpawn >= o->ret.x &&
             centroSpawn < o->ret.x + o->ret.width &&
             o->ret.y > 240.0f &&
             o->ret.y < yChao ) {
            yChao = o->ret.y;
        }

        el = el->proximo;

    }

    return yChao - 148.0f;

}

static float calcularYChaoPrincipal( Mapa *mapa ) {

    if ( mapa == NULL || mapa->linhas <= 0 || mapa->colunas <= 0 ) {
        return 0.0f;
    }

    int contagemPorLinha[mapa->linhas];

    for ( int i = 0; i < mapa->linhas; i++ ) {
        contagemPorLinha[i] = 0;
    }

    ElementoMapa *el = mapa->obstaculos;

    while ( el != NULL ) {

        Obstaculo *o = (Obstaculo*)el->objeto;
        int linha = (int)( o->ret.y / mapa->dimensaoPadraoElementos );

        if ( linha >= 0 && linha < mapa->linhas ) {
            contagemPorLinha[linha]++;
        }

        el = el->proximo;

    }

    int minimoBlocosChao = (int)( mapa->colunas * 0.75f );

    for ( int i = 0; i < mapa->linhas; i++ ) {
        if ( contagemPorLinha[i] >= minimoBlocosChao ) {
            return i * mapa->dimensaoPadraoElementos;
        }
    }

    return calcularAlturaMapa( mapa ) - mapa->dimensaoPadraoElementos;

}

static void atualizarMusicaFase( GameWorld *gw ) {

    if ( IsMusicStreamPlaying( rm.musicaFase02 ) ) {
        StopMusicStream( rm.musicaFase02 );
    }

    if ( !IsMusicStreamPlaying( rm.musicaFase01 ) ) {
        PlayMusicStream( rm.musicaFase01 );
    }

    UpdateMusicStream( rm.musicaFase01 );

}

static void verificarTransicaoFase( GameWorld *gw ) {

    int larguraMapa = calcularLarguraMapa( gw->mapa );

    /*
     * Chegou perto do fim do mapa:
     * fase 1 vai para fase 2.
     * fase 2 reinicia a própria fase, como demonstração contínua.
     */
    if ( gw->jogador->ret.x >= larguraMapa - 300 ) {

        if ( gw->faseAtual == 1 ) {
            iniciarTransicaoFase( gw, 2 );
        } else if ( gw->faseAtual == 2 ) {
            iniciarTransicaoFase( gw, 3 );
        } else {
            iniciarTransicaoFase( gw, 0 );
        }

    }

}

static void iniciarTransicaoFase( GameWorld *gw, int proximaFase ) {

    int segundos = (int)gw->tempoFase;
    int bonusTempoBase = 300 - segundos;

    if ( bonusTempoBase < 0 ) {
        bonusTempoBase = 0;
    }

    gw->estado = ESTADO_GAMEWORLD_TRANSICAO_FASE;
    gw->tempoTela = 0.0f;
    gw->duracaoTransicao = 4.0f;
    gw->proximaFase = proximaFase;

    gw->bonusAneis = gw->jogador->quantidadeAneis * 100;
    gw->bonusTempo = bonusTempoBase * 10;
    gw->bonusTotal = gw->bonusAneis + gw->bonusTempo;
    gw->pontuacaoAntesTransicao = gw->jogador->pontuacao;

}

static void continuarJogo( GameWorld *gw ) {

    int fase = gw->faseAtual;

    carregarFase( gw, fase );

    gw->jogador->quantidadeVidas = 3;
    gw->jogador->pontuacao = 0;
    gw->jogador->quantidadeAneis = 0;

    gw->estado = ESTADO_GAMEWORLD_JOGANDO;
    gw->tempoTela = 0.0f;
    gw->mostrandoMensagemFase = true;
    gw->tempoMensagemFase = 1.65f;

}

static void reiniciar( GameWorld *gw ) {

    if ( IsMusicStreamPlaying( rm.musicaFase01 ) ) {
        StopMusicStream( rm.musicaFase01 );
    }

    if ( IsMusicStreamPlaying( rm.musicaFase02 ) ) {
        StopMusicStream( rm.musicaFase02 );
    }

    if ( gw->mapa != NULL ) {
        destruirMapa( gw->mapa );
        gw->mapa = NULL;
    }

    if ( gw->jogador != NULL ) {
        destruirJogador( gw->jogador );
        gw->jogador = NULL;
    }

    inicializar( gw );

}
