/*
   This file is part of the KDE Baloo project.
   Copyright (C) 2011 Sebastian Trueg <trueg@kde.org>
   Copyright (C) 2013-2014 Vishesh Handa <vhanda@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) version 3, or any
   later version accepted by the membership of KDE e.V. (or its
   successor approved by the membership of KDE e.V.), which shall
   act as a proxy defined in Section 6 of version 3 of the license.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "pendingfilequeuetest.h"
#include "../pendingfilequeue.h"

#include <qtest.h>
#include <qsignalspy.h>
#include <qtimer.h>
#include <QDebug>

using namespace Baloo;

PendingFileQueueTest::PendingFileQueueTest()
{
}

void PendingFileQueueTest::testTimeout()
{
    QString myUrl("/tmp");

    PendingFileQueue queue;
    queue.setMinimumTimeout(2);

    QSignalSpy spy(&queue, SIGNAL(indexFile(QString)));

    PendingFile file(myUrl);
    file.setModified();
    queue.enqueue(file);

    // The signal should be emitted immediately
    QTest::qWait(20);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.takeFirst().first().toString(), myUrl);

    // Enqueue the url again. This time it should wait for should wait for the
    // minimumTimeout
    queue.enqueue(file);

    QTest::qWait(1000);
    QVERIFY(spy.isEmpty());

    QTest::qWait(1100);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.takeFirst().first().toString(), myUrl);
}

void PendingFileQueueTest::testRequeue()
{
    QString myUrl("/tmp");

    PendingFileQueue queue;
    queue.setMinimumTimeout(2);
    queue.setMaximumTimeout(5);

    QSignalSpy spy(&queue, SIGNAL(indexFile(QString)));

    PendingFile file(myUrl);
    file.setModified();
    queue.enqueue(file);

    // The signal should be emitted immediately
    QTest::qWait(20);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.takeFirst().first().toString(), myUrl);

    // Send many events
    queue.enqueue(file);
    QTest::qWait(20);
    queue.enqueue(file);
    QTest::qWait(20);
    queue.enqueue(file);
    QTest::qWait(20);

    QTest::qWait(3500);
    QVERIFY(spy.isEmpty());

    QTest::qWait(1600);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.takeFirst().first().toString(), myUrl);
}

QTEST_MAIN(PendingFileQueueTest)