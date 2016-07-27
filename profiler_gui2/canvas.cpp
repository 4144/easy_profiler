/**************************************
*
***************************************/

#include "canvas.hpp"
#include <algorithm>
#include <QDebug>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

const qreal GRAPHICS_ROW_SIZE = 16;
const qreal GRAPHICS_ROW_SIZE_FULL = GRAPHICS_ROW_SIZE + 2;
const qreal ROW_SPACING = 10;
const QRgb DEFAULT_COLOR = 0x00f0e094;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QRgb toRgb(unsigned int _red, unsigned int _green, unsigned int _blue)
{
    if (_red == 0 && _green == 0 && _blue == 0)
        return DEFAULT_COLOR;
    return (_red << 16) + (_green << 8) + _blue;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ProfBlockItem::setRect(qreal _x, float _y, float _w, float _h) {
    x = _x;
    y = _y;
    w = _w;
    h = _h;
}

qreal ProfBlockItem::left() const {
    return x;
}

float ProfBlockItem::top() const {
    return y;
}

float ProfBlockItem::width() const {
    return w;
}

float ProfBlockItem::height() const {
    return h;
}

qreal ProfBlockItem::right() const {
    return x + w;
}

float ProfBlockItem::bottom() const {
    return y + h;
}

/////////////////////////////////////////////////

CppCanvas::CppCanvas()
{

}

CppCanvas::~CppCanvas()
{

}

/////////////////////////////////////////////////

void CppCanvas::clear()
{
    m_levelsIndexes.clear();
    m_levels.clear();
}

/////////////////////////////////////////////////

const QSizeF& CppCanvas::canvasSize() const
{
    return m_canvasSize;
}

void CppCanvas::setCanvasSize(const QSizeF& _size)
{
    m_canvasSize = _size;
    emit canvasSizeChanged();
}

void CppCanvas::setCanvasSize(qreal _width, qreal _height)
{
    m_canvasSize.setWidth(_width);
    m_canvasSize.setHeight(_height);
    emit canvasSizeChanged();
}

/////////////////////////////////////////////////

void CppCanvas::paint(qreal _region_x, qreal _region_y, float _region_width, float _region_height, double _scale)
{
    //qDebug() << "C++ CppCanvas::paint(" << _region_x << ", " << _region_y << ", " << _region_width << ", " << _region_height << ");";

    if (m_levels.empty() || m_levels.front().empty())
    {
        return;
    }

    //const auto self_x = 0.0;
    const auto currentScale = _scale; // Current GraphicsView scale
    //const auto scaleRevert = 1.0 / currentScale; // Multiplier for reverting current GraphicsView scale
    const auto sceneLeft = _region_x, sceneRight = _region_x + _region_width;

    //qDebug() << "SCENE(" << sceneLeft << ", " << sceneRight << ");";


    // Reset indices of first visible item for each layer
    const auto levelsNumber = levels();
    for (unsigned short i = 1; i < levelsNumber; ++i)
        m_levelsIndexes[i] = -1;


    // Search for first visible top item
    auto& level0 = m_levels[0];
    auto first = ::std::lower_bound(level0.begin(), level0.end(), sceneLeft, [](const ProfBlockItem& _item, double _value)
    {
        return _item.left() < _value;
    });

    if (first != level0.end())
    {
        m_levelsIndexes[0] = first - level0.begin();
        if (m_levelsIndexes[0] > 0)
            m_levelsIndexes[0] -= 1;
    }
    else
    {
        m_levelsIndexes[0] = level0.size() - 1;
    }


    const auto dx = 0;
//    // STUB! --------------------------------------------------------
//    //
//    // This is to make _painter->drawText() work properly
//    // (it seems there is a bug in Qt5.6 when drawText called for big coordinates,
//    // drawRect at the same time called for actually same coordinates
//    // works fine without using this additional transform fromTranslate)
//    const auto dx = m_levels[0][m_levelsIndexes[0]].left();
//    _painter->setTransform(QTransform::fromTranslate(dx, 0), true);
//    //
//    // END STUB! ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//    _painter->setTransform(QTransform::fromScale(scaleRevert, 1), true);


    // Iterate through layers and draw visible items
    for (unsigned short l = 0; l < levelsNumber; ++l)
    {
        auto& level = m_levels[l];
        const auto next_level = l + 1;
        char state = 1;

        for (unsigned int i = m_levelsIndexes[l], end = level.size(); i < end; ++i)
        {
            auto& item = level[i];
            if (item.state != 0) state = item.state;

            if (item.right() < sceneLeft || state == -1)
            {
                ++m_levelsIndexes[l];
                continue;
            }

            auto w = ::std::max(item.width() * currentScale, 1.0);

            if (w < 20)
            {
                // Items which width is less than 20 will be painted as big rectangles which are hiding it's children
                if (item.left() > sceneRight)
                    break;

//              if (previousColor != item.color)
//              {
//                  previousColor = item.color;
//                  brush.setColor(previousColor);
//                  _painter->setBrush(brush);
//              }

                ////rect.setRect(item.left(), item.top(), item.width(), item.totalHeight);
                ////rect.setRect(item.left() * currentScale, item.top(), w, item.totalHeight);
                //rect.setRect((item.left() - dx) * currentScale, item.top(), w, item.totalHeight);
                //_painter->drawRect(rect);

                emit fillRect((item.left() - dx) * currentScale, item.top(), w, item.totalHeight, item.color);

                if (next_level < levelsNumber && item.children_begin != -1)
                    m_levels[next_level][item.children_begin].state = -1;

                continue;
            }

            if (next_level < levelsNumber && item.children_begin != -1)
            {
                if (m_levelsIndexes[next_level] == -1)
                    m_levelsIndexes[next_level] = item.children_begin;
                m_levels[next_level][item.children_begin].state = 1;
            }

            if (item.left() > sceneRight)
                break;

//          if (previousColor != item.color)
//          {
//              previousColor = item.color;
//              brush.setColor(previousColor);
//              _painter->setBrush(brush);
//          }

            ////rect.setRect(item.left(), item.top(), item.width(), item.height());
            ////rect.setRect(item.left() * currentScale, item.top(), w, item.height());
            //rect.setRect((item.left() - dx) * currentScale, item.top(), w, item.height());
            //_painter->drawRect(rect);

            emit fillRect((item.left() - dx) * currentScale, item.top(), w, item.height(), item.color);

//            auto xtext = 0;
//            if (item.left() < sceneLeft)
//            {
//                w += (item.left() - sceneLeft) * currentScale;
//                xtext = sceneLeft - dx;
//            }
//            else
//            {
//                xtext = item.left() - dx;
//            }

//            //w = item.width();
//            rect.setRect(xtext * currentScale, item.top(), w, item.height());

//            auto textColor = 0x00ffffff - previousColor;
//            if (textColor == previousColor) textColor = 0;
//            pen.setColor(textColor); // Text is painted with inverse color
//            _painter->setPen(pen);
//            auto text = m_bTest ? "NOT VERY LONG TEST TEXT" : item.block->node->getBlockName();
//            _painter->drawText(rect, 0, text);
//            _painter->setPen(penStyle); // restore pen for rectangle painting
        }
    }
}

/////////////////////////////////////////////////

unsigned short CppCanvas::levels() const
{
    return static_cast<unsigned short>(m_levels.size());
}

void CppCanvas::setLevels(unsigned short _levels)
{
    m_levels.resize(_levels);
    m_levelsIndexes.resize(_levels, -1);
}

void CppCanvas::reserve(unsigned short _level, size_t _items)
{
    m_levels[_level].reserve(_items);
}

const CppCanvas::Items& CppCanvas::items(unsigned short _level) const
{
    return m_levels[_level];
}

const ProfBlockItem& CppCanvas::getItem(unsigned short _level, size_t _index) const
{
    return m_levels[_level][_index];
}

ProfBlockItem& CppCanvas::getItem(unsigned short _level, size_t _index)
{
    return m_levels[_level][_index];
}

size_t CppCanvas::addItem(unsigned short _level)
{
    m_levels[_level].emplace_back();
    return m_levels[_level].size() - 1;
}

size_t CppCanvas::addItem(unsigned short _level, const ProfBlockItem& _item)
{
    m_levels[_level].emplace_back(_item);
    return m_levels[_level].size() - 1;
}

size_t CppCanvas::addItem(unsigned short _level, ProfBlockItem&& _item)
{
    m_levels[_level].emplace_back(::std::forward<ProfBlockItem&&>(_item));
    return m_levels[_level].size() - 1;
}

/////////////////////////////////////////////////

void CppCanvas::fillChildren(int _level, qreal _x, qreal _y, size_t _childrenNumber, size_t& _total_items)
{
    size_t nchildren = _childrenNumber;
    _childrenNumber >>= 1;

    for (size_t i = 0; i < nchildren; ++i)
    {
        size_t j = addItem(_level);
        auto& b = getItem(_level, j);
        b.color = toRgb(30 + rand() % 225, 30 + rand() % 225, 30 + rand() % 225);
        b.state = 0;

        if (_childrenNumber > 0)
        {
            const auto& children = items(_level + 1);
            b.children_begin = static_cast<unsigned int>(children.size());

            fillChildren(_level + 1, _x, _y + GRAPHICS_ROW_SIZE + 2, _childrenNumber, _total_items);

            const auto& last = children.back();
            b.setRect(_x, _y, last.right() - _x, GRAPHICS_ROW_SIZE);
            b.totalHeight = GRAPHICS_ROW_SIZE + last.totalHeight + 2;
        }
        else
        {
            b.setRect(_x, _y, 10 + rand() % 40, GRAPHICS_ROW_SIZE);
            b.totalHeight = GRAPHICS_ROW_SIZE;
            b.children_begin = 0;
        }

        _x = b.right();
        ++_total_items;
    }
}

/////////////////////////////////////////////////

void CppCanvas::test(quint64 _frames_number, quint64 _total_items_number_estimate, int _depth)
{
    static const qreal X_BEGIN = 100;
    static const qreal Y_BEGIN = 50;

    const auto children_per_frame = _total_items_number_estimate / _frames_number;
    const size_t first_level_children_count = sqrt(pow(2, _depth - 1)) * pow(children_per_frame, 1.0 / double(_depth)) + 0.5;

    clear();
    setLevels(_depth + 1);
    reserve(0, _frames_number);
    size_t chldrn = first_level_children_count;
    size_t tot = first_level_children_count;
    for (int i = 1; i <= _depth; ++i)
    {
        reserve(i, tot * _frames_number);
        chldrn >>= 1;
        tot *= chldrn;
    }

    size_t total_items = 0; // for statistics
    qreal x = X_BEGIN;
    for (unsigned int i = 0; i < _frames_number; ++i)
    {
        size_t j = addItem(0);
        auto& b = getItem(0, j);
        b.color = toRgb(30 + rand() % 225, 30 + rand() % 225, 30 + rand() % 225);
        b.state = 0;

        const auto& children = items(1);
        b.children_begin = static_cast<unsigned int>(children.size());

        fillChildren(1, x, Y_BEGIN + GRAPHICS_ROW_SIZE + 2, first_level_children_count, total_items);

        const auto& last = children.back();
        b.setRect(x, Y_BEGIN, last.right() - x, GRAPHICS_ROW_SIZE);
        b.totalHeight = GRAPHICS_ROW_SIZE + last.totalHeight + 2;

        x += b.width() + 500;

        ++total_items;
    }

    setCanvasSize(x, Y_BEGIN + getItem(0, 0).totalHeight);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
