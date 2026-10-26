#include "game.h"

Game::Game(){
    createInitialState(session.currentState, 19);

    board.LoadAssetsDefault("assets/board/wood1.jpg", "assets/b_40.png", "assets/w_40.png");
    background.LoadAssets("assets/go-jeu_1000_blur.jpg");
    background.SetPosition(0, 0);
    logo.LoadAssets("assets/UI/icon258.png");
    logo.SetPosition(371, 200);
    playButton.LoadAssets("assets/UI/PNG/Yellow/Default/button_rectangle_depth_flat.png");
    currentScreen = MENU;
}

void Game::Draw(){
    switch (currentScreen)
    {
    case MENU:
    {
        background.Draw(true);
        logo.Draw(false);
        playButton.Draw();
    }break;
    case PVP:
    {
        board.Draw(session.currentState.board);
        
    } break;
    case PVE:
    {

    } break;
    case EXIT:
    {

    } break;
    default:
        break;
    }
}

void Game::Update(){
    switch (currentScreen)
    {
    case MENU:
        if (playButton.IsClicked(GetMousePosition())) {
            currentScreen = PVP;
        }
        break;
    
    default:
        break;
    }
}

// void Game::GhostStone(int col, int row) {
//     std::pair<int,int> cell = board.GetCellIndex(GetMousePosition());

//     if (cell != std::pair<int, int>{-1, -1}){
//         board.DrawGhostStone(cell.first, cell.second, currentPlayer);
//     }
// }

