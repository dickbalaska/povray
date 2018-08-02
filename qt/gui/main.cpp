/******************************************************************************
 * main.cpp - The Qt main
 *
 * qtpov - A Qt IDE frontend for POV-Ray
 * Copyright(c) 2017 - Dick Balaska, and BuckoSoft.
 *
 * qtpov is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * qtpov is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/

#include "mainwindow.h"
#include <QApplication>
#include <QTime>
#include "qtpovrayversion.h"

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	QByteArray localMsg = msg.toLocal8Bit();
	QByteArray time = QTime::currentTime().toString("hh:mm:ss.zzz").toLocal8Bit();
	switch (type) {
	case QtDebugMsg:
#ifdef _DEBUG
		//fprintf(stderr, "[%s] %s (%s:%u)\n", time.constData(), localMsg.constData(), context.file, context.line);
		fprintf(stderr, "[%s] DEBG %s\n", time.constData(), localMsg.constData());
#endif
		break;
	case QtInfoMsg:
		fprintf(stderr, "[%s] INFO %s (%s:%u)\n", time.constData(), localMsg.constData(), context.file, context.line);
		break;
	case QtWarningMsg:
		fprintf(stderr, "[%s] WARN %s (%s:%u)\n", time.constData(), localMsg.constData(), context.file, context.line);
		break;
	case QtCriticalMsg:
		fprintf(stderr, "[%s] CRIT %s (%s:%u)\n", time.constData(), localMsg.constData(), context.file, context.line);
		break;
	case QtFatalMsg:
		fprintf(stderr, "[%s] FATL %s (%s:%u, %s)\n", time.constData(), localMsg.constData(), context.file, context.line, context.function);
		abort();
	}
}

int main(int argc, char *argv[])
{
	if (argc > 1) {
		if (!strncmp(argv[1], "-version", 8) || !strncmp(argv[1], "--version", 9)) {
			printf("qtpovray %s\n", VERSION);
			exit(0);
		}
	}
	qInstallMessageHandler(myMessageOutput);
	QApplication a(argc, argv);
	setlocale(LC_ALL,"C");
    MainWindow w;
    w.show();

    return a.exec();
}
