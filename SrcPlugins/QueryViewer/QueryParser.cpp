
#include "QueryParser.h"

namespace
{
    const char c_semicolon = ';';
    const char c_quote = '\'';
    const char c_doubleQuote = '\"';
    const char c_minus = '-';
}

QueryParser::QueryParser()
    : m_openQuote(NONE)
{
    // empty
}

QStringList QueryParser::parse(const QString &queryString)
{
    QStringList output;
    int prevQueryPosition = 0;
    QString stringToParse = queryString.trimmed();

    for (int i = 0; i < stringToParse.length(); ++i)
    {
        const QChar &currentChar = stringToParse[i];

        if ((currentChar == ::c_semicolon) && (!isOpenQuote()))
        {
            int queryLength = i - prevQueryPosition;
            if (queryLength > 0)
            {
                QString query = stringToParse.mid(prevQueryPosition, queryLength + 1);
                output.append(query);
            }

            prevQueryPosition = i + 1;
        }
        else if (currentChar == ::c_quote)
        {
            processQuote(SINGLE);
        }
        else if(currentChar == ::c_doubleQuote)
        {
            processQuote(DOUBLE);
        }
        else if (currentChar == ::c_minus)
        {
            processMinus(queryString, i);
            prevQueryPosition = i + 1;
        }
    }

    if (prevQueryPosition < stringToParse.length())
    {
        QString query = stringToParse.mid(prevQueryPosition);
        output.append(query);
    }

    return output;
}

bool QueryParser::isOpenQuote() const
{
    return m_openQuote != NONE;
}

void QueryParser::processQuote(const QuoteType type)
{
    Q_ASSERT(type != NONE);

    if (!isOpenQuote())
    {
        m_openQuote = type;
    }
    else if (type == m_openQuote)
    {
        if (isOpenQuote())
        {
            m_openQuote = NONE;
        }
    }
}

void QueryParser::processMinus(const QString &inQueryString,
                               int &outPosition)
{
    // check next symbol after '-'
    if (inQueryString[outPosition + 1] == ::c_minus)
    {
        // this is comment
        outPosition = inQueryString.indexOf('\n', outPosition);
        if (-1 == outPosition)
        {
            // all text commented
            outPosition = inQueryString.length();
        }
    }
}
