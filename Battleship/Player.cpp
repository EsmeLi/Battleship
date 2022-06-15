#include "Player.h"
#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>
#include <string>
#include <cmath>
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
    {
        if ( ! b.placeShip(Point(k,0), k, HORIZONTAL))
        {
            return false;
        }
    }
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
    HumanPlayer(string nm, const Game& g);
    virtual bool isHuman() const { return true; }
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
    
};

HumanPlayer::HumanPlayer(string nm, const Game& g):Player(nm, g)
{}

bool HumanPlayer::placeShips(Board& b)
{
    cout << name() << " must place " << game().nShips() << " ships." << endl;
    b.display(false);

    Direction dir = HORIZONTAL;
    for (int i=0; i<game().nShips(); i++)
    {
        while (true)
        {
            string d;
            cout << "Enter h or v for direction of " << game().shipName(i) << " (length" <<  game().shipLength(i) << "): ";
            cin >> d;
            if (d == "h")
            {
                dir = HORIZONTAL;
                break;
            }
            else if (d== "v")
            {
                dir = VERTICAL;
                break;
            }
            else
                cout << "Direction must be h or v." << endl;
        }

        while (true)
        {
            if (dir == HORIZONTAL)
            {
                int r,c;
                cout << "Enter row and column of leftmost cell (e.g., 3 5): ";
                cin >> r >> c;
                Point p = Point(r, c);
                if (b.placeShip(p, i, dir))
                {
                    b.display(false);
                    break;
                }
                else
                {
                    cout << "The ship can not be placed there." << endl;
                }
            }
            else if (dir == VERTICAL)
            {
                int r, c;
                cout << "Enter row and column of topmost cell (e.g., 3 5): ";
                cin >> r >> c;
                Point p = Point(r, c);
                if (b.placeShip(p, i, dir))
                {
                    b.display(false);
                    break;
                }
                else
                {
                    cout << "The ship can not be placed there." << endl;
                }
            }
        }
    }
    return true;
}

Point HumanPlayer::recommendAttack()
{
    int r,c;
    cout << "Enter the row and column to attack (e.g., 3 5): ";
    cin >> r >> c;
    return Point(r,c);
}

void HumanPlayer::recordAttackResult(Point p, bool validShot, bool shotHit,bool shipDestroyed, int shipId)
{
}

void HumanPlayer::recordAttackByOpponent(Point p)
{
}

//*********************************************************************
//  MediocrePlayer
//*********************************************************************

// TODO:  You need to replace this with a real class declaration and
//        implementation.
// Remember that Mediocre::placeShips(Board& b) must start by calling
// b.block(), and must call b.unblock() just before returning.

class MediocrePlayer : public Player
{
  public:
    MediocrePlayer(string nm, const Game& g);
    bool configExist(Board& b, int shipN);
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
    
  private:
    vector<double> pointVec;
    vector<Point> unAttacked;
    Point lastHit;
    int state;
};

MediocrePlayer::MediocrePlayer(string nm, const Game& g):Player(nm, g), state(1)
{
    // initialize a vector including all points on board, because they are all unattacked at first
    for (int i=0; i<game().rows(); i++)
    {
        for (int j=0; j<game().cols(); j++)
        {
            unAttacked.push_back(Point(i, j));
        }
    }
}

// a recursive function that determine whether there is ship placement possible on given board
// called later in placeShip
bool MediocrePlayer::configExist(Board& b, int shipN)
{
    for (int i=0; i<game().rows(); i++)
    {
        for (int j=0; j<game().cols(); j++)
        {
            Point p = Point(i, j);
            if (b.placeShip(p, shipN, HORIZONTAL))
            {
                // base case, when we successfully place the last ship horizontally, the whole function return true
                if (shipN == 0)
                    return true;
                
                // see if we can place the next ship on board
                bool result = configExist(b, shipN-1);
                
                // if later ships cannot be placed, unplace the current ship
                if (!result)
                    b.unplaceShip(p, shipN, HORIZONTAL);
                return result;
            }
            
            // if attempt above with horizontal direction failed, we will try vertical;
            // if both direction failed, we will return false
            if (b.placeShip(p, shipN, VERTICAL))
            {
                if (shipN == 0)
                    return true;
                bool result = configExist(b, shipN-1);
                if (!result)
                    b.unplaceShip(p, shipN, VERTICAL);
                return result;
            }
        }
    }
    return false;
}

// place ship by first blocking half of the board and the unblock it after successfully placed
bool MediocrePlayer::placeShips(Board& b)
{
    for (int i=0; i<50; i++)
    {
        b.block();
        if (configExist(b, game().nShips()-1))
        {
            b.unblock();
            return true;
            break;
        }
        b.clear();
    }
    return false;
}

// this function record the result of each attack so our player know which state he is in
void MediocrePlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId)
{
    if (validShot)
    {
        if (shotHit)
        {
            // if any ship was destroyed, immediately go back to state 1
            if (shipDestroyed)
                state = 1;
            // if we hit something but do not destroy anything, we are in state 2, regardless of current state
            
            else
            {
                if(state == 1)
                    lastHit = p;
                
                // if we are switching from state 1 to state 2, we want to record the point we last hit so we know what points are available for us to we can to choose from in state 2
                state = 2;
            }
        }
    }
}

// this function return a point that the mediocre player will attack by certain strategy
Point MediocrePlayer::recommendAttack()
{
    if (state == 2)
    {
        // if we are in state 2, we will choose random points from unattacked points on the board
        // I did so by choosing random index in unAttacked
        while(true)
        {
            int rand = randInt((int)unAttacked.size());
            Point p = unAttacked[rand];
            
            vector<double>::iterator iter = find (pointVec.begin(), pointVec.end(), p.r*game().cols() + p.c);
            
            // if p is not attacked before, and p is within the +-4 range of our last hit, we will return p after push back p to pointVec and delete p from unAttacked, since it is now attacked
            if (iter == pointVec.end() && ((abs(p.r-lastHit.r) <= 4 && p.c == lastHit.c) || (abs(p.c-lastHit.c)<=4 && p.r == lastHit.r)))
            {
                pointVec.push_back(p.r*game().cols() + p.c);
                unAttacked.erase(unAttacked.begin()+rand);
                return p;
                break;
            }
        }
        
        int tryCount = 0;
        int order = 0;
        
        // this loops counts how many tries we have done in state 2
        // we will go back to state 1 after we have attacked all points in the +-4 "cross" of lasthit and we still did not destroy any ship because some ship has length longer than 5
        for (int i=0; i<16; i++)
        {
            if (i%4==0 && i!=0)
                order++;
            Point p;
            
            if (order==0)
                p = Point(lastHit.r+i%4+1, lastHit.c);
            else if (order==1)
                p = Point(lastHit.r-i%4-1, lastHit.c);
            else if (order==2)
                p = Point(lastHit.r, lastHit.c+i%4+1);
            else if (order==3)
                p = Point(lastHit.r, lastHit.c-i%4-1);
            
            vector<double>::iterator iter = find (pointVec.begin(), pointVec.end(), p.r*game().cols() + p.c);

            // we count both invalid points and attacked points as a "try"
            if (!game().isValid(p) || iter != pointVec.end())
            {
                tryCount++;
            }
        }
        
        // if we have tried all 16 points, go back to state 1
        if (tryCount==16)
        {
            state = 1;
        }
        
    }
    
    if (state == 1)
    {
        // in state 1, we choose random unattacked point and return it if it has not been attacked before
        while(true)
        {
            int rand = randInt((int)unAttacked.size());
            Point p = unAttacked[rand];
            
            vector<double>::iterator iter = find (pointVec.begin(), pointVec.end(), p.r*game().cols() + p.c);
            
            // if p is unattacked, attack p now
            if (iter == pointVec.end())
            {
                // push p back to attacked vector pointVec and delete p in unAttacked
                pointVec.push_back(p.r*game().cols() + p.c);
                unAttacked.erase(unAttacked.begin()+rand);
                return p;
                break;
            }
        }
    
    }
    return Point();
}

void MediocrePlayer::recordAttackByOpponent(Point p)
{}

//*********************************************************************
//  GoodPlayer
//*********************************************************************

// here is the constructor of a good player
// Point firstHit and lastHit are used to keep track of which point we want our recommendation to start with
// fake_board is a 2d array GoodPlayer use to record its valid attack and destroy result
// alreadyAttack is a vector with double computing by p.r*cols+p.c of a attacked point p
// length is a vector with int recording all length of existing ship on the board
class GoodPlayer : public Player
{
  public:
    GoodPlayer(string nm, const Game& g);
    bool configExist(Board& b, int shipN);
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
  private:
    int state, rowIter, colIter, direction;
    Point firstHit, lastHit;
    char fake_board [MAXROWS][MAXCOLS];
    vector<double> alreadyAttack;
    vector<int> length;
};

GoodPlayer::GoodPlayer(string nm, const Game& g):Player(nm, g)
{
    state = 1;
    direction = 0;
    
    // push back length of each ship to vector length
    for (int i=0; i<game().nShips(); i++)
    {
        length.push_back(game().shipLength(i));
    }
    
    rowIter = colIter = 0;
    
    // initialize a fake board with all dots
    for (int i=0; i<MAXROWS; i++)
    {
        for (int j=0; j<MAXCOLS; j++)
            fake_board[i][j] = '.';
    }
}

// same function with the one in MediocrePlayer, return true if ships are succesfully placed; false otherwise
bool GoodPlayer::configExist(Board& b, int shipN)
{
    for (int i=0; i<game().rows(); i++)
    {
        for (int j=0; j<game().cols(); j++)
        {
            Point p = Point(i, j);
            if (b.placeShip(p, shipN, HORIZONTAL))
            {
                if (shipN == 0)
                    return true;
                bool result = configExist(b, shipN-1);
                if (!result)
                    b.unplaceShip(p, shipN, HORIZONTAL);
                return result;
            }
            
            
            if (b.placeShip(p, shipN, VERTICAL))
            {
                if (shipN == 0)
                    return true;
                bool result = configExist(b, shipN-1);
                if (!result)
                    b.unplaceShip(p, shipN, VERTICAL);
                return result;
            }
        }
    }
    return false;
}

// similar placeShips strategy as MediocrePlayer, but will try placing ship without blocking any spot on board to see if any placement possible after 50 failure with first blocking
// only return false if no placement possible at all
bool GoodPlayer::placeShips(Board& b)
{
    for (int i=0; i<50; i++)
    {
        b.block();
        if (configExist(b, game().nShips()-1))
        {
            b.unblock();
            return true;
            break;
        }
        b.clear();
    }
    if (configExist(b, game().nShips()-1))
        return true;
    return false;
}

Point GoodPlayer::recommendAttack()
{
    Point p;
    bool needRand = false;
    bool found = false;
    
    // in state 3, we keep going with a found direction until fail
    // if failed (invalid point or attacked point), go back to state 2
    if (state == 3)
    {
        if (direction <0 || direction >3)
            needRand = true;
        else
        {
            needRand = false;
            found = false;
            while (true)
            {
                if (direction == 0)
                    p = Point(lastHit.r, lastHit.c-1);
                else if (direction == 1)
                    p = Point(lastHit.r, lastHit.c+1);
                else if (direction == 2)
                    p = Point(lastHit.r-1, lastHit.c);
                else if (direction == 3)
                    p = Point(lastHit.r+1, lastHit.c);
                
                vector<double>::iterator iter = find (alreadyAttack.begin(), alreadyAttack.end(), p.r*game().cols() + p.c);
                
                if (!game().isValid(p))
                    break;
                            
                else if (iter == alreadyAttack.end())
                {
                    alreadyAttack.push_back(p.r*game().cols() + p.c);
                    found = true;
                    break;
                }
                lastHit = p;
            }
            if (!found)
            {
                lastHit = firstHit;
                state = 2;
                direction = 0;
            }
        }
    }
    
    // in state 2, we try each direction until we hit something in a direction
    if (state == 2)
    {
        found = false;
        needRand = false;
        for (int i=direction; i<4; i++)
        {
            if (direction == 0)
                p = Point(lastHit.r, lastHit.c-1);
            else if (direction == 1)
                p = Point(lastHit.r, lastHit.c+1);
            else if (direction == 2)
                p = Point(lastHit.r-1, lastHit.c);
            else if (direction == 3)
                p = Point(lastHit.r+1, lastHit.c);
            
            vector<double>::iterator iter = find (alreadyAttack.begin(), alreadyAttack.end(), p.r*game().cols() + p.c);
                        
            if (game().isValid(p) && iter == alreadyAttack.end())
            {
                found = true;
                alreadyAttack.push_back(p.r*game().cols() + p.c);
                break;
            }
            else
            {
                if(direction<3)
                    direction ++;
            }
        }
        
        if(!found)
        {
            needRand = true;
            state = 1;
        }
        
    }
    
    // in state 1 or we need random point, we will go through each spot on the board starting 0,0 and skipping smallest length-1 spots, first horizontally and vertically
    if (state == 1 || needRand)
    {
        if (rowIter < game().rows() && colIter < game().cols())
        {
            int min = *min_element(length.begin(), length.end());
            for(; rowIter<game().rows(); rowIter++)
            {
                if (rowIter%2==0)
                    colIter = 0;
                else
                    colIter=1;
                
                for (; colIter<game().cols(); colIter+=min)
                {
                    p = Point(rowIter, colIter);
                    
                    vector<double>::iterator iter = find (alreadyAttack.begin(), alreadyAttack.end(), p.r*game().cols() + p.c);
                                
                    if (iter == alreadyAttack.end())
                    {
                        alreadyAttack.push_back(p.r*game().cols() + p.c);
                        found = true;
                        break;
                    }
                }
                if (found)
                    break;
            }
        }
        else
        {
            rowIter = colIter = 0;
            int min = *min_element(length.begin(), length.end());
            for(; colIter<game().cols(); colIter++)
            {
                if (colIter%2==0)
                    rowIter = 0;
                else
                    rowIter=1;
                
                for (; rowIter<game().rows(); rowIter+=min)
                {
                    p = Point(rowIter, colIter);
                    
                    vector<double>::iterator iter = find (alreadyAttack.begin(), alreadyAttack.end(), p.r*game().cols() + p.c);
                                
                    if (iter == alreadyAttack.end())
                    {
                        alreadyAttack.push_back(p.r*game().cols() + p.c);
                        found = true;
                        break;
                    }
                }
                if (found)
                    break;
            }
        }
        
        // if we not found anything above, we will just find any point that is available
        if (!found)
        {

            for(int i=0; i<game().rows(); i++)
            {
                for (int j=0; j<game().cols(); j++)
                {
                    p = Point(i, j);
                    
                    vector<double>::iterator iter = find (alreadyAttack.begin(), alreadyAttack.end(), p.r*game().cols() + p.c);
                                
                    if (iter == alreadyAttack.end())
                    {
                        alreadyAttack.push_back(p.r*game().cols() + p.c);
                        found = true;
                        break;
                    }
                }
                if (found)
                    break;
            }
                
        }
        
    }
    if (game().isValid(p))
    {
        return p;
    }
    return Point();
}

void GoodPlayer::recordAttackByOpponent(Point p)
{}

// this function record the result of each attack
void GoodPlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId)
{
    if (validShot)
    {
        if (shotHit)
        {
            // if we hit something, record it on our fake board
            fake_board[p.r][p.c] = 'X';
            if (shipDestroyed)
            {
                state = 1;
                vector<int>::iterator iter = find (length.begin(), length.end(), game().shipLength(shipId));
                length.erase(iter);
                
                if(direction == 0)
                {
                    for (int i=0; i<game().shipLength(shipId); i++)
                    {
                        fake_board[p.r][p.c+i] = 'A';
                    }
                }
                
                if(direction == 1)
                {
                    for (int i=0; i<game().shipLength(shipId); i++)
                    {
                        fake_board[p.r][p.c-i] = 'A';
                    }
                }
                if(direction == 2)
                {
                    for (int i=0; i<game().shipLength(shipId); i++)
                    {
                        fake_board[p.r+i][p.c] = 'A';
                    }
                }
                if(direction == 3)
                {
                    for (int i=0; i<game().shipLength(shipId); i++)
                    {
                        fake_board[p.r-i][p.c] = 'A';
                    }
                }
                
                for (int i=0; i<MAXROWS; i++)
                {
                    for (int j=0; j<MAXCOLS; j++)
                    {
                        if (fake_board[i][j] == 'X')
                        {
                            firstHit = Point(i, j);
                            lastHit = Point(i, j);
                            state = 2;
                        }
                    }
                }
                
                direction = 0;
            }
            
            // hit but not destroyed
            else if (state == 1)
            {
                firstHit = p;
                lastHit = p;
                state = 2;
            }
            
            // hit and in state 2, switch to state 3
            else if (state == 2)
            {
                lastHit = p;
                state  = 3;
            }
            
            // hit and in state 3, stay in state 3
            else if ( state == 3)
            {
                lastHit = p;
            }
        }
        
        // state 2 but no hit, try next direction
        else if (state == 2)
        {
            direction++;
        }
        // state 3 but not hit, go back to state 2
        else if (state == 3)
        {
            state = 2;
            lastHit = firstHit;
            direction = 0;
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
