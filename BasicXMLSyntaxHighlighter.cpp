#include "BasicXMLSyntaxHighlighter.h"

BasicXMLSyntaxHighlighter::BasicXMLSyntaxHighlighter(QObject * parent) :
    QSyntaxHighlighter(parent)
{
    setRegexes();
    // setFormats();
}

BasicXMLSyntaxHighlighter::BasicXMLSyntaxHighlighter(QTextDocument * parent) :
    QSyntaxHighlighter(parent)
{
    setRegexes();
    // setFormats();
}

BasicXMLSyntaxHighlighter::BasicXMLSyntaxHighlighter(QTextEdit * parent) :
    QSyntaxHighlighter(parent)
{
    setRegexes();
    // setFormats();
}

void BasicXMLSyntaxHighlighter::highlightBlock(const QString & text)
{
    // Special treatment for xml element regex as we use captured text to emulate lookbehind
    int xmlElementIndex = m_xmlElementRegex.indexIn(text);
    while(xmlElementIndex >= 0)
    {
        int matchedPos = m_xmlElementRegex.pos(1);
        int matchedLength = m_xmlElementRegex.cap(1).length();
        setFormat(matchedPos, matchedLength, m_xmlElementFormat);

        xmlElementIndex = m_xmlElementRegex.indexIn(text, matchedPos + matchedLength);
    }

    // Highlight xml keywords *after* xml elements to fix any occasional / captured into the enclosing element
    typedef QList<QRegExp>::const_iterator Iter;
    Iter xmlKeywordRegexesEnd = m_xmlKeywordRegexes.end();
    for(Iter it = m_xmlKeywordRegexes.begin(); it != xmlKeywordRegexesEnd; ++it) {
        const QRegExp & regex = *it;
        highlightByRegex(m_xmlKeywordFormat, regex, text);
    }

    highlightByRegex(m_xmlAttributeFormat, m_xmlAttributeRegex, text);
    highlightByRegex(m_xmlCommentFormat, m_xmlCommentRegex, text);
    highlightByRegex(m_xmlValueFormat, m_xmlValueRegex, text);
}

void BasicXMLSyntaxHighlighter::highlightByRegex(const QTextCharFormat & format,
                                                 const QRegExp & regex, const QString & text)
{
    int index = regex.indexIn(text);

    while(index >= 0)
    {
        int matchedLength = regex.matchedLength();
        setFormat(index, matchedLength, format);

        index = regex.indexIn(text, index + matchedLength);
    }
}

void BasicXMLSyntaxHighlighter::setRegexes()
{
    m_xmlElementRegex.setPattern("<[?\\s]*[/]?[\\s]*([^\\n][^>]*)(?=[\\s/>])");
    m_xmlAttributeRegex.setPattern("\\w+(?=\\=)");
    m_xmlValueRegex.setPattern("\"[^\\n\"]+\"(?=[?\\s/>])");
    m_xmlCommentRegex.setPattern("<!--[^\\n]*-->");

    m_xmlKeywordRegexes = QList<QRegExp>() << QRegExp("<\\?") << QRegExp("/>")
                                           << QRegExp(">") << QRegExp("<") << QRegExp("</")
                                           << QRegExp("\\?>");
}

void BasicXMLSyntaxHighlighter::setFormats(struct TextFormat textFormats[])
{
    if (textFormats == NULL) {
        m_xmlKeywordFormat.setForeground(Qt::blue);
        m_xmlKeywordFormat.setFontWeight(QFont::Bold);

        m_xmlElementFormat.setForeground(Qt::darkMagenta);
        m_xmlElementFormat.setFontWeight(QFont::Bold);

        m_xmlAttributeFormat.setForeground(Qt::darkGreen);
        m_xmlAttributeFormat.setFontWeight(QFont::Bold);
        m_xmlAttributeFormat.setFontItalic(true);

        m_xmlValueFormat.setForeground(Qt::darkRed);

        m_xmlCommentFormat.setForeground(Qt::gray);
    } else {
        m_xmlKeywordFormat.setForeground(textFormats[0].foreground);
        m_xmlKeywordFormat.setFontWeight(textFormats[0].weight);
        m_xmlKeywordFormat.setFontItalic(textFormats[0].italic);

        m_xmlElementFormat.setForeground(textFormats[1].foreground);
        m_xmlElementFormat.setFontWeight(textFormats[1].weight);
        m_xmlElementFormat.setFontItalic(textFormats[1].italic);

        m_xmlAttributeFormat.setForeground(textFormats[2].foreground);
        m_xmlAttributeFormat.setFontWeight(textFormats[2].weight);
        m_xmlAttributeFormat.setFontItalic(textFormats[2].italic);

        m_xmlValueFormat.setForeground(textFormats[3].foreground);
        m_xmlValueFormat.setFontWeight(textFormats[3].weight);
        m_xmlValueFormat.setFontItalic(textFormats[3].italic);

        m_xmlCommentFormat.setForeground(textFormats[4].foreground);
        m_xmlCommentFormat.setFontWeight(textFormats[4].weight);
        m_xmlCommentFormat.setFontItalic(textFormats[4].italic);
    }
}

