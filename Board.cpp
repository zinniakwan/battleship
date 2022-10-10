#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

class BoardImpl
{
  public:
    BoardImpl(const Game& g);
    void clear();
    void block();
    void unblock();
    bool placeShip(Point topOrLeft, int shipId, Direction dir);
    bool unplaceShip(Point topOrLeft, int shipId, Direction dir);
    void display(bool shotsOnly) const;
    bool attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId);
    bool allShipsDestroyed() const;

  private:
    struct shipsPlaced{
        int idNum;
        Point topLoc;
        int undamaged;
    };
    
    char grid[MAXCOLS][MAXROWS];
    int cols, rows;
    const Game& m_game;
    
    vector<shipsPlaced> idPlaced;
};

BoardImpl::BoardImpl(const Game& g)
 : m_game(g)
{
    cols=m_game.cols();
    rows=m_game.rows();
    clear();
    
}

void BoardImpl::clear()
//helps to clear out the grid, by resetting every value to '.'
{
    for (int i=0; i<rows; i++){
        for(int j=0; j<cols; j++ ){
            grid[i][j]='.';
        }
    }
}

void BoardImpl::block()
{
      // Block cells with 50% probability
    int counter = 0;
    while(counter< (m_game.cols() * m_game.rows()) /2){
        int r = randInt(m_game.rows());
        int c = randInt(m_game.cols());
        if (grid[r][c]=='.'){
            grid[r][c]='#';
            counter++;
        }
    }
}

void BoardImpl::unblock()
{
    //loop through every point on the boad, and if it is blocked, unblock it
    for (int r = 0; r < m_game.rows(); r++)
        for (int c = 0; c < m_game.cols(); c++)
        {
            if (grid[r][c]=='#') {grid[r][c]='.';}
        }
}

bool BoardImpl::placeShip(Point topOrLeft, int shipId, Direction dir)
{
    // the beginning has a bunch of statements that makes sure the ship can be placed at a certain point
    if(shipId>m_game.nShips()) {return false;}
    for (int i=0; i< idPlaced.size(); i++){
        if (idPlaced.at(i).idNum==shipId){return false;}
    }
    if (topOrLeft.r<0 || topOrLeft.c<0 || topOrLeft.r>=m_game.rows() || topOrLeft.c>=m_game.cols()) {return false;}
    if (dir==HORIZONTAL){
        if (topOrLeft.c+m_game.shipLength(shipId)-1>=m_game.rows()) {return false;}
        for (int i = 0; i < m_game.shipLength(shipId); i++){
            if (grid[topOrLeft.r][topOrLeft.c+i]!='.') {return false;}
        }
        for (int i = 0; i < m_game.shipLength(shipId); i++){
            grid[topOrLeft.r][topOrLeft.c+i]=m_game.shipSymbol(shipId);
        }
    }
    else if (dir==VERTICAL){
        if (topOrLeft.r+m_game.shipLength(shipId)-1>=m_game.cols()) {return false;}
        for (int i = 0; i < m_game.shipLength(shipId); i++){
            if (grid[topOrLeft.r+i][topOrLeft.c]!='.') {return false;}
        }
        //here we change the symbol in the gird to the ship's where it is being placed
        for (int i = 0; i < m_game.shipLength(shipId); i++){
            grid[topOrLeft.r+i][topOrLeft.c]=m_game.shipSymbol(shipId);
        }
    }
    //push the ship onto the vector of ships on the grid
    shipsPlaced temp = {shipId, topOrLeft, m_game.shipLength(shipId)};
    idPlaced.push_back(temp);
    return true;
}
        
    

bool BoardImpl::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
    // in the beginning we check a bunch of points to see if they exist and are there
    bool found=false;
    int index=0;
    shipsPlaced finder;
    for (int i=0; i< idPlaced.size(); i++){
        if (idPlaced.at(i).idNum==shipId){
            found = true;
            finder = idPlaced.at(i);
            index=i;
            break;}
    }
    if (!found) {return false;}

    Point location;
    bool stop = false;
    for (int ro = 0; ro < m_game.rows() && !stop; ro++){
        for (int co = 0; co < m_game.cols() && !stop; co++){
            if (grid[ro][co]==m_game.shipSymbol(shipId)){
                location.r=ro;
                location.c=co;
                stop=true;
            }
        }
    }
    
    if (location.r!=topOrLeft.r || location.c!=topOrLeft.c) {return false;}

    //change the board where the ship was placed to have all the points be '.'
    if (dir==HORIZONTAL){
        for (int i = 0; i < m_game.shipLength(shipId); i++){
            grid[topOrLeft.r][topOrLeft.c+i]='.';
        }
    }
    else if (dir==VERTICAL){
        for (int i = 0; i < m_game.shipLength(shipId); i++){
            grid[topOrLeft.r+i][topOrLeft.c]='.';
        }
    }
    //remove from vector
    idPlaced.erase(idPlaced.begin()+index);
    return true;
}

void BoardImpl::display(bool shotsOnly) const
{
    //displays the board with the correct spacing
    //first line we just have two spaces
    cout << "  ";
    for (int i=0; i< m_game.cols(); i++){
        cout<<i;
    }
    cout<<endl;
    //then after that we start off outputting the row number and all the points in the row
    for(int i=0; i< m_game.rows(); i++){
        cout<< i<< " ";
        for(int j=0; j< m_game.cols(); j++){
            if(!shotsOnly){
                //if its only shots, all the ships will be displayed as '.'
                cout<< grid[i][j];}
            else{
                switch (grid[i][j]){
                    case 'X':
                    case 'o':
                        cout<< grid[i][j];
                        break;
                    default:
                        cout<< '.';
                }
            }
        }
        cout<< endl;
    }
}

bool BoardImpl::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
    shotHit=false;
    shipDestroyed=false;
    //return false if we are wasting a shot
    if (p.r<0 || p.c<0 || p.r>=m_game.rows() || p.c>=m_game.cols()|| grid[p.r][p.c]=='o' || grid[p.r][p.c]=='X') {return false;}

    if (grid[p.r][p.c]!='.'){
        shotHit=true;
        shipsPlaced finder;
        for (int i=0; i< idPlaced.size(); i++){
            char at=grid[p.r][p.c];
            //setting finder to the idPlaced struct that was attacked
            if (m_game.shipSymbol(idPlaced.at(i).idNum)==at){
                idPlaced.at(i).undamaged--;
                //decrement the amount of damage;
                finder = idPlaced.at(i);
                break;}
        }
        grid[p.r][p.c]= 'X';
        //set that point that was a damaged ship segement to an 'X'
        //when there are no more undamaged pieces the ship is destroyed
        if (finder.undamaged<=0) {
            shipDestroyed=true;
            shipId=finder.idNum;
        }
    }
    else {
        //if we miss, the board is set to 'o'
        grid[p.r][p.c]= 'o';
    }
    return true;
}

bool BoardImpl::allShipsDestroyed() const
{
    //loop through the whole board and see if we find a point that is a undamaged ship segment
    for(int i=0; i< m_game.rows(); i++){
        for(int j=0; j< m_game.cols(); j++){
            switch (grid[i][j]){
                case 'X':
                case 'o':
                case '.':
                    break;
                default:
                    return false;
            }
        }
    }
    return true;
}

//******************** Board functions ********************************

// These functions simply delegate to BoardImpl's functions.
// You probably don't want to change any of this code.

Board::Board(const Game& g)
{
    m_impl = new BoardImpl(g);
}

Board::~Board()
{
    delete m_impl;
}

void Board::clear()
{
    m_impl->clear();
}

void Board::block()
{
    return m_impl->block();
}

void Board::unblock()
{
    return m_impl->unblock();
}

bool Board::placeShip(Point topOrLeft, int shipId, Direction dir)
{
    return m_impl->placeShip(topOrLeft, shipId, dir);
}

bool Board::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
    return m_impl->unplaceShip(topOrLeft, shipId, dir);
}

void Board::display(bool shotsOnly) const
{
    m_impl->display(shotsOnly);
}

bool Board::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
    return m_impl->attack(p, shotHit, shipDestroyed, shipId);
}

bool Board::allShipsDestroyed() const
{
    return m_impl->allShipsDestroyed();
}
