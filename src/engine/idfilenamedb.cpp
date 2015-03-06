/*
   This file is part of the KDE Baloo project.
 * Copyright (C) 2015  Vishesh Handa <vhanda@kde.org>
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

#include "idfilenamedb.h"

#include <QDataStream>

using namespace Baloo;

IdFilenameDB::IdFilenameDB(MDB_txn* txn)
    : m_txn(txn)
{
    Q_ASSERT(txn != 0);

    int rc = mdb_dbi_open(txn, "idfilename", MDB_CREATE | MDB_INTEGERKEY, &m_dbi);
    Q_ASSERT_X(rc == 0, "IdFilenameDB", mdb_strerror(rc));
}

IdFilenameDB::~IdFilenameDB()
{
    mdb_dbi_close(mdb_txn_env(m_txn), m_dbi);
}

void IdFilenameDB::put(quint64 docId, const FilePath& path)
{
    Q_ASSERT(docId > 0);
    Q_ASSERT(!path.name.isEmpty());

    MDB_val key;
    key.mv_size = sizeof(quint64);
    key.mv_data = static_cast<void*>(&docId);

    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << path.parentId << path.name;

    MDB_val val;
    val.mv_size = data.size();
    val.mv_data = static_cast<void*>(data.data());

    int rc = mdb_put(m_txn, m_dbi, &key, &val, 0);
    Q_ASSERT_X(rc == 0, "IdFilenameDB::put", mdb_strerror(rc));
}

IdFilenameDB::FilePath IdFilenameDB::get(quint64 docId)
{
    Q_ASSERT(docId > 0);

    MDB_val key;
    key.mv_size = sizeof(quint64);
    key.mv_data = static_cast<void*>(&docId);

    FilePath path;

    MDB_val val;
    int rc = mdb_get(m_txn, m_dbi, &key, &val);
    if (rc == MDB_NOTFOUND) {
        return path;
    }
    Q_ASSERT_X(rc == 0, "IdfilenameDB::get", mdb_strerror(rc));

    QByteArray data = QByteArray::fromRawData(static_cast<char*>(val.mv_data), val.mv_size);
    QDataStream stream(&data, QIODevice::ReadOnly);

    stream >> path.parentId;
    stream >> path.name;
    return path;
}

void IdFilenameDB::del(quint64 docId)
{
    Q_ASSERT(docId > 0);

    MDB_val key;
    key.mv_size = sizeof(quint64);
    key.mv_data = static_cast<void*>(&docId);

    int rc = mdb_del(m_txn, m_dbi, &key, 0);
    Q_ASSERT_X(rc == 0, "IdfilenameDB::del", mdb_strerror(rc));
}
