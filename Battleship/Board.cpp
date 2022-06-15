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
      // TODO:  Decide what private members you need.  Here's one that's likely
      //        to be useful:
    const Game& m_game;
    char m_board[MAXROWS][MAXCOLS];
    vector<int> placedShip;
    vector<char> demagedShip;
};

BoardImpl::BoardImpl(const Game& g)
 : m_game(g)
{
    for (int i = 0; i < m_game.rows(); i++)
    {
        for (int j = 0; j < m_game.cols(); j++)
            m_board[i][j] = '.';
    }
}

// clear our board by making it all '.'
void BoardImpl::clear()
{
    for (int i = 0; i < m_game.rows(); i++)
    {
        for (int j=0; j<m_game.cols(); j++)
        {
            m_board[i][j] = '.';
        }
    }
}

// block half of the board using #
void BoardImpl::block()
{
    for (int i=0; i<m_game.rows()*m_game.cols()/2; i++)
    {
        Point p = m_game.randomPoint();
        if (m_board[p.r][p.c] == '#')
            i--;
        else
            m_board[p.r][p.c] = '#';
    }
}

// unblock the board
void BoardImpl::unblock()
{
    for (int i=0; i<m_game.rows(); i++)
    {
        for (int j=0; j<m_game.cols(); j++)
        {
            if (m_board[i][j] == '#')
                m_board[i][j] = '.';
        }
    }
}

// place ships on the board and return true if can be placed, false otherwise
bool BoardImpl::placeShip(Point topOrLeft, int shipId, Direction dir)
{

    if (shipId <0 || shipId >= m_game.nShips())
    {
        return false;
    }
    
    for (int i=0; i<m_game.shipLength(shipId); i++)
    {
        if (dir==HORIZONTAL)
        {
            Point p = Point(topOrLeft.r, topOrLeft.c+i);
            if (!m_game.isValid(p) || m_board[topOrLeft.r][topOrLeft.c+i] != '.')
            {
                return false;
            }
        }
        else
        {
            Point p = Point(topOrLeft.r+i, topOrLeft.c);
            if (!m_game.isValid(p) || m_board[topOrLeft.r+i][topOrLeft.c] != '.')
            {
                return false;
            }
        }
    }
    
    vector<int>::iterator iter = find (placedShip.begin(), placedShip.end(), shipId);

    if (iter != placedShip.end())
    {
        return false;
    }
    
    for (int i=0; i<m_game.shipLength(shipId); i++)
    {
        if (dir==HORIZONTAL)
            m_board[topOrLeft.r][topOrLeft.c+i] = m_game.shipSymbol(shipId);
        else
            m_board[topOrLeft.r+i][topOrLeft.c] = m_game.shipSymbol(shipId);
    }
    
    placedShip.push_back(shipId);
    return true;
}

bool BoardImpl::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
    if (shipId <0 || shipId >= m_game.nShips())
    {
        return false;
    }
    
    for (int i=0; i<m_game.shipLength(shipId); i++)
    {
        if (dir==HORIZONTAL)
        {
            Point p = Point(topOrLeft.r, topOrLeft.c+i);
            if (!m_game.isValid(p) || m_board[topOrLeft.r][topOrLeft.c+i]!=m_game.shipSymbol(shipId))
                return false;
        }
        else
        {
            Point p = Point(topOrLeft.r+i, topOrLeft.c);
            if (!m_game.isValid(p) || m_board[topOrLeft.r+i][topOrLeft.c]!=m_game.shipSymbol(shipId))
                return false;
        }
    }
    
    vector<int>::iterator iter = find (placedShip.begin(), placedShip.end(), shipId);

    if (iter == placedShip.end())
    {
        return false;
    }
    
    for (int i=0; i<m_game.shipLength(shipId); i++)
    {
        if (dir==HORIZONTAL)
            m_board[topOrLeft.r][topOrLeft.c+i] = '.';
        else
            m_board[topOrLeft.r+i][topOrLeft.c] = '.';
    }
    
    placedShip.erase(iter);
    return true;
}

void BoardImpl::display(bool shotsOnly) const
{
    cout << "  ";
    for (int i=0; i<m_game.cols(); i++)
        cout << i;
    cout << '\n';
    
    for (int i=0; i<m_game.rows(); i++)
    {
        for (int j=-1; j<m_game.cols(); j++)
        {
            if (j==-1)
                cout << i << " ";
            else if (shotsOnly && m_board[i][j] != 'X' && m_board[i][j] != 'o' && m_board[i][j] != '#')
                cout << '.';
            else
                cout << m_board[i][j];
        }
        cout << '\n';
    }
}

bool BoardImpl::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
    if (!m_game.isValid(p) || m_board[p.r][p.c] == 'X' || m_board[p.r][p.c] == 'o')
        return false;
    
    if (m_board[p.r][p.c] == '.')
    {
        shotHit = false;
        m_board[p.r][p.c] = 'o';
        return true;
    }
    
    demagedShip.push_back(m_board[p.r][p.c]);
    shotHit = true;
    
    int n = (int) count(demagedShip.begin(), demagedShip.end(), m_board[p.r][p.c]);
    
    int i = 0;
    for (; i<placedShip.size(); i++)
    {
        if (m_game.shipSymbol(placedShip[i]) == m_board[p.r][p.c])
            break;
    }
    
    if (m_game.shipLength(placedShip[i]) == n)
    {
        shipDestroyed = true;
        shipId = placedShip[i];
    }
    
    else
        shipDestroyed = false;
    
    m_board[p.r][p.c] = 'X';
    
    return true;
}

bool BoardImpl::allShipsDestroyed() const
{
    int total = 0;
    for (int i=0; i<placedShip.size(); i++)
    {
        total += m_game.shipLength(placedShip[i]);
    }
    if (total == demagedShip.size())
        return true;
    return false;
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
