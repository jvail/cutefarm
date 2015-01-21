/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the example classes of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>

#include "highlighter.h"

//! [0]
Highlighter::Highlighter(QTextDocument *parent,  bool hasUseNotRules)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;


    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    if (hasUseNotRules) {
    	donotuseKeywordFormat.setForeground(Qt::red);
    	donotuseKeywordFormat.setFontWeight(QFont::Bold);
    }
    else {
    	donotuseKeywordFormat.setForeground(Qt::darkBlue);
    	donotuseKeywordFormat.setFontWeight(QFont::Bold);
    }

    dataTypeFormat.setForeground(QColor("LightSlateGrey"));
    dataTypeFormat.setFontWeight(QFont::Bold);

    QStringList dataTypePatterns;
    dataTypePatterns << "\\bINTEGER\\b" << "\\bBOOL\\b" << "\\bBOOLEAN\\b"
                    << "\\bDOUBLE\\b" << "\\bTEXT\\b" << "\\bCHAR\\b"
                    << "\\bVARCHAR\\b" << "\\bDATE\\b" << "\\bREAL\\b";
    foreach (const QString &pattern, dataTypePatterns) {
         rule.pattern = QRegExp(pattern, Qt::CaseInsensitive);
         rule.format = dataTypeFormat;
         highlightingRules.append(rule);
     }


    QStringList keywordPatterns;
    keywordPatterns << "\\bSELECT\\b" << "\\bWHERE\\b" << "\\bFROM\\b"
                    << "\\bAS\\b" << "\\bORDER BY\\b" << "\\bAND\\b"
                    << "\\bOR\\b" << "\\bIN\\b" << "\\bNOT\\b"
                    << "\\bDISTINCT\\b" << "\\bNULL\\b"  << "\\bIS\\b"
                    << "\\bASC\\b" << "\\bDESC\\b" << "\\bCASE\\b"
                    << "\\bWHEN\\b" << "\\bTHEN\\b" << "\\bELSE\\b"
                    << "\\bEND\\b" << "\\bGROUP BY\\b";
    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern, Qt::CaseInsensitive);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    QStringList donotuseKeywordPatterns;
    donotuseKeywordPatterns << "\\bINSERT\\b" << "\\bUPDATE\\b" << "\\bDELETE\\b" << "\\bUNIQUE\\b"
                    << "\\bDROP\\b" << "\\bCREATE\\b" << "\\bPRAGMA\\b"
                    << "\\bREPLACE\\b" << "\\bPRIMARY\\b" << "\\bKEY\\b"
                    << "\\bAUTOINCREMENT\\b" << "\\bKEY\\b"  << "\\bCONSTRAINT\\b"
                    << "\\bCHECK\\b" << "\\bREFERENCES\\b" << "\\bDEFAULT\\b"
                    << "\\bON\\b" << "\\bCASCADE\\b" << "\\bRESTRICT\\b"
                    << "\\bTABLE\\b" << "\\bVIEW\\b" << "\\bTRIGGER\\b"
                    << "\\bINDEX\\b" << "\\bDEFERRABLE\\b" <<  "\\bINITIALLY\\b" << "\\bDEFERRED\\b";
    foreach (const QString &pattern, donotuseKeywordPatterns) {
        rule.pattern = QRegExp(pattern, Qt::CaseInsensitive);
        rule.format = donotuseKeywordFormat;
        highlightingRules.append(rule);
    }
//! [1]

//! [2]
//    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegExp("[A-Za-z-_]+_id\\b");
    rule.format = classFormat;
    highlightingRules.append(rule);
//! [2]

//! [3]
    singleLineCommentFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("--[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::darkGreen);
//! [3]

//! [4]
//    quotationFormat.setFontItalic(true);
    quotationFormat.setForeground(Qt::blue);
    rule.pattern = QRegExp("\\b[0-9-.]+\\b");
    rule.format = quotationFormat;
    highlightingRules.append(rule);
//! [4]

//! [5]
    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::blue);
    rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);
//! [5]

//! [6]
    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");
}
//! [6]

//! [7]
void Highlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
//! [7] //! [8]
    setCurrentBlockState(0);
//! [8]

//! [9]
    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = commentStartExpression.indexIn(text);

//! [9] //! [10]
    while (startIndex >= 0) {
//! [10] //! [11]
        int endIndex = commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
    }
}
//! [11]
