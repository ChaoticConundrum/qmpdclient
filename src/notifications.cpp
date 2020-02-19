/*
 * QMPDClient - An MPD client written in Qt 4.
 * Copyright (C) 2005-2008 Håvard Tautra Knutsen <havtknut@tihlde.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "config.h"
#include "debug.h"
#include "notifications.h"
#include "richtext.h"
#include "mpd.h"

#include "coverartdialog.h"
#include <QApplication>
#include <QDesktopWidget>

Notifications::Notifications(TrayIcon *trayIcon, QObject *parent)
	: QObject(parent),
	  m_coverArt(new CoverArtDialog(nullptr)),
	  trayIcon(trayIcon), notifyIcon(":/icons/64x64/qmpdclient.png") {
	setObjectName("notifications");
	connect(MPD::instance(), SIGNAL(playingSongUpdated(const MPDSong &)), this, SLOT(setSong(const MPDSong &)));
}

QString Notifications::name(Type t) {
	switch (t) {
		case FREEDESKTOP:
			return "Freedesktop";
		default:
			return "QMPDClient";
	}
}

QString Notifications::makeTitle(const MPDSong &s) {
	int desktopWidth = QApplication::desktop()->width();
	QString title = elideRichText("", s.title().isEmpty() ? s.filename() : s.title()  , "", desktopWidth / 2) + "\n";
	QString artist = elideRichText("", s.artist(), "", desktopWidth / 4);
	QString album = elideRichText("", s.album(), "", desktopWidth / 4);
	
	if (!artist.isEmpty())
		title += artist + "\n";
	if (!album.isEmpty())
		title += album;
	return title;
}

void Notifications::notify(const QString &text) {
	trayIcon->showMessage(qApp->applicationName(), text, notifyIcon, Config::instance()->notificationsTimeout()*1000);
}

void Notifications::setSong(const MPDSong &s) {
	if (m_previousSong.isNull() || m_previousSong == s || !Config::instance()->notificationsEnabled() || Config::instance()->notificationsTimeout() < 1) {
		m_previousSong = s;
		return;
	}
	if(!s.isNull())
	{
		m_coverArt->setSong(s);
		notify(makeTitle(s));
	}
	m_previousSong = s;
}

QList<Notifications::Type> Notifications::notifiers()
{
	return QList<Type>() << FREEDESKTOP;
}
