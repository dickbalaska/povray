/******************************************************************************
 * imagedisplayer.h - Display a graphic in an "editor" page
 *
 * qtpovray - A Qt IDE frontend for POV-Ray
 * Copyright(c) 2018 - Dick Balaska, and BuckoSoft.
 *
 * qtpovray is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * qtpovray is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/
#ifndef IMAGEDISPLAYER_H
#define IMAGEDISPLAYER_H

#include <QScrollArea>

class ImageDisplayer :  public QScrollArea
{
	Q_OBJECT
public:
	explicit ImageDisplayer(QWidget *parent = nullptr);

//	virtual void setEditorFocus() { setFocus(); }

	void	setFilePath(const QString& filePath);
	const QString& getFilePath();
	void	setFileName(const QString& filePath);
	const QString& getFileName();

	void	loadImage();
signals:

public slots:

private:
	QString			m_filePath;		// absolute path to the file
	QString			m_fileName;		// just the filename displayed in the tab
};

inline void ImageDisplayer::setFilePath(const QString& filePath) { this->m_filePath = filePath; }
inline const QString& ImageDisplayer::getFilePath() { return(m_filePath); }
inline void ImageDisplayer::setFileName(const QString& fileName) { this->m_fileName = fileName; }
inline const QString& ImageDisplayer::getFileName() { return(m_fileName); }

#endif // IMAGEDISPLAYER_H
