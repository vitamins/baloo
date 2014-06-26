/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright (C) 2014  Vishesh Handa <me@vhanda.in>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "termgeneratortest.h"
#include "../termgenerator.h"

#include <QTest>
#include <QDebug>

using namespace Baloo;

namespace {
    QStringList allWords(const Xapian::Document& doc)
    {
        QStringList words;
        for (auto it = doc.termlist_begin(); it != doc.termlist_end(); it++) {
            std::string str = *it;
            words << QString::fromUtf8(str.c_str(), str.length());
        }

        return words;
    }
}
void TermGeneratorTest::testWordBoundaries()
{
    QString str = QString::fromLatin1("The quick (\"brown\") 'fox' can't jump 32.3 feet, right? No-Wrong;xx.txt");

    Xapian::Document doc;
    TermGenerator termGen(&doc);
    termGen.indexText(str);

    QStringList words = allWords(doc);

    QStringList expectedWords;
    expectedWords << "32.3" << "brown" << "can't" << "feet" << "fox" << "jump"
                  << "no" << "quick" << "right" << "the" << "txt" << "wrong"
                  << "xx";

    QCOMPARE(words, expectedWords);
}

void TermGeneratorTest::testAccetCharacters()
{
    QString str = QString::fromLatin1("Como est� K�g");

    Xapian::Document doc;
    TermGenerator termGen(&doc);
    termGen.indexText(str);

    QStringList words = allWords(doc);

    QStringList expectedWords;
    expectedWords << "esta" << "como" << "kug";

    QEXPECT_FAIL("", "Xapian does not handle diarectics", Continue);
    QCOMPARE(words, expectedWords);
}

QTEST_MAIN(TermGeneratorTest)

#include "termgeneratortest.moc"