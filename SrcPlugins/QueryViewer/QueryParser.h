#ifndef QUERYPARSER_H
#define QUERYPARSER_H

#include <QStringList>

class QueryParser
{
public:
    QueryParser();
    QStringList parse(const QString &queryString);

private:
    enum QuoteType
    {
        NONE,
        SINGLE,
        DOUBLE
    };

    bool isOpenQuote() const;
    void processQuote(const QuoteType type);
    void processMinus(const QString &inQueryString,
                      int &outPosition);

    QuoteType m_openQuote;
};

#endif // QUERYPARSER_H
