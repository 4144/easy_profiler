/**************************************
*
***************************************/

#ifndef EASY_PROFILER___CANVAS
#define EASY_PROFILER___CANVAS

#include <vector>
#include <QObject>
#include <QColor>
#include <QSizeF>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

struct BlocksTree;

#pragma pack(push, 1)
struct ProfBlockItem
{
    const BlocksTree*       block;
    qreal                       x;
    float                 y, w, h;
    QRgb                    color;
    unsigned int   children_begin;
    unsigned short    totalHeight;
    char                    state;

    void setRect(qreal _x, float _y, float _w, float _h);
    qreal left() const;
    float top() const;
    float width() const;
    float height() const;
    qreal right() const;
    float bottom() const;
};
#pragma pack(pop)

/////////////////////////////////////////////////

class CppCanvas : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QSizeF canvasSize READ canvasSize WRITE setCanvasSize NOTIFY canvasSizeChanged)

private:

    typedef ::std::vector<ProfBlockItem> Items;
    typedef ::std::vector<Items> Sublevels;
    typedef ::std::vector<unsigned int> DrawIndexes;

    DrawIndexes m_levelsIndexes;
    Sublevels m_levels;

    QSizeF m_canvasSize;
    bool m_bTest;

public:

    CppCanvas();
    virtual ~CppCanvas();

    Q_INVOKABLE void clear();
    Q_INVOKABLE void paint(qreal _region_x, qreal _region_y, float _region_width, float _region_height, double _scale);
    Q_INVOKABLE void test(quint64 _frames_number, quint64 _total_items_number_estimate, int _depth);

    const QSizeF& canvasSize() const;
    void setCanvasSize(const QSizeF& _size);
    void setCanvasSize(qreal _width, qreal _height);

private:

    unsigned short levels() const;
    void setLevels(unsigned short _levels);
    void reserve(unsigned short _level, size_t _items);
    const Items& items(unsigned short _level) const;
    const ProfBlockItem& getItem(unsigned short _level, size_t _index) const;
    ProfBlockItem& getItem(unsigned short _level, size_t _index);
    size_t addItem(unsigned short _level);
    size_t addItem(unsigned short _level, const ProfBlockItem& _item);
    size_t addItem(unsigned short _level, ProfBlockItem&& _item);

    void fillChildren(int _level, qreal _x, qreal _y, size_t _childrenNumber, size_t& _total_items);

signals:

    void canvasSizeChanged();

    void fillRect(double rect_x, double rect_y, float rect_width, float rect_height, const QColor& fill_color);
    void setColor(const QColor& fill_color);

};

class Real : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal value READ value WRITE setValue)
    qreal m_value;
public:
    Real() : m_value(0) {}
    virtual ~Real() {}
    qreal value() const { return m_value; }
    void setValue(qreal _value) { m_value = _value; }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // EASY_PROFILER___CANVAS
