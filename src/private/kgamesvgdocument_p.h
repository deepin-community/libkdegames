/*
    SPDX-FileCopyrightText: 2007 Mark A. Taff <kde@marktaff.com>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef _KGAMESVGDOCUMENT_P_H_
#define _KGAMESVGDOCUMENT_P_H_

/* @file
    This file contains the regexs for parsing the transform attribute of
    an SVG file using DOM.

    @see: http://www.w3.org/TR/SVG/coords.html#TransformAttribute
*/

/**
 * @brief A regex that matches a single whitespace
 */
static const QString WSP = QStringLiteral("\\s");

/**
 * @brief A regex that matches zero or more whitespace
 */
static const QString WSP_ASTERISK = WSP + QLatin1String("*");

/**
 * @brief A regex that matches a comma
 */
static const QLatin1Char COMMA = QLatin1Char(',');

/**
 * @brief A regex that matches a comma or whitespace
 */
static const QString COMMA_WSP =
    QLatin1String("(?:(?:") + WSP + QLatin1Char('+') + COMMA + QLatin1Char('?') + WSP + QLatin1String("*)|(?:") + COMMA + WSP + QLatin1String("*))");

/**
 * @brief A regex that matches a number
 */
static const QString NUMBER = QStringLiteral("(?:(?:[-|\\+]?\\d+(?:\\.)*\\d*(?:e)?[-|\\+]?\\d*)|(?:[-|\\+]?(?:\\.)+\\d*(?:e)?[-|\\+]?\\d*))");
// Do not wrap the above line!

/**
 * @brief A regex that matches opening parenthesis
 */
static const QString OPEN_PARENS = QStringLiteral("\\(");

/**
 * @brief A regex that matches closing parenthesis
 */
static const QString CLOSE_PARENS = QStringLiteral("\\)");

// clang-format off
/**
 * @brief A regex that matches a matrix transform
 */
static const QString MATRIX = QLatin1String("(matrix)" ) + WSP_ASTERISK + OPEN_PARENS + WSP_ASTERISK +
                QLatin1Char( '(' ) + NUMBER + QLatin1Char( ')' ) + COMMA_WSP +
                QLatin1Char( '(' ) + NUMBER + QLatin1Char( ')' ) + COMMA_WSP +
                QLatin1Char( '(' ) + NUMBER + QLatin1Char( ')' ) + COMMA_WSP +
                QLatin1Char( '(' ) + NUMBER + QLatin1Char( ')' ) + COMMA_WSP +
                QLatin1Char( '(' ) + NUMBER + QLatin1Char( ')' ) + COMMA_WSP +
                QLatin1Char( '(' ) + NUMBER + QLatin1Char( ')' ) + WSP_ASTERISK + CLOSE_PARENS;

/**
 * @brief A regex that matches a translate transform
 */
static const QString TRANSLATE = QLatin1String("(translate)" ) + WSP_ASTERISK + OPEN_PARENS + WSP_ASTERISK +
                QLatin1Char( '(' ) + NUMBER + QLatin1Char( ')' ) +
                QLatin1String( "(?:" ) + COMMA_WSP + QLatin1Char( '(' ) + NUMBER + QLatin1Char( ')' ) + QLatin1String( ")?" ) + WSP_ASTERISK + CLOSE_PARENS;

/**
 * @brief A regex that matches scale transform
 */
static const QString SCALE = QLatin1String("(scale)" ) + WSP_ASTERISK + OPEN_PARENS + WSP_ASTERISK +
                QLatin1Char( '(' ) + NUMBER + QLatin1Char( ')' ) +
                QLatin1String( "(?:" ) + COMMA_WSP + QLatin1Char( '(' ) + NUMBER + QLatin1Char( ')' ) + QLatin1String( ")?" ) + WSP_ASTERISK + CLOSE_PARENS;

/**
 * @brief A regex that matches rotate transform
 */
static const QString ROTATE = QLatin1String("(rotate)" ) + WSP_ASTERISK + OPEN_PARENS + WSP_ASTERISK +
                QLatin1Char( '(' ) + NUMBER + QLatin1Char( ')' ) + QLatin1String( "(?:" ) + COMMA_WSP +
                QLatin1Char( '(' ) + NUMBER + QLatin1Char( ')' ) + COMMA_WSP +
                QLatin1Char( '(' ) + NUMBER + QLatin1Char( ')' ) +  QLatin1String( ")?" ) + WSP_ASTERISK + CLOSE_PARENS;

/**
 * @brief A regex that matches skewX transform
 */
static const QString SKEW_X = QLatin1String( "(skewX)" ) + WSP_ASTERISK + OPEN_PARENS + WSP_ASTERISK +
                QLatin1Char( '(' ) + NUMBER + QLatin1Char( ')' ) + WSP_ASTERISK + CLOSE_PARENS;

/**
 * @brief A regex that matches skewY transform
 */
static const QString SKEW_Y =
    QLatin1String("(skewY)") + WSP_ASTERISK + OPEN_PARENS + WSP_ASTERISK + QLatin1Char('(') + NUMBER + QLatin1Char(')') + WSP_ASTERISK + CLOSE_PARENS;

/**
 * @brief A regex that matches any single transform
 */
static const QString TRANSFORM = QLatin1String("(?:" ) + MATRIX + QLatin1String( "|" ) + TRANSLATE + QLatin1String( "|" ) + SCALE + QLatin1String( "|" ) +
                ROTATE + QLatin1String( "|" ) + SKEW_X + QLatin1String( "|" ) + SKEW_Y + QLatin1String( ")");

/**
 * @brief A regex that matches the entire transform attribute
 */
static const QString TRANSFORMS = QLatin1String("(?:" ) + TRANSFORM + QLatin1String( "|" ) + QLatin1String( "(?:" ) + TRANSFORM +
                COMMA_WSP + QLatin1String( "+)*" ) + TRANSFORM + QLatin1String( ")");
// clang-format on

#endif // _KGAMESVGDOCUMENT_P_H_
