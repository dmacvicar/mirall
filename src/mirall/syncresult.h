/*
 * Copyright (C) by Duncan Mac-Vicar P. <duncan@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 */

#ifndef MIRALL_SYNCRESULT_H
#define MIRALL_SYNCRESULT_H

#include <QStringList>

namespace Mirall
{

class SyncResult
{
public:
    enum Result
    {
        Success,
        Error
    };

    SyncResult(Result result);
    ~SyncResult();

    Result result() const;

private:
    Result _result;
    /**
     * when the sync tool support this...
     */
    QStringList _deletedSource;
    QStringList _deletedDestination;
};

}

#endif
