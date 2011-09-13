/*
 * Copyright (C) by Duncan Mac-Vicar P. <duncan@kde.org>
 *
 * Originally based on example copyright (c) Ashish Shukla
 * Ported to use QSocketNotifier later instead of a thread loop
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

#include <sys/inotify.h>
#include <cerrno>
#include <unistd.h>
#include <QDebug>
#include <QStringList>
#include <QSocketNotifier>

#include "inotify.h"

// Buffer Size for read() buffer
#define DEFAULT_READ_BUFFERSIZE 2048

namespace Mirall {

INotify::INotify(int mask) : _mask(mask)
{
    _fd = inotify_init();
    _notifier = new QSocketNotifier(_fd, QSocketNotifier::Read);
    QObject::connect(_notifier, SIGNAL(activated(int)), SLOT(slotActivated(int)));
    _buffer_size = DEFAULT_READ_BUFFERSIZE;
    _buffer = (char *) malloc(_buffer_size);
}

void INotify::slotActivated(int fd)
{
    int len;
    struct inotify_event* event;
    int i;
    int error;

    do {
        len = read(_fd, _buffer, _buffer_size);
        error = errno;
        /**
         * From inotify documentation:
         *
         * The behavior when the buffer given to read(2) is too
         * small to return information about the next event
         * depends on the kernel version: in kernels  before 2.6.21,
         * read(2) returns 0; since kernel 2.6.21, read(2) fails with
         * the error EINVAL.
         */
        if (len < 0 && error == EINVAL)
        {
            // double the buffer size
            qWarning() << "buffer size too small";
            _buffer_size *= 2;
            _buffer = (char *) realloc(_buffer, _buffer_size);
            /* and try again ... */
            continue;
        }
    } while (false);

    /* TODO handle len == 0 */

    // reset counter
    i = 0;
    // while there are enough events in the buffer
    while(i + sizeof(struct inotify_event) < len) {
        // cast an inotify_event
        event = (struct inotify_event*)&_buffer[i];
        // with the help of watch descriptor, retrieve, corresponding INotify
        if (event == NULL) {
            qDebug() << "NULL event";
            continue;
        }

        // fire event
        if (event->len > 0) {
            QStringList paths(_wds.keys(event->wd));
            foreach (QString path, paths)
                emit notifyEvent(event->mask, event->cookie, path + "/" + QString::fromUtf8(event->name));
        }
        else
            qWarning() << "n is NULL";

        // increment counter
        i += sizeof(struct inotify_event) + event->len;
    }
}

INotify::~INotify()
{
    // Remove all inotify watchs.
    QString key;
    foreach (key, _wds.keys())
        inotify_rm_watch(_fd, _wds.value(key));

    close(_fd);
    free(_buffer);
}

void INotify::addPath(const QString &path)
{
    // Add an inotify watch.
    int wd = inotify_add_watch(_fd, path.toAscii().constData(), _mask);
    _wds[path] = wd;
}

void INotify::removePath(const QString &path)
{
    // Remove the inotify watch.
    inotify_rm_watch(_fd, _wds[path]);
    _wds.remove(path);
}

QStringList INotify::directories() const
{
    return _wds.keys();
}

} // ns mirall

#include "inotify.moc"
