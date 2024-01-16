#include <iostream>
#include <cmath>
#include <string>
#include <Windows.h>
#include <vector>
#include <random>

#define minSizeX 10
#define minSizeY 10
#define MapSizeX 50
#define MapSizeY 50

int map[MapSizeY][MapSizeX];

using namespace std;

struct Room
{
    int _x, _y, _w, _h;
};

struct Point
{
    int x, y;
};

struct Road
{
    Road(Point start, Point end) : _x1(start.x), _y1(start.y), _x2(end.x), _y2(end.y) { }
    Road(int x1, int y1, int x2, int y2) : Road(Point{x1, y1}, Point{x2,y2}) { }
    int _x1, _y1;
    int _x2, _y2;
};
class Container
{
public:
    Container(int x, int y, int w, int h) : _x(x), _y(y), _w(w), _h(h) { }
    ~Container() { }

    void GetRoom(OUT vector<Room*>& rooms);
    
    void GetContainer(OUT vector<Container*>& containers);
    
    Container* GetParent() { return _parent; }
    Point GetCenter() { return Point{_x + _w/2, _y + _h/2}; }
    
    
    static Container* CreateContainer(int x, int y, int w, int h)
    {
        Container* container = new Container(x, y, w, h);
        return container;
    }

    static int GetRandomNum(int min, int max)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dis(min, max);
       
        return dis(gen);
    }

public:
    int _x, _y, _w, _h;
    Container* _lChild = nullptr;
    Container* _rChild = nullptr;
    Container* _parent = nullptr;

    Room* _room = nullptr;
    Road* _road = nullptr;
};


void Container::GetRoom(OUT vector<Room*>& rooms)
{
    if (!_lChild && !_rChild && _room)
        rooms.push_back(_room);
    else
    {
        if (_lChild)
            _lChild->GetRoom(rooms);
        if (_rChild)
            _rChild->GetRoom(rooms);
    }
        
}

void Container::GetContainer(OUT vector<Container*>& containers)
{
    containers.push_back(this);
    if(_lChild)
        _lChild->GetContainer(containers);
    
    if(_rChild)
        _rChild->GetContainer(containers);
    
}

Room* MakeRoom(Container* container)
{
    int x = container->_x;
    int y = container->_y;
    int w = container->_w;
    int h = container->_h;

    Room* room = new Room();

    room->_x = x + 2;
    room->_y = y + 2;
    room->_w = w - 4;
    room->_h = h - 4;

    container->_room = room;

    return room;
}

void printMap()
{
    for (int i = 0; i < MapSizeY; i++)
    {
        for (int j = 0; j < MapSizeX; j++)
        {
            if(map[i][j] == 0) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0);
            else if(map[i][j] == 1) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
            else if(map[i][j] == 2) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9);
            
            cout << map[i][j];
        }
        cout << endl;
       
    }
    cout << endl;
}

void Divide(Container* parent)
{
    int xLen = parent->_w;
    int yLen = parent->_h;

    int x = parent->_x;
    int y = parent->_y;
    int w = parent->_w;
    int h = parent->_h;

    int ratio = Container::GetRandomNum(4, 6);

    if (w - 4 <= minSizeX || h - 4 <= minSizeY)
    {
        Room* room = MakeRoom(parent);
        return;
    }
        

    if (xLen > yLen)
    {
        int slicedW = w * ratio / 10;

        // lchild;
        Container* lChild = Container::CreateContainer(x, y, slicedW, h);

        // rChild;
        Container* rChild = Container::CreateContainer(x + slicedW, y, w - slicedW, h);

        Divide(lChild);
        Divide(rChild);

        parent->_lChild = lChild;
        parent->_rChild = rChild;


    }
    else if (xLen <= yLen)
    {
        int slicedH = h * ratio / 10;

        // lchild;
        Container* lChild = Container::CreateContainer(x, y, w, slicedH);

        // rChild;
        Container* rChild = Container::CreateContainer(x, y + slicedH, w, h - slicedH);

        Divide(lChild);
        Divide(rChild);

        parent->_lChild = lChild;
        parent->_rChild = rChild;
    }
}

void Connect(Container* root)
{
    if (root->_lChild == nullptr || root->_rChild == nullptr) return;
    
    Point lCenter = root->_lChild->GetCenter();
    Point rCenter = root->_rChild->GetCenter();

    Road* road = new Road(lCenter.x, lCenter.y, rCenter.x, rCenter.y);
    root->_road = road;

    if (lCenter.x == rCenter.x)
    {
        
        for (int i = lCenter.y ; i < rCenter.y; i++)
        {
            map[i][lCenter.x] = 2;
        }
    }
    else if (lCenter.y == rCenter.y)
    {
        for (int i = lCenter.x; i < rCenter.x; i++)
        {
            map[lCenter.y][i] = 2;
        }
    }
    
    Connect(root->_lChild);
    Connect(root->_rChild);
}

void BSP()
{
    memset(map, 0, sizeof(map));
    Container* root = Container::CreateContainer(0, 0, MapSizeX, MapSizeY);
    Divide(root);
    Connect(root);

    vector<Room*> rooms;
    root->GetRoom(rooms);

    for (auto item : rooms)
    {
        int x = item->_x;
        int y = item->_y;
        int w = item->_w;
        int h = item->_h;

        for (int i = y; i < y + h; i++)
        {
            for (int j = x; j < x + w; j++)
            {
                map[i][j] = 1;
            }
        }
    }
}

int main()
{
    BSP();
    printMap();

}
