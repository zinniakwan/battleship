#include "Player.h"
#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

//*********************************************************************
//  AwfulPlayer
//*********************************************************************

class AwfulPlayer : public Player
{
  public:
    AwfulPlayer(string nm, const Game& g);
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                                bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
  private:
    Point m_lastCellAttacked;
};

AwfulPlayer::AwfulPlayer(string nm, const Game& g)
 : Player(nm, g), m_lastCellAttacked(0, 0)
{}

bool AwfulPlayer::placeShips(Board& b)
{
      // Clustering ships is bad strategy
    for (int k = 0; k < game().nShips(); k++)
        if ( ! b.placeShip(Point(k,0), k, HORIZONTAL))
            return false;
    return true;
}

Point AwfulPlayer::recommendAttack()
{
    if (m_lastCellAttacked.c > 0)
        m_lastCellAttacked.c--;
    else
    {
        m_lastCellAttacked.c = game().cols() - 1;
        if (m_lastCellAttacked.r > 0)
            m_lastCellAttacked.r--;
        else
            m_lastCellAttacked.r = game().rows() - 1;
    }
    return m_lastCellAttacked;
}

void AwfulPlayer::recordAttackResult(Point /* p */, bool /* validShot */,
                                     bool /* shotHit */, bool /* shipDestroyed */,
                                     int /* shipId */)
{
      // AwfulPlayer completely ignores the result of any attack
}

void AwfulPlayer::recordAttackByOpponent(Point /* p */)
{
      // AwfulPlayer completely ignores what the opponent does
}

//*********************************************************************
//  HumanPlayer
//*********************************************************************

bool getLineWithTwoIntegers(int& r, int& c)
{
    bool result(cin >> r >> c);
    if (!result)
        cin.clear();  // clear error state so can do more input operations
    cin.ignore(10000, '\n');
    return result;
}

// TODO:  You need to replace this with a real class declaration and
//        implementation.
class HumanPlayer : public Player
{
  public:
    HumanPlayer(string nm, const Game& g):Player(nm, g){}
    virtual bool isHuman() const { return true; }
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                    bool shipDestroyed, int shipId){}
    virtual void recordAttackByOpponent(Point p){}
private:
    bool isHoV (string s, char & hov);
};

//this function is called to make sure that a human can only enter an h or v
bool HumanPlayer::isHoV (string s, char & hov){
    if (s.size()==0) return false;
        if (s.at(0)=='h'){
            hov='h';
            return true;
        }
        else if (s.at(0)=='v'){
            hov='v';
            return true;
        }
    return false;
}


bool HumanPlayer::placeShips(Board &b){
    //allows the human to place ships
    cout<< name()<< " must place "<< game().nShips() <<" ships." << endl;
    for (int i=0; i <game().nShips(); i++){
        b.display(false);
        string input;
        char hov;
        while (true)
         {
             cout << "Enter h or v for direction of aircraft carrier (length " << game().shipLength(i)<< "): ";
             getline(cin, input);
             if (isHoV(input, hov)) {break;}
           cout << "Direction must be h or v." << endl;
         }
        Direction p=HORIZONTAL;
        switch (hov){
        case 'h':
            p= HORIZONTAL;
            break;
        case 'v':
            p=VERTICAL;
            break;
        };
        
        int ro, co;
        while (true)
         {
             cout << "Enter row and column of leftmost cell (e.g., 3 5): " ;
             while (!getLineWithTwoIntegers(ro, co))
             {
                 cout<< "You must enter two integers."<< endl;
                 cout << "Enter row and column of leftmost cell (e.g., 3 5): " ;
             }
             if (b.placeShip(Point(ro, co), i, p)) {break;}
             //if the ship is not placable, you must continue prompting for input
           cout << "The ship can not be placed there." << endl;
         }
    }
    return true;
}

Point HumanPlayer::recommendAttack(){
    //calls for human to input two points
    int r, c;
    cout << "Enter the row and column to attack (e.g., 3 5): ";
    while (!getLineWithTwoIntegers(r, c))
    {
        cout<< "You must enter two integers."<< endl;
        
        cout << "Enter the row and column to attack (e.g., 3 5): ";
    }
    Point p= {r,c};
    return p;
}


//*********************************************************************
//  MediocrePlayer
//*********************************************************************

// TODO:  You need to replace this with a real class declaration and
//        implementation.
class MediocrePlayer : public Player
{
  public:
    MediocrePlayer(string nm, const Game& g);
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                    bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p){}

  private:
    struct ship{
        int id;
        int m_length;
    };
    void getAllShips(const Game & m_game, vector<ship> & SHIPS);
    bool placeRecursively(Board& b, const Game & g, const vector<ship> SHIPS );
    //this is used for the sort function
    friend bool operator<(const ship &a, const ship &b)
    {
       return (a.m_length < b.m_length);
    }
    bool currentShipDest;
    Point currentAttacking;
    bool validPoint(Point p);
    char trackAttacked[MAXCOLS][MAXROWS];
    void getAllAttacks(Point P);
    vector<Point>possibleAttacks;
//    Point returnRandomPointInSet(Point p);
//    bool hasMoreAttacks();
};



MediocrePlayer::MediocrePlayer(string nm, const Game& g) : Player(nm, g),currentShipDest(true), currentAttacking(0, 0){
    for (int i=0; i<game().rows(); i++){
        for(int j=0; j<game().cols(); j++ ){
            trackAttacked[i][j]='.';
        }
    }
}

//this function loops through every single ship in game, and stores them in the SHIPS vector
void MediocrePlayer::getAllShips(const Game & m_game, vector<ship> & SHIPS){
    for (int i= 0; i < m_game.nShips(); i++){
        ship temp = {i, m_game.shipLength(i)};
    SHIPS.push_back(temp);
    }
    sort( SHIPS.begin( ), SHIPS.end( ) );
}

//this is the recursive function to place all ships after the board is blocked
bool MediocrePlayer::placeRecursively(Board &b ,const Game & g, const vector<ship> SHIPS ){
    if (SHIPS.size()==0) {return true;}//no ships to place anymore
    vector<ship>OneLess=SHIPS;
    OneLess.pop_back();
    for (int i = 0; i < g.rows(); i++){
        for (int j = 0; j < g.cols(); j++){
            Point p(i, j);
            if (b.placeShip(p, SHIPS.back().id, HORIZONTAL)){
                if (placeRecursively(b, g, OneLess)){ return true;}
                else{//if we ened up not being to place other ships, we must remove our current ship so we can try other positions
                    b.unplaceShip(p,SHIPS.back().id, HORIZONTAL);
                }
            }
            else if (b.placeShip(p, SHIPS.back().id, VERTICAL)){
                if (placeRecursively(b, g, OneLess)){ return true;}
                else{
                    b.unplaceShip(p,SHIPS.back().id, VERTICAL);
                }
            }
        }
    }
    return false;
}

bool MediocrePlayer::placeShips(Board &b){
    vector<ship> shipsInOrder;
    getAllShips(game(), shipsInOrder);
    //we try 50 times before returning false
    for (int i=0; i<50; i++){
        b.block();
        if (placeRecursively(b, game(), shipsInOrder)) {
            b.unblock();
            return true;}
        //create a vector that holds all of the things in order from largest to smallest
        b.unblock();
    }
    return false;
}

Point MediocrePlayer::recommendAttack(){
    Point p;
    int pointsToAttack=0;
    if (currentShipDest){//state 1
        //loop through all valid points to attack and count
        for (int i= 0; i < game().rows(); i++){
            for (int j=0; j< game().cols(); j++){
                if (trackAttacked[i][j]=='.') {
                    pointsToAttack++;
                }
            }
        }
        int counter=0;
        int randval=randInt(pointsToAttack);
        //choose a random point to attack out of all the valid points
        for (int i= 0; i < game().rows(); i++){
            for (int j=0; j< game().cols(); j++){
                if (trackAttacked[i][j]=='.') {
                    if (counter==randval){
                        return Point(i, j);
                    }
                    counter++;
                }
            }
        }
    }
    else {
        //if we are in state two, we randomly select from our vectors of possible attacks
            int rand= randInt(possibleAttacks.size());
            p=possibleAttacks[rand];
        //return that point, and erase that point from possible attacks vector
            possibleAttacks.erase (possibleAttacks.begin()+rand);
    }
    return p;
}
    
void MediocrePlayer::recordAttackResult(Point p, bool validShot, bool shotHit,
                                bool shipDestroyed, int shipId){
    if (validShot){
        trackAttacked[p.r][p.c]='X';
        //update track attacked when a shot is hit
        if (shotHit && !shipDestroyed && currentShipDest==true){
            //enter state two and store our original point
            currentAttacking=p;
            getAllAttacks(currentAttacking);
            //get all possible attacks in the cross in the possibleAttacks vector
            currentShipDest=false;
        }
        if(shotHit && shipDestroyed && currentShipDest==false){
            //if the ship is destroyed switch to state 1 and clear the possibleAttacks vector
            possibleAttacks.clear();
            currentShipDest=true;
        }
    }
    if (possibleAttacks.size()==0){
        //switch to state one of there are no more possible attacks in state 2
        currentShipDest=true;
    }
}


void MediocrePlayer::getAllAttacks(Point P){
    Point temp;
    //travel in the cross from the original point we attacked, push onto vector if it is a valid point
        int r, c;
        for (int i = 0; i < 16; i++){
            switch (i/4){
                case 0:
                    c=currentAttacking.c;
                    r=currentAttacking.r+1+(i%4);
                    break;
                case 1:
                    r=currentAttacking.r;
                    c=currentAttacking.c+1+(i%4);
                    break;
                case 2:
                    c=currentAttacking.c;
                    r=currentAttacking.r-1-(i%4);
                    break;
                case 3:
                    r=currentAttacking.r;
                    c=currentAttacking.c-1-(i%4);
                    break;
            };
            temp.r=r;
            temp.c=c;
            if (validPoint(temp)){
                possibleAttacks.push_back(temp);
            }
        }
    }

bool MediocrePlayer::validPoint(Point p){
    //if point is off board
    if (p.c<0 || p.r <0 || p.c>=game().cols() || p.r>=game().rows()){
        return false;
    }
    //if point is already hit
    if (trackAttacked[p.r][p.c]=='X') {return false;}
    return true;
}



// Remember that Mediocre::placeShips(Board& b) must start by calling
// b.block(), and must call b.unblock() just before returning.

//*********************************************************************
//  GoodPlayer
//*********************************************************************

// TODO:  You need to replace this with a real class declaration and
//        implementation.
class GoodPlayer : public Player
{
  public:
    GoodPlayer(string nm, const Game& g);
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                    bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p){}

  private:
    int rows, cols;
    vector<int> ShipIdsToFind;
    bool currentShipDest;
    Point currentAttacking;
    bool placable(Point p, Direction d, int shipId);
    bool validPoint(Point p);
    char trackAttacked[MAXCOLS][MAXROWS];
    vector<int> probability;
    void clearProb();
    void FindProb();    enum Dir {undecided, NORTH, EAST, SOUTH, WEST};
    Dir d;
    void removeAts(const Point & p, Dir d, int shipId);
    bool getPointToAttack(Point & p);
    
    struct ship{
        int id;
        int m_length;
    };
    //this is used for the sort function
    friend bool operator<(const ship &a, const ship &b)
    {
       return (a.m_length < b.m_length);
    }
    void getAllShips(const Game & m_game, vector<ship> & SHIPS);
    bool placeRecursively(Board &b ,const Game & g, const vector<ship> SHIPS );
};

GoodPlayer::GoodPlayer(string nm, const Game& g) : Player(nm, g),currentShipDest(true), currentAttacking(0, 0), d(undecided){
    cols=game().cols();
    rows=game().rows();
    for (int i=0; i<game().rows(); i++){
        for(int j=0; j<game().cols(); j++ ){
            trackAttacked[i][j]='.';
        }
    }
    for (int i=0; i<game().rows()*game().cols(); i++){
        probability.push_back(0);
    }
    for (int i= 0; i < game().nShips(); i++){
        ShipIdsToFind.push_back(i);
    }
}

void GoodPlayer::clearProb(){
    //clears our probaility vector at beginning of each round
    vector<int>::iterator it;
    it = probability.begin();
    while (it!=probability.end()){
        *it=0;
        it++;
    }
}


bool GoodPlayer::placable(Point p, Direction d, int shipId){
    //checks if it is valid to place a ship here
    //it is valid if somewhere in the board is an '@'
    //@'s represent parts of damaged ships that have not been sunken yet
    if (p.r<0 || p.c<0 || p.r>=rows || p.c>=cols) {return false;}
    if (d==HORIZONTAL){
        if (p.c+game().shipLength(shipId)-1>=rows) {return false;}
        for (int i = 0; i < game().shipLength(shipId); i++){
            if (trackAttacked[p.r][p.c+i]=='X') {return false;}
        }
    }
    else if (d==VERTICAL){
        if (p.r+game().shipLength(shipId)-1>=cols) {return false;}
        for (int i = 0; i < game().shipLength(shipId); i++){
            if (trackAttacked[p.r+i][p.c]=='X') {return false;}
        }
    }
    return true;
}

void GoodPlayer::FindProb(){
    //loops through every single point in the vector
    //if it is possible to place the ship there, we increment that point's probability in the probability vector
    for (int s=0; s< ShipIdsToFind.size(); s++){
        for(int i = 0; i < rows; i++){
        for (int j= 0; j < cols; j++){
            Point p(i, j);
            if (trackAttacked[i][j]=='X'){continue;}
            if(placable(p, HORIZONTAL, ShipIdsToFind[s])){
                //changed
                for (int t = 0; t<game().shipLength(ShipIdsToFind[s]); t++){
                    int vecPos=(i*cols+j+t);
                    if(trackAttacked[i][j+t]!='@'){
                    probability[vecPos]++;
                    }
                }
            }
            if(placable(p, VERTICAL, ShipIdsToFind[s])){
                for (int t = 0; t<game().shipLength(ShipIdsToFind[s]); t++){
                    int vecPos=((i+t)*cols+j);
                    if(trackAttacked[i+t][j]!='@'){
                    probability[vecPos]++;
                    }
                }
            }
        }
    }
}
}

bool GoodPlayer::getPointToAttack(Point &p){
    //if we are in state 2, we keep travelling in the direction and return the first valid point
    //if we hit an invalid point, such as the border or one that we already hit, we switch directions and continue on
    if (d==NORTH){
        while(true){
            p.r--;
            if (!validPoint(p)){
                d=SOUTH;
                Point p(currentAttacking.r,currentAttacking.c);
                break;
            }
            if(trackAttacked[p.r][p.c]=='.'){return true;}
        }
    }
    if (d==SOUTH){
        while(true){
            p.r++;
            if (!validPoint(p)){
                d=EAST;
                Point p(currentAttacking.r,currentAttacking.c);
                break;
            }
            if(trackAttacked[p.r][p.c]=='.'){return true;}
        }
    }
    if (d==EAST){
        while(true){
            p.c++;
            if (!validPoint(p)){
                d=WEST;
                Point p(currentAttacking.r,currentAttacking.c);
                break;
            }
            if(trackAttacked[p.r][p.c]=='.'){return true;}
        }
    }

    if (d==WEST){
        while(true){
            p.c--;
            if (!validPoint(p)){
                d=undecided;
                currentShipDest=true;
                Point p(currentAttacking.r,currentAttacking.c);
                break;
            }
            if(trackAttacked[p.r][p.c]=='.'){return true;}
        }
    }
    return false;
}

Point GoodPlayer::recommendAttack(){
    //if we are in state two, we reccomend a poinnt to attack
    Point p(currentAttacking.r,currentAttacking.c);
    if (!currentShipDest){
        if (getPointToAttack(p)){
            return p;
        }
    }
    clearProb();
    FindProb();
    vector<int>::iterator it;
    //if we are in state 1, we calculate the probaibility of every point, then return the point with the max probability
    it = max_element(probability.begin(), probability.end());
    if (*it==0){
        int pointsToAttack=0;
        if (currentShipDest){
            for (int i= 0; i < game().rows(); i++){
                for (int j=0; j< game().cols(); j++){
                    if (trackAttacked[i][j]=='.') {
                        pointsToAttack++;
                    }
                }
            }
            int counter=0;
            int randval=randInt(pointsToAttack);
            for (int i= 0; i < game().rows(); i++){
                for (int j=0; j< game().cols(); j++){
                    if (trackAttacked[i][j]=='.') {
                        if (counter==randval){
                            return Point(i, j);
                        }
                        counter++;
                    }
                }
            }
        }
    }
    int vecPos= it-probability.begin();
    return Point (vecPos/cols, vecPos%cols);
}


void GoodPlayer::recordAttackResult(Point p, bool validShot, bool shotHit,
                                    bool shipDestroyed, int shipId){
    //if we hit a shot and missed
    if(validShot && !shotHit){
        trackAttacked[p.r][p.c]='X';}
    if(shotHit && validShot){
        //if we hit a part of a ship
        trackAttacked[p.r][p.c]='@';
    }
    if (shotHit && !shipDestroyed && currentShipDest){
        //enter state 2, record the first point hit
            currentAttacking=p;
            currentShipDest=false;
            d=NORTH;
        }
    if(shotHit && shipDestroyed && !currentShipDest){
        //we switch back to state one
            currentShipDest=true;
            //the following lines of code help us turn the @'s into X, for the @'s that were part of the sunken ship
            removeAts(p, d, shipId);
            d=undecided;    //maybe wrong we shall see;
        ShipIdsToFind.erase(find(ShipIdsToFind.begin(), ShipIdsToFind.end(), shipId));
        }

    if ( !currentShipDest && !shotHit ){
        //if the shot misses in state 2, automatically switch to the next direction
        if (d==NORTH){
            d=SOUTH;
        }
        else if (d==EAST){
            d=WEST;
        }
        else if (d==SOUTH){
            d=EAST;
        }
        else if (d==WEST){
            d=undecided;
            currentShipDest=true;
        }
    }
}


bool GoodPlayer::validPoint(Point p){
    //if point is off board
    if (p.c<0 || p.r <0 || p.c>=game().cols() || p.r>=game().rows()){
        return false;
    }
    //if point is already hit
    if (trackAttacked[p.r][p.c]=='X') {return false;}
    return true;
}

bool GoodPlayer::placeShips(Board &b){
    //same as mediocre, except we loop 300 times
    vector<ship> shipsInOrder;
    getAllShips(game(), shipsInOrder);
    for (int i=0; i<300; i++){
        b.block();
        if (placeRecursively(b, game(), shipsInOrder)) {
            b.unblock();
        return true;}
        b.unblock();
    }
    //if we loop 300 times, just place without blocking
    if (placeRecursively(b, game(), shipsInOrder)) {
        return true;}
    return false;
}

void GoodPlayer::getAllShips(const Game & m_game, vector<ship> & SHIPS){
    //same as mediocre
    for (int i= 0; i < m_game.nShips(); i++){
        ship temp = {i, m_game.shipLength(i)};
    SHIPS.push_back(temp);
    }
    sort( SHIPS.begin( ), SHIPS.end( ) );
}

//same as mediocre
bool GoodPlayer::placeRecursively(Board &b ,const Game & g, const vector<ship> SHIPS ){
    if (SHIPS.size()==0) {return true;}//no ships to place anymore
    vector<ship>OneLess=SHIPS;
    OneLess.pop_back();
    for (int i = 0; i < g.rows(); i++){
        for (int j = 0; j < g.cols(); j++){
            Point p(i, j);
            if (b.placeShip(p, SHIPS.back().id, HORIZONTAL)){
                if (placeRecursively(b, g, OneLess)){ return true;}
                else{
                    b.unplaceShip(p,SHIPS.back().id, HORIZONTAL);
                }
            }
            else if (b.placeShip(p, SHIPS.back().id, VERTICAL)){
                if (placeRecursively(b, g, OneLess)){ return true;}
                else{
                    b.unplaceShip(p,SHIPS.back().id, VERTICAL);
                }
            }
        }
    }
    return false;
}

void GoodPlayer::removeAts(const Point & p, Dir d, int shipId){
    //removes @'s but only for the sunken ship, not for the ones that were part of a different ship
    //check psuedocode for deeper understanding
    if (d==NORTH){
        Point temp = p;
        int counter=0;
        while(counter<game().shipLength(shipId)){
            if(temp.r==currentAttacking.r){
                trackAttacked[temp.r][temp.c]='X';
                counter++;
                break;}
            trackAttacked[temp.r][temp.c]='X';
            temp.r++;
            //moving temp foward
            counter++;
        }
        int atUp(0), atDown(0);
        //count the amount of @'s above and below
        for (int i=1; validPoint(Point (p.r-i, p.c)) ;i++){
            if (trackAttacked[p.r-i][p.c]=='@'){atUp++;}
            else {break;}
        }
        for (int i=1; validPoint(Point (currentAttacking.r+i,currentAttacking.c)) ;i++){
            if (trackAttacked[currentAttacking.r+i][currentAttacking.c]=='@'){atDown++;}
            else {break;}
        }
        
        if ((game().shipLength(shipId)-counter)==0 || (game().shipLength(shipId)-(atUp+atDown)-counter)<0){}
        //we don't want to change the @'s above and below to 'X' if we are unsure if they belonged to the ship destroyed
        else if ((game().shipLength(shipId)-(atUp+atDown)-counter)>=0){
            for (int i=0; i<atUp; i++){
                trackAttacked[p.r-1-i][p.c]='X';
            }
            for (int i=0; i < atDown ;i++){
                trackAttacked[currentAttacking.r+1+i][currentAttacking.c]='X';
            }
        }
    }
    //repeat for all other directions
    else if (d==SOUTH){
        Point temp = p;
        int counter=0;
        while(counter<game().shipLength(shipId)){
            if(temp.r==currentAttacking.r){
                trackAttacked[temp.r][temp.c]='X';
                counter++;
                break;}
            trackAttacked[temp.r][temp.c]='X';
            temp.r--;
            //moving temp foward
            counter++;

        }
        int atUp(0), atDown(0);
        for (int i=1; validPoint(Point(p.r+i,p.c)) ;i++){
            if (trackAttacked[p.r+i][p.c]=='@'){atDown++;}
            else {break;}
        }
        for (int i=1; validPoint(Point(currentAttacking.r-1,currentAttacking.c)) ;i++){
            if (trackAttacked[currentAttacking.r-i][currentAttacking.c]=='@'){atUp++;}
            else {break;}
        }
        
        if ((game().shipLength(shipId)-counter)==0 || (game().shipLength(shipId)-(atUp+atDown)-counter)<0){}
        else if ((game().shipLength(shipId)-(atUp+atDown)-counter)>=0){
            for (int i=0; i<atDown; i++){
                trackAttacked[p.r+1+i][p.c]='X';
            }
            for (int i=0; i < atUp ;i++){
                trackAttacked[currentAttacking.r-1-i][currentAttacking.c]='X';
            }
        }
    }
    else if (d==EAST){
        Point temp = p;
        int counter=0;
        while(counter<game().shipLength(shipId)){
            if(temp.c==currentAttacking.c){
                trackAttacked[temp.r][temp.c]='X';
                counter++;
                break;}
            trackAttacked[temp.r][temp.c]='X';
            temp.c--;
            //moving temp foward
            counter++;

        }
        int atLeft(0), atRight(0);
        for (int i=1; validPoint(Point (p.r, p.c+i));i++){
            if (trackAttacked[p.r][p.c+i]=='@'){atRight++;}
            else {break;}
        }
        for (int i=1; validPoint(Point(currentAttacking.r, currentAttacking.c-1)) ;i++){
            if (trackAttacked[currentAttacking.r][currentAttacking.c-i]=='@'){atLeft++;}
            else {break;}
        }
        
        if ((game().shipLength(shipId)-counter)==0 || (game().shipLength(shipId)-(atLeft+atRight)-counter)<0){}
        else if ((game().shipLength(shipId)-(atLeft+atRight)-counter)>=0){
            for (int i=0; i<atRight; i++){
                trackAttacked[p.r][p.c+1+i]='X';
            }
            for (int i=0; i < atLeft ;i++){
                trackAttacked[currentAttacking.r][currentAttacking.c-1-i]='X';
            }
        }
    }
    else if (d==WEST){
        Point temp = p;
        int counter=0;
        while(counter<game().shipLength(shipId)){
            if(temp.c==currentAttacking.c){
                trackAttacked[temp.r][temp.c]='X';
                counter++;
                break;}
            trackAttacked[temp.r][temp.c]='X';
            temp.c++;
            //moving temp foward
            counter++;
        }
        int atLeft(0), atRight(0);
        for (int i=1; validPoint(Point (p.r, p.c-i));i++){
            if (trackAttacked[p.r][p.c-i]=='@'){atLeft++;}
            else {break;}
        }
        for (int i=1;validPoint(Point(currentAttacking.r,currentAttacking.c+i)) ;i++){
            if (trackAttacked[currentAttacking.r][currentAttacking.c+i]=='@'){atRight++;}
            else {break;}
        }
        
        if ((game().shipLength(shipId)-counter)==0 || (game().shipLength(shipId)-(atLeft+atRight)-counter)<0){}
        else if ((game().shipLength(shipId)-(atLeft+atRight)-counter)>=0){
            for (int i=0; i<atLeft; i++){
                trackAttacked[p.r][p.c-1-i]='X';
            }
            for (int i=0; i < atRight ;i++){
                trackAttacked[currentAttacking.r][currentAttacking.c+1+i]='X';
            }
        }
    }
}

    
//*********************************************************************
//  createPlayer
//*********************************************************************

Player* createPlayer(string type, string nm, const Game& g)
{
    static string types[] = {
        "human", "awful", "mediocre", "good"
    };
    
    int pos;
    for (pos = 0; pos != sizeof(types)/sizeof(types[0])  &&
                                                     type != types[pos]; pos++)
        ;
    switch (pos)
    {
      case 0:  return new HumanPlayer(nm, g);
      case 1:  return new AwfulPlayer(nm, g);
      case 2:  return new MediocrePlayer(nm, g);
      case 3:  return new GoodPlayer(nm, g);
      default: return nullptr;
    }
}
