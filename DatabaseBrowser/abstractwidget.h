#ifndef ABSTRACTWIDGET_H
#define ABSTRACTWIDGET_H

#include <QWidget>

class QEvent;

class AbstractWidget : public QWidget
{
    Q_OBJECT

public:
    AbstractWidget(QWidget * parent = 0, Qt::WindowFlags f = 0);
    virtual ~AbstractWidget();

protected:
    // change language event
    virtual void changeEvent(QEvent *event);

    // translate all strings
    virtual void retranslate() = 0;
};

#endif // ABSTRACTWIDGET_H
