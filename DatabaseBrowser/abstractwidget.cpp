
#include <QEvent>

#include "abstractwidget.h"

AbstractWidget::AbstractWidget(QWidget * parent /* = 0 */, Qt::WindowFlags f /* = 0 */)
    : QWidget(parent, f)
{
}

AbstractWidget::~AbstractWidget()
{
    // empty
}

void AbstractWidget::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        retranslate();
    }

    QWidget::changeEvent(event);
}
