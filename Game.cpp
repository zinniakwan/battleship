#include "Game.h"
#include "Board.h"
#include "Player.h"
#include "globals.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>
#include <vector>
#include <algorithm>

using namespace std;


class GameImpl
{
  public:
    GameImpl(int nRows, int nCols);
    int rows() const;
    int cols() const;
    bool isValid(Point p) const;
    Point randomPoint() const;
    bool addShip(int length, char symbol, string name);
    int nShips() const;
    int shipLength(int shipId) const;
    char shipSymbol(int shipId) const;
    string shipName(int shipId) const;
    Player* play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause);
private:
    string actionName( bool shotHit, bool shipDestroyed, int shipId);
    struct ship{
        int m_length;
        char m_symbol;
        string m_name;
    };
    vector<ship>shipStore;
    int g_rows;
    int g_cols;
};

void waitForEnter()
{
    cout << "Press enter to continue: ";
    cin.ignore(10000, '\n');
}

GameImpl::GameImpl(int nRows, int nCols) 
{
    if (nRows>MAXROWS||nCols>MAXCOLS) {exit(0);}
    g_rows=nRows;
    g_cols=nCols;
}

int GameImpl::rows() const
{
    return g_rows;
}

int GameImpl::cols() const
{
    return g_cols;
}

bool GameImpl::isValid(Point p) const
{
    return p.r >= 0  &&  p.r < rows()  &&  p.c >= 0  &&  p.c < cols();
}

Point GameImpl::randomPoint() const
{
    return Point(randInt(rows()), randInt(cols()));
}

bool GameImpl::addShip(int length, char symbol, string name)
{
    //start off by checking if it is valid to add this ship
    if (!(length<=rows()&&length<=cols())){return false;}
    if (symbol=='X' || symbol == 'o' || symbol=='.') return false;
    for (int i=0; i<shipStore.size(); i++){
        if (symbol==shipStore.at(i).m_symbol) return false;
    }
    
    //add this ship into the vector shipStore
    ship temp = {length, symbol, name};
    shipStore.push_back(temp);
    
    return true;
}

int GameImpl::nShips() const
{
    return shipStore.size();
}

int GameImpl::shipLength(int shipId) const
{
    return shipStore.at(shipId).m_length;
}

char GameImpl::shipSymbol(int shipId) const
{
    return shipStore.at(shipId).m_symbol;
}

string GameImpl::shipName(int shipId) const
{
    return shipStore.at(shipId).m_name; 
}


string GameImpl::actionName( bool shotHit, bool shipDestroyed, int shipId){
    //this function is called by play and returns the correct action string depending on the result of the attack
    if (!shotHit) {return "missed";}
    if (!shipDestroyed) {return "hit something";}
    return ("destroyed the "+shipName(shipId));
}

Player* GameImpl::play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause)
{
    if (!p1->placeShips(b1)||!p2->placeShips(b2)){
        return nullptr;
    }
    bool p1viewShotsOnly = p1->isHuman();
    bool p2viewShotsOnly = p2->isHuman();
    //these bools record whether players are human
    int i=0;
    while (!(b1.allShipsDestroyed() || b2.allShipsDestroyed())){
        //the bools help switch turns for each player
        if (i%2==0){
            cout<< p1->name() <<"s turn.  Board for "<< p2->name()<< ":" << endl;
            b2.display(p1viewShotsOnly);
            Point p = p1->recommendAttack();
            bool shotHit, shipDestroyed, validShot;
            int shipId;
            if (!b2.attack(p, shotHit,shipDestroyed, shipId)) {
                validShot=false;
                cout << p1->name() << " wasted a shot at ("  << p.r << "," << p.c << ")." << endl;
            }
            else {
                validShot=true;
                string action= actionName(shotHit, shipDestroyed, shipId);
                cout<< p1->name() << " attacked (" << p.r << "," << p.c << ") and " << action << ", resulting in:" << endl;
                b2.display(p1viewShotsOnly);
            }
            p1->recordAttackResult(p, validShot , shotHit, shipDestroyed, shipId);
            p2->recordAttackByOpponent(p);

        }
        else{
            cout<< p2->name() <<"s turn.  Board for "<< p1->name()<< ":" << endl;
            b1.display(p2viewShotsOnly);
            Point p = p2->recommendAttack();
            bool shotHit, shipDestroyed, validShot;
            int shipId;
            if (!b1.attack(p, shotHit,shipDestroyed, shipId)) {
                validShot=false;
                cout << p2->name() << " wasted a shot at ("  << p.r << "," << p.c << ")." << endl;
            }
            else {
                validShot=true;
                string action= actionName(shotHit, shipDestroyed, shipId);
                cout<< p2->name() << " attacked (" << p.r << "," << p.c << ") and " << action << ", resulting in:" << endl;
                b1.display(p2viewShotsOnly);
            }
            p2->recordAttackResult(p, validShot , shotHit, shipDestroyed, shipId);
            p1->recordAttackByOpponent(p);

        }
        //you need to get the player to press enter to continue
        if (shouldPause&& !(b1.allShipsDestroyed() || b2.allShipsDestroyed())){
            waitForEnter();
        }
        i++;
    }
    
    Player * winner= (b1.allShipsDestroyed()? p2 : p1);
    cout << winner->name()<< " wins!"<< endl;
    
    //if the loser is a human, we display their opponent's board
    if (b1.allShipsDestroyed()){
        if (p1->isHuman()){
            cout<<"Here is where "<< p2->name() <<"'s ships were:"<< endl;
            b2.display(false);
        }
    }
    else {
        if (p2->isHuman()){
            cout<<"Here is where "<< p1->name() <<"'s ships were:"<< endl;
            b1.display(false);
        }
    }
    return winner;
}

//******************** Game functions *******************************

// These functions for the most part simply delegate to GameImpl's functions.
// You probably don't want to change any of the code from this point down.

Game::Game(int nRows, int nCols)
{
    if (nRows < 1  ||  nRows > MAXROWS)
    {
        cout << "Number of rows must be >= 1 and <= " << MAXROWS << endl;
        exit(1);
    }
    if (nCols < 1  ||  nCols > MAXCOLS)
    {
        cout << "Number of columns must be >= 1 and <= " << MAXCOLS << endl;
        exit(1);
    }
    m_impl = new GameImpl(nRows, nCols);
}

Game::~Game()
{
    delete m_impl;
}

int Game::rows() const
{
    return m_impl->rows();
}

int Game::cols() const
{
    return m_impl->cols();
}

bool Game::isValid(Point p) const
{
    return m_impl->isValid(p);
}

Point Game::randomPoint() const
{
    return m_impl->randomPoint();
}

bool Game::addShip(int length, char symbol, string name)
{
    if (length < 1)
    {
        cout << "Bad ship length " << length << "; it must be >= 1" << endl;
        return false;
    }
    if (length > rows()  &&  length > cols())
    {
        cout << "Bad ship length " << length << "; it won't fit on the board"
             << endl;
        return false;
    }
    if (!isascii(symbol)  ||  !isprint(symbol))
    {
        cout << "Unprintable character with decimal value " << symbol
             << " must not be used as a ship symbol" << endl;
        return false;
    }
    if (symbol == 'X'  ||  symbol == '.'  ||  symbol == 'o')
    {
        cout << "Character " << symbol << " must not be used as a ship symbol"
             << endl;
        return false;
    }
    int totalOfLengths = 0;
    for (int s = 0; s < nShips(); s++)
    {
        totalOfLengths += shipLength(s);
        if (shipSymbol(s) == symbol)
        {
            cout << "Ship symbol " << symbol
                 << " must not be used for more than one ship" << endl;
            return false;
        }
    }
    if (totalOfLengths + length > rows() * cols())
    {
        cout << "Board is too small to fit all ships" << endl;
        return false;
    }
    return m_impl->addShip(length, symbol, name);
}

int Game::nShips() const
{
    return m_impl->nShips();
}

int Game::shipLength(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipLength(shipId);
}

char Game::shipSymbol(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipSymbol(shipId);
}

string Game::shipName(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipName(shipId);
}

Player* Game::play(Player* p1, Player* p2, bool shouldPause)
{
    if (p1 == nullptr  ||  p2 == nullptr  ||  nShips() == 0)
        return nullptr;
    Board b1(*this);
    Board b2(*this);
    return m_impl->play(p1, p2, b1, b2, shouldPause);
}

