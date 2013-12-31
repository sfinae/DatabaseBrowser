#ifndef DATA_VIEWER_H
#define DATA_VIEWER_H

#include "BaseViewer.h"

#include <QScopedPointer>

class DataViewerPrivate;

class DataViewer : public BaseViewer
{
    Q_OBJECT
    Q_INTERFACES(BaseViewer)
    Q_PLUGIN_METADATA(IID "com.michael.PluginExample.BaseViewer")

public:
    DataViewer();
    virtual ~DataViewer();

    virtual bool eventFilter(QObject *watched, QEvent *event);

    virtual BaseViewer* clone() const;

    virtual void onDatabaseItemRemoved();
    virtual void onDatabaseItemActivated(const DatabaseItem &item);

    virtual QIcon icon() const;
    virtual QString statusTip() const;
    virtual QString text() const;

protected:
    virtual void retranslate();

private:
    QScopedPointer<DataViewerPrivate> d_ptr;
};

#endif // DATA_VIEWER_H
