#include "highlighter.h"

Highlighter::Highlighter(QTextDocument *parent, CustomTextEdit *cte) : QSyntaxHighlighter(parent) {
    HighlightingRule rule;

    // Regular Text
    keywordFormat.setForeground(cte->clr_text);
    rule.pattern = QRegularExpression(".*");
    rule.format = keywordFormat;
    highlightingRules.append(rule);

    // Function
    functionFormat.setForeground(cte->clr_function);
    rule.pattern = QRegularExpression("([A-Za-z0-9\\$:_])+(\\s)*(?=\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);

    // Keyword
    keywordFormat.setForeground(cte->clr_keyword);
    QStringList keywordPatterns;
    keywordPatterns <<
        "bool" <<
        "char" <<
        "double" <<
        "float" <<
        "short" <<
        "int" <<
        "long" <<
        "unsigned" <<
        "const" <<
        "void" <<
        "virtual" <<
        "inline" <<
        "extern" <<
        "typedef" <<
        "struct" <<
        "uint" <<

        "abstract" << "assert" << "break" << "byte" <<
        "case" << "catch" << "class" << "const" <<
        "continue" << "default" << "do" << "else" <<
        "enum" << "extends" << "final" << "finally" <<
        "for" << "goto" << "if" << "implements" << "import" <<
        "instanceof" << "interface" << "native" <<
        "new" << "package" << "private" << "protected" << "public" <<
        "return" << "static" << "strictfp" << "super" <<
        "switch" << "synchronized" << "this" << "throw" << "throws" <<
        "transient" << "try" << "volatile" << "while" <<

        "defined" <<

        "#ifndef" <<
        "#ifdef" <<
        "#undef" <<
        "#if" <<
        "#elif" <<
        "#else" <<
        "#include" <<
        "#define" <<
        "#endif";
    foreach (const QString &pattern, keywordPatterns) {
        if (pattern.startsWith("#")) {
            rule.pattern = QRegularExpression(pattern);
        }
        else {
            rule.pattern = QRegularExpression("\\b" + pattern + "\\b");
        }
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    //size_t, Uint32
    keywordFormat.setForeground(cte->clr_special_keyword);
    rule.pattern = QRegularExpression("\\b(size_t|Uint32)\\b");
    rule.format = keywordFormat;
    highlightingRules.append(rule);

    // Preprocessor
    quotationFormat.setForeground(cte->clr_function);
    rule.pattern = QRegularExpression("((?<=#define)\\s+\\b[A-z0-9]+\\b)|((?<=#undef)\\s+.*)");
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    // Special Class
    classFormat.setForeground(cte->clr_special_class);
    rule.pattern = QRegularExpression("\\bI[A-Z][A-Za-z]+\\b|\\bSDL_[A-Za-z]+\\b|\\bGL[A-Za-z]+\\b");
    rule.format = classFormat;
    highlightingRules.append(rule);

    // Constants and Numbers
    // all-caps constants (below)
    // \\b[A-Z][A-Z0-9_]*\\b
    quotationFormat.setForeground(cte->clr_number);
    rule.pattern = QRegularExpression("\\bNULL\\b|\\b0x[0-9A-Fa-f]+\\b|\\b\\d+\\.\\d*f{0,1}\\b|\\b\\d+l\\b|\\b\\d+\\b|");
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    // Strings
    quotationFormat.setForeground(cte->clr_string);
    rule.pattern = QRegularExpression("(?s)\".*?\"");
    rule.format = quotationFormat;
    highlightingRules.append(rule);
    rule.pattern = QRegularExpression("(?s)\'.*?\'");
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    // Includes
    rule.pattern = QRegularExpression("(?<=#include)\\s+<.*?>");
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    // Format
    quotationFormat.setForeground(cte->clr_number);
    rule.pattern = QRegularExpression("%[A-z0-9\\.]+");
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    // Comments
    singleLineCommentFormat.setForeground(cte->clr_comment);
    rule.pattern = QRegularExpression("//[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);
    multiLineCommentFormat.setForeground(cte->clr_comment);
    commentStartExpression = QRegularExpression("/\\*");
    commentEndExpression = QRegularExpression("\\*/");
}

void Highlighter::highlightBlock(const QString &text) {
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(commentStartExpression);

    while (startIndex >= 0) {
        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex + match.capturedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}
