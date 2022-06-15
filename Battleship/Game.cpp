#include "Game.h"
#include "Board.h"
#include "Player.h"
#include "globals.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>

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
    struct ship
    {
        ship(int l, char s, string n):  length(l), symbol(s), name(n){}
        int length;
        char symbol;
        string name;
    };
    int row;
    int col;
    vector<ship> shipvec;
};

void waitForEnter()
{
    cout << "Press enter to continue: ";
    cin.ignore(10000, '\n');
}

GameImpl::GameImpl(int nRows, int nCols)
{
    row = nRows;
    col = nCols;
}

int GameImpl::rows() const
{
    return row;
}

int GameImpl::cols() const
{
    return col;
}

bool GameImpl::isValid(Point p) const
{
    return (p.r >= 0  &&  p.r < rows()  &&  p.c >= 0  &&  p.c < cols());
}

Point GameImpl::randomPoint() const
{
    return Point(randInt(rows()), randInt(cols()));
}

bool GameImpl::addShip(int length, char symbol, string name)
{
    shipvec.push_back(ship(length, symbol, name));
    return true;
}

int GameImpl::nShips() const
{
    return (int)shipvec.size();
}

int GameImpl::shipLength(int shipId) const
{
    return shipvec[shipId].length;
}

char GameImpl::shipSymbol(int shipId) const
{
    return shipvec[shipId].symbol;
}

string GameImpl::shipName(int shipId) const
{
    return shipvec[shipId].name;
}

Player* GameImpl::play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause = true)
{
    b1.clear();
    b2.clear();
    
    if (!p1->placeShips(b1) || !p2->placeShips(b2))
    {
        return nullptr;
    }
    
    int round = 0;
    
    while(true)
    {
        if (round%2==0)
        {
            cout << p1->name() << "'s turn. Board for " << p2->name() << ":" << '\n';
            b2.display(p1->isHuman());
            bool shotHit, shipDestroyed;
            int shipId;
            Point attack = p1->recommendAttack();
            bool validShot = b2.attack(attack, shotHit, shipDestroyed, shipId);
            p1->recordAttackResult(attack, validShot, shotHit, shipDestroyed, shipId);
            p2->recordAttackByOpponent(attack);
            if (!validShot)
                cout << p1->name() << " wasted a shot at (" << attack.r << "," << attack.c << "). \n";
            else if (!shotHit)
            {
                cout << p1->name() << " attacked (" << attack.r << "," << attack.c << ") and missed, resulting in: \n";
                b2.display(p1->isHuman());
            }
            else if (shipDestroyed)
            {
                cout << p1->name() << " attacked (" << attack.r << "," << attack.c << ") and destroyed the " << shipName(shipId) <<  ", resulting in: \n";
                b2.display(p1->isHuman());

            }
            else
            {
                cout << p1->name() << " attacked (" << attack.r << "," << attack.c << ") and hit something, resulting in: \n";
                b2.display(p1->isHuman());
            }
        }
        else
        {
            cout << p2->name() << "'s turn. Board for " << p1->name() << ":" << '\n';
            b1.display(p2->isHuman());
            bool shotHit, shipDestroyed;
            int shipId;
            Point attack = p2->recommendAttack();
            bool validShot = b1.attack(attack, shotHit, shipDestroyed, shipId);
            p2->recordAttackResult(attack, validShot, shotHit, shipDestroyed, shipId);
            p1->recordAttackByOpponent(attack);
            if (!validShot)
                cout << p2->name() << " wasted a shot at (" << attack.r << "," << attack.c << "). \n";
            else if (!shotHit)
            {
                cout << p2->name() << " attacked (" << attack.r << "," << attack.c << ") and missed, resulting in: \n";
                b1.display(p2->isHuman());
            }
            else if (shipDestroyed)
            {
                cout << p2->name() << " attacked (" << attack.r << "," << attack.c << ") and destroyed the " << shipName(shipId) <<  ", resulting in: \n";
                b1.display(p2->isHuman());
            }
            else
            {
                cout << p2->name() << " attacked (" << attack.r << "," << attack.c << ") and hit something, resulting in: \n";
                b1.display(p2->isHuman());
            }
            
        }
        if (b2.allShipsDestroyed())
        {
            cout << p1->name() << " wins! \n";
            if (p2->isHuman())
            {
                cout << "Here is where "<< p1->name() << "'s ships were: \n";
                b1.display(false);
            }
            return p1;
        }
        if (b1.allShipsDestroyed())
        {
            cout << p2->name() << " wins! \n";
            if (p1->isHuman())
            {
                cout << "Here is where "<< p2->name() << "'s ships were: \n";
                b2.display(false);
            }

            return p2;
        }
        if (shouldPause)
            waitForEnter();
        round ++;
    }
    
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

