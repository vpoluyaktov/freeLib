#include <QCollator>
#include <QHeaderView>

#include "treebookitem.h"
#include "common.h"
#include "library.h"

TreeBookItem::TreeBookItem(QTreeWidget *parent, int type)
    :QTreeWidgetItem(parent, type)
{

}

TreeBookItem::TreeBookItem(QTreeWidgetItem *parent, int type)
    :QTreeWidgetItem(parent, type)
{

}

bool TreeBookItem::operator<(const QTreeWidgetItem &other) const
{
    static QCollator collator;
    auto sortColumn = treeWidget()->sortColumn();

    if (type() == ITEM_TYPE_BOOK && other.type() == ITEM_TYPE_SERIA && parent()->type() == ITEM_TYPE_AUTHOR) {
        auto header = treeWidget()->header();
        auto sortOrder = header->sortIndicatorOrder();
        return sortOrder == Qt::DescendingOrder;
    }
    if (type() == ITEM_TYPE_SERIA && other.type() == ITEM_TYPE_BOOK && other.parent()->type() == ITEM_TYPE_AUTHOR) {
        auto header = treeWidget()->header();
        auto sortOrder = header->sortIndicatorOrder();
        return sortOrder == Qt::AscendingOrder;
    }

    if (type() == ITEM_TYPE_BOOK && other.type() == ITEM_TYPE_BOOK) {
        SBook& bookThis = mLibs[g_idCurrentLib].mBooks[data(0, Qt::UserRole).toUInt()];
        SBook& bookOther = mLibs[g_idCurrentLib].mBooks[other.data(0, Qt::UserRole).toUInt()];
        switch (sortColumn) {
        case 0:  // название книги
            if (bookThis.idSerial > 0 && bookOther.idSerial)
                return collator.compare(text(0), other.text(0)) < 0;
            else
                return collator.compare(text(0), other.text(0)) >= 0;
        case 1: // номер в серии
            return bookThis.numInSerial < bookOther.numInSerial;
        case 2: // размер
            return bookThis.nSize < bookOther.nSize;
        case 3: // оценка (рейтинг)
            return bookThis.nStars < bookOther.nStars;
        case 4: // дата добавления
            return bookThis.date < bookOther.date;
        case 5: // жанр
            return collator.compare(mGenre[bookThis.listIdGenres.first()].sName, mGenre[bookOther.listIdGenres.first()].sName) < 0;
        case 6: // язык
            return mLibs[g_idCurrentLib].vLaguages[bookThis.idLanguage] < mLibs[g_idCurrentLib].vLaguages[bookOther.idLanguage];
        case 7: // формат
            return bookThis.sFormat < bookOther.sFormat;
        case 8: // прочитано
            return bookThis.bReaded < bookOther.bReaded;
        }
    }

    if (type() == ITEM_TYPE_AUTHOR && other.type() == ITEM_TYPE_AUTHOR) {
        auto header = treeWidget()->header();
        auto sortOrder = header->sortIndicatorOrder();
        switch (sortColumn) {
        case 0:  // название книги
            return collator.compare(text(0), other.text(0)) < 0;
        default:
            return (collator.compare(text(0), other.text(0)) < 0) != (sortOrder == Qt::DescendingOrder);
        }
    }
    if (type() == ITEM_TYPE_SERIA && other.type() == ITEM_TYPE_SERIA) {
        auto header = treeWidget()->header();
        auto sortOrder = header->sortIndicatorOrder();
        switch (sortColumn) {
        case 0:  // название книги
            return collator.compare(text(0), other.text(0)) < 0;
        default:
            return (collator.compare(text(0), other.text(0)) < 0) != (sortOrder == Qt::DescendingOrder);
        }
    }
    return true;
}
