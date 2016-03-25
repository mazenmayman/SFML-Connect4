
//
// Disclamer:
// ----------
//
// This code will work only if you selected window, graphics and audio.
//
// Note that the "Run Script" build phase will copy the required frameworks
// or dylibs to your application bundle so you can execute it on any OS X
// computer.
//
// Your resource files (images, sounds, fonts, ...) are also copied to your
// application bundle. To get the path to these resource, use the helper
// method resourcePath() from ResourcePath.hpp
//

#include "ResourcePath.hpp"
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <iomanip>

using namespace sf;
using namespace std;

#define rows    6
#define cols    7
#define tile_size   100
#define space_size 10

#define screen_width    (cols + 2) * tile_size
#define screen_height   (rows + 2) * tile_size

#define game_width  cols * tile_size
#define game_height rows * tile_size

#define game_xpos   tile_size
#define game_ypos   tile_size

#define circle_radius (tile_size - space_size) / 2

#define win_size 4

enum Turn { player1 = 1, player2 };

struct player {
    Turn p_turn;
    Color p_color;
};

player game_player1;
player game_player2;


void intialize_restart(int game_array[rows][cols], CircleShape game_circles[rows][cols], int &curr_col, player &curr_player,CircleShape &curr_player_circle, Text &curr_player_text, Text &result_text, bool &game_end);
void switch_player(player &curr_player, CircleShape &curr_player_circle, Text &curr_player_text, Text &result_text);
bool update_game(int game_array[rows][cols], CircleShape game_circles[rows][cols], int &curr_col, player &curr_player);
bool checker(int game_array[rows][cols], player &curr_player, int row, int col, int row_change, int col_change, int win_moves);
bool check_won(int game_array[rows][cols], player &curr_player);
bool game_finished(int a[rows][cols]);


int main()
{
    // Window Creation
    RenderWindow window(VideoMode(screen_width, screen_height), "Connect" + to_string(win_size));
    
    // Rectangle Shape Creation of the border
    RectangleShape game_rect;
    game_rect.setSize(Vector2f(game_width, game_height));
    game_rect.setFillColor(Color::Blue);
    game_rect.setPosition(game_xpos, game_ypos);
    
    // declare the game_array
    int game_array[rows][cols] = {0};
    
    // declare the game_circles and set their positions
    CircleShape game_circles[rows][cols];
    float posx = game_xpos + space_size/2, posy = game_ypos + space_size/2;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            game_circles[i][j].setRadius(circle_radius);
            game_circles[i][j].setFillColor(Color::White);
            game_circles[i][j].setPosition(posx, posy);
            posx += tile_size;
        }
        posx = game_xpos + space_size/2;
        posy += tile_size;
    }
    
    
    // initialize the two players
    game_player1.p_turn = player1;
    game_player2.p_turn = player2;
    game_player1.p_color = Color::Red;
    game_player2.p_color = Color::Green;
    
    
    // declare curr_col & curr_player
    int curr_col;
    player curr_player;
    
    // declare the curr_player_circle
    CircleShape curr_player_circle;
    curr_player_circle.setRadius(circle_radius);
    curr_player_circle.setPosition(((curr_col+1)*tile_size)+space_size/2, space_size/2);
    curr_player_circle.setFillColor(curr_player.p_color);
    
    
    /*** Texts ***/
    // get the font
    sf::Font font;
    if (!font.loadFromFile(resourcePath() + "sansation.ttf")) {
        cout << "Failed to load sansation" << endl;
        return EXIT_FAILURE;
    }
    // declare the curr_player_text
    Text curr_player_text;
    curr_player_text.setFont(font);
    curr_player_text.setCharacterSize(tile_size/4);
    curr_player_text.setPosition(tile_size*0.1,tile_size*0.1);
    
    // dcelare the result_text
    Text result_text;
    result_text.setFont(font);
    result_text.setCharacterSize(tile_size/4);
    result_text.setPosition(tile_size+space_size/2, screen_height-tile_size+space_size/2);
    
    /*** game_end ***/
    // declare the game_end flag
    bool game_end;
    
    /*********/
    // initialize all the game variables
    intialize_restart(game_array, game_circles, curr_col, curr_player, curr_player_circle, curr_player_text, result_text, game_end);
    
    while (window.isOpen())
    {
        // flag to check if board has changed
        bool changed = false;
        
        //processing the events and user inputs
        Event currentevent;
        while (window.pollEvent(currentevent))
        {
            if (currentevent.type == Event::Closed)
                window.close();
            if (currentevent.type == Event::KeyPressed)
            {
                if (currentevent.key.code == Keyboard::Escape)
                    window.close();
                
                if(!game_end){
                    if (currentevent.key.code == Keyboard::Right){
                        if(curr_player_circle.getPosition().x < (screen_width-2*tile_size)){
                            curr_col++;
                            curr_player_circle.setPosition(((curr_col+1)*tile_size)+space_size/2, space_size/2);
                        }
                    }
                    
                    if (currentevent.key.code == Keyboard::Left){
                        if(curr_player_circle.getPosition().x > (2*tile_size)){
                            curr_col--;
                            curr_player_circle.setPosition(((curr_col+1)*tile_size)+space_size/2, space_size/2);
                        }
                    }
                    
                    
                    if (currentevent.key.code == Keyboard::Down){
                        changed = update_game(game_array, game_circles, curr_col, curr_player);
                        if(!changed){
                            result_text.setString("Idiot! can't play here.");
                        }
                        
                    }
                }
                else{
                    if (currentevent.key.code == Keyboard::Space){
                        intialize_restart(game_array, game_circles, curr_col, curr_player, curr_player_circle, curr_player_text, result_text, game_end);
                    }
                }
            }
        }
        
        // check winner or game_finished
        if(changed){
            // check if the player won
            if(check_won(game_array, curr_player)){
                game_end = true;
                curr_player_text.setString("");
                result_text.setString("Player" + to_string(curr_player.p_turn) + " won! \t\t Press Space to restart");
                result_text.setColor(curr_player.p_color);
            }
            // check if the game_finished
            else if(game_finished(game_array)) {
                result_text.setString("Game Over! Press Space to restart");
            }
            else{ // just switch turns
                switch_player(curr_player, curr_player_circle, curr_player_text, result_text);
            }
        }
        
        // drawing
        window.clear(Color::White);
        window.draw(game_rect);
        for (int i = 0; i < rows;i++ )
            for (int j = 0; j < cols; j++)
                window.draw(game_circles[i][j]);
        
        window.draw(curr_player_circle);
        window.draw(curr_player_text);
        window.draw(result_text);
        window.display();
    }
}

void intialize_restart(int game_array[rows][cols], CircleShape game_circles[rows][cols], int &curr_col, player &curr_player,CircleShape &curr_player_circle, Text &curr_player_text, Text &result_text, bool &game_end){
    
    // reset the game_curcles & game_array
    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){
            game_circles[i][j].setFillColor(Color::White);
            game_array[i][j] = 0;
        }
    }
    
    // reset curr_col, curr_player
    curr_col = 0;
    curr_player = game_player1;
    
    // reset the curr_player_circle
    curr_player_circle.setPosition(((curr_col+1)*tile_size)+space_size/2, space_size/2);
    curr_player_circle.setFillColor(curr_player.p_color);
    
    // reset the curr_player_text
    curr_player_text.setColor(curr_player.p_color);
    curr_player_text.setString("Player: "+to_string(curr_player.p_turn));
    
    // reset the result_text
    result_text.setColor(Color::Black);
    result_text.setString("");
    
    game_end = false;
}

bool update_game(int game_array[rows][cols], CircleShape game_circles[rows][cols], int &curr_col, player &curr_player){
    
    for(int i = rows-1; i >= 0; i--){
        if(game_array[i][curr_col] == 0){
            game_array[i][curr_col] = curr_player.p_turn;
            game_circles[i][curr_col].setFillColor(curr_player.p_color);
            return true;
        }
    }
    
    return false;
}

void switch_player(player &curr_player, CircleShape &curr_player_circle, Text &curr_player_text, Text &result_text){
    
    if(curr_player.p_turn == player1){
        curr_player = game_player2;
    }
    else{
        curr_player = game_player1;
    }
    
    curr_player_circle.setFillColor(curr_player.p_color);
    curr_player_text.setColor(curr_player.p_color);
    curr_player_text.setString("Player: "+to_string(curr_player.p_turn));
    result_text.setString("");
}

bool checker(int game_array[rows][cols], player &curr_player, int row, int col, int row_change, int col_change, int win_moves){
    // If the current cell is not our curr_player, return false right away
    if(game_array[row][col] != curr_player.p_turn){
        return false;
    }
    
    // else
    // Base case if the current cell is curr_player
    if(win_moves == 1){
        return true;
    }
    // else Recursive Base
    row += row_change;
    col += col_change;
    return checker(game_array, curr_player, row, col, row_change, col_change, win_moves-1);
}

bool check_won(int game_array[rows][cols], player &curr_player){
    
    // horizontal checking
    for (int i = 0; i < rows; i++)  // We check each row
        for(int j = 0; j < cols-win_size+1; j++)
            // Check each cell in the same row if it's the start of adjecent "win_size" cells
            if (checker(game_array, curr_player, i, j, 0, 1, win_size))
                return true;
    
    // vertical checking
    for (int j = 0; j < cols; j++)  // We check each column
        for(int i = 0; i < rows-win_size+1; i++)
            // Check each cell in the same column if it's the start of adjecent "win_size" cells
            if (checker(game_array, curr_player, i, j, 1, 0, win_size))
                return true;
    
    // diagonal checking
    for (int i = 0; i < rows-win_size+1; i++)
        for (int j = 0; j < cols-win_size+1; j++)
            if (checker(game_array, curr_player, i, j, 1, 1, win_size))
                return true;
    
    // reverse diagonal checking
    for (int i = 0; i < rows-win_size+1; i++)
        for (int j = cols-1; j >= win_size-1; j--)
            if (checker(game_array, curr_player, i, j, 1, -1, win_size))
                return true;
    
    return false;
}


bool game_finished(int a[rows][cols])
{
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            if (a[i][j] == 0)
                return false;
    
    return false;
}