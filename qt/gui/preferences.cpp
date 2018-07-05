/******************************************************************************
 * preferences.cpp - The dialog box to control user preferences
 *
 * qtpovray - A Qt GUI IDE frontend for POV-Ray
 * Copyright(c) 2017 - Dick Balaska, and BuckoSoft.
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

#include <QtWidgets>
#include <QDebug>

#include "mainwindow.h"
#include "workspace.h"
#include "preferences.h"

Preferences::Preferences(MainWindow *parent, PreferenceData* d)
	: QDialog(parent)
{
	m_mainWindow = parent;
	m_prefData = d;
	//qDebug() << "PreColor " << data->getEditorColors()->comment.getColor() << &data->getEditorColors()->comment;
	setWindowTitle(tr("Preferences"));
	m_iconOk  = new QIcon(":/resources/icons/Check_16x16.png");
	m_iconBad = new QIcon(":/resources/icons/Cancel_16x16.png");

	QTabWidget* tabWidget = new QTabWidget(this);
	m_systemTab = new SystemTab(this);
	tabWidget->addTab(m_systemTab, tr("System"));
	m_editorTab = new EditorTab(this);
	tabWidget->addTab(m_editorTab, tr("Editor"));
	m_colorTab = new ColorTab(this);
	tabWidget->addTab(m_colorTab, tr("Colors"));
	m_keyTab = new KeyTab(this);
	tabWidget->addTab(m_keyTab, tr("Keys"));

	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->setObjectName("prefs:mainLayout");
	mainLayout->addWidget(tabWidget);
	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
										 | QDialogButtonBox::Cancel);
	connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
	mainLayout->addWidget(buttonBox);
	setLayout(mainLayout);
#ifdef USE_WEBSOCKETS
	mainWindow->setPrefVersionWidget(povrayTab->getPovrayBanner());
#endif
	m_systemTab->validateData();
	//qDebug() << "PostColor" << data->getEditorColors()->comment.getColor() << &data->getEditorColors()->comment;
	QRect rect = m_mainWindow->getWorkspace()->getPreferencesRect();
	if (!rect.topLeft().isNull()) {
		move(rect.topLeft());
		resize(rect.size());
	}
}

Preferences::~Preferences()
{
	m_mainWindow->getWorkspace()->setPreferencesRect(QRect(pos(), size()));
}
ColorTab::ColorTab(Preferences* parent)
	: QWidget(parent),
	  parent(parent) {
	//PreferenceData* data = parent->data;
	EditorColors* editorColors = parent->m_prefData->getEditorColors();
	QHBoxLayout* mainLayout = new QHBoxLayout;
	QVBoxLayout* leftLayout = new QVBoxLayout;
	QGroupBox* gridGroupBox = new QGroupBox(tr("Common"));
	QGridLayout* layout = new QGridLayout(this);

	//qDebug() << "ContColor" << editorColors->comment.getColor() << &editorColors->comment;
	layout->addWidget(new QLabel("<b>B</b>", this), 0, 2, Qt::AlignHCenter);
	addButton(layout, commentButton, 1, &editorColors->comment, tr("Comments:"));
	addButton(layout, stringButton,  2, &editorColors->string,  tr("Strings:"));
	layout->setVerticalSpacing(10);

	gridGroupBox->setLayout(layout);
	leftLayout->addWidget(gridGroupBox);
	leftLayout->addStretch(1);
	mainLayout->addLayout(leftLayout);

	QVBoxLayout* rightLayout = new QVBoxLayout;
	gridGroupBox = new QGroupBox(tr("Keywords"));
	layout = new QGridLayout(this);
	layout->addWidget(new QLabel("<b>B</b>", this), 0, 2, Qt::AlignHCenter);
	addButton(layout, commonButton,		1, &editorColors->common, tr("Common:"));
	addButton(layout, objectButton,		2, &editorColors->object, tr("Object:"));
	addButton(layout, modifierButton,	3, &editorColors->modifier, tr("Modifier:"));
	addButton(layout, textureButton,	4, &editorColors->texture, tr("Texture:"));
	addButton(layout, colorButton,		5, &editorColors->color, tr("Color:"));
	addButton(layout, mathButton,		6, &editorColors->math, tr("Math:"));
	layout->setVerticalSpacing(10);

	gridGroupBox->setLayout(layout);
	rightLayout->addWidget(gridGroupBox);
	rightLayout->addStretch(1);
	mainLayout->addLayout(rightLayout);

	this->setLayout(mainLayout);
}

void ColorTab::addButton(QGridLayout *layout, ColorButton* button, int row,
						 Highlight* highlight, const QString& label) {
	layout->addWidget(new QLabel(label, parent), row, 0, Qt::AlignLeft);
	QPixmap* qpm = new QPixmap(52,40);
	qpm->fill(highlight->getColor());
	button = new ColorButton(QIcon(*qpm), this, highlight);
	layout->addWidget(button, row, 1, Qt::AlignCenter);
	QCheckBox* check = new QCheckBox(this);
	layout->addWidget(check, row, 2, Qt::AlignHCenter);
	check->setChecked(highlight->isBold());
	connect(check, SIGNAL(stateChanged(int)), button, SLOT(boldChecked(int)));
	//qDebug() << "addBColor:" << highlight->getColor() << highlight;
}

ColorButton::ColorButton(const QIcon &icon, QWidget *parent, Highlight *hl)
	: QPushButton(icon, "", parent)
{
	this->highlight = hl;
	//qDebug() << "Color:" << highlight->getColor() << highlight;
	connect(this, SIGNAL(clicked(bool)), this, SLOT(buttonClicked(bool)));
}

void ColorButton::buttonClicked(bool) {
	qDebug() << "Color-" << highlight->getColor() << highlight;
	QColor color = QColorDialog::getColor(this->highlight->getColor(), this, QString(),
										  QColorDialog::DontUseNativeDialog);
	if (color.isValid()) {
		highlight->setColor(color);
		QPixmap* qpm = new QPixmap(52,40);
		qpm->fill(highlight->getColor());
		this->setIcon(QIcon(*qpm));
	}
}

void ColorButton::boldChecked(int state) {
	this->highlight->setBold(state == Qt::Checked);
}

SystemTab::SystemTab(Preferences* parent)
	: QWidget(parent),
	  parent(parent) {
	PreferenceData* prefData = parent->m_prefData;

	QVBoxLayout* mainLayout = new QVBoxLayout;
	QGroupBox* gridGroupBox = new QGroupBox(tr("Directories"));
	QGridLayout* layout = new QGridLayout(this);

#ifdef USE_WEBSOCKETS
	povrayExecutableStatus = new QLabel(this);
	povrayExecutableStatus->setPixmap(parent->iconBad->pixmap(16));
	layout->addWidget(povrayExecutableStatus, 0, 0, 1, 1, Qt::AlignLeft);
	layout->addWidget(new QLabel(tr("Executable"), parent), 0, 1, 1, 1, Qt::AlignLeft);
	povrayExecutable = new QLineEdit(this);
	povrayExecutable->setText(prefData->getPovrayExecutable());
	layout->addWidget(povrayExecutable, 0, 2, 1, 1);
	QPushButton* browseExeButton = new QPushButton(tr("Browse"), this);
	layout->addWidget(browseExeButton, 0, 3, 1, 1, Qt::AlignRight);
#endif

	povrayIncludesStatus = new QLabel(this);
	povrayIncludesStatus->setPixmap(parent->m_iconBad->pixmap(16));
	layout->addWidget(povrayIncludesStatus, 1, 0, 1, 1, Qt::AlignLeft);
	layout->addWidget(new QLabel(tr("Includes"), this), 1, 1, 1, 1, Qt::AlignLeft);
	povrayIncludes = new QLineEdit(this);
	povrayIncludes->setText(prefData->getPovrayIncludes());
	layout->addWidget(povrayIncludes, 1, 2, 1, 1);
	QPushButton* browseIncButton = new QPushButton(tr("Browse"), this);
	layout->addWidget(browseIncButton, 1, 3, 1, 1, Qt::AlignRight);

	povrayInsertMenuStatus = new QLabel(this);
	povrayInsertMenuStatus->setPixmap(parent->m_iconBad->pixmap(16));
	layout->addWidget(povrayInsertMenuStatus, 2, 0, 1, 1, Qt::AlignLeft);
	layout->addWidget(new QLabel(tr("Insert Menu"), this), 2, 1, 1, 1, Qt::AlignLeft);
	povrayInsertMenu = new QLineEdit(this);
	povrayInsertMenu->setText(prefData->getPovrayInsertMenu());
	layout->addWidget(povrayInsertMenu, 2, 2, 1, 1);
	QPushButton* browseInsButton = new QPushButton(tr("Browse"), this);
	layout->addWidget(browseInsButton, 2, 3, 1, 1, Qt::AlignRight);
	//gridGroupBox->setLayout(layout);

	povraySceneDirectoryStatus = new QLabel(this);
	povraySceneDirectoryStatus->setPixmap(parent->m_iconBad->pixmap(16));
	layout->addWidget(povraySceneDirectoryStatus, 3, 0, 1, 1, Qt::AlignLeft);
	layout->addWidget(new QLabel(tr("Sample Scenes"), this), 3, 1, 1, 1, Qt::AlignLeft);
	povraySceneDirectory = new QLineEdit(this);
	povraySceneDirectory->setText(prefData->getPovraySceneDirectory());
	layout->addWidget(povraySceneDirectory, 3, 2, 1, 1);
	QPushButton* browseIndButton = new QPushButton(tr("Browse"), this);
	layout->addWidget(browseIndButton, 3, 3, 1, 1, Qt::AlignRight);
	//gridGroupBox->setLayout(layout);

	povrayHelpDirectoryStatus = new QLabel(this);
	povrayHelpDirectoryStatus->setPixmap(parent->m_iconBad->pixmap(16));
	layout->addWidget(povrayHelpDirectoryStatus, 4, 0, 1, 1, Qt::AlignLeft);
	layout->addWidget(new QLabel(tr("POV-Ray Help"), this), 4, 1, 1, 1, Qt::AlignLeft);
	povrayHelpDirectory = new QLineEdit(this);
	povrayHelpDirectory->setText(prefData->getPovrayHelpDirectory());
	layout->addWidget(povrayHelpDirectory, 4, 2, 1, 1);
	QPushButton* browseInhButton = new QPushButton(tr("Browse"), this);
	layout->addWidget(browseInhButton, 4, 3, 1, 1, Qt::AlignRight);
	//gridGroupBox->setLayout(layout);

	qtpovrayHelpDirectoryStatus = new QLabel(this);
	qtpovrayHelpDirectoryStatus->setPixmap(parent->m_iconBad->pixmap(16));
	layout->addWidget(qtpovrayHelpDirectoryStatus, 5, 0, 1, 1, Qt::AlignLeft);
	layout->addWidget(new QLabel(tr("Qtpovray Help"), this), 5, 1, 1, 1, Qt::AlignLeft);
	qtpovrayHelpDirectory = new QLineEdit(this);
	qtpovrayHelpDirectory->setText(prefData->getQtpovrayHelpDirectory());
	layout->addWidget(qtpovrayHelpDirectory, 5, 2, 1, 1);
	QPushButton* browseInqButton = new QPushButton(tr("Browse"), this);
	layout->addWidget(browseInqButton, 5, 3, 1, 1, Qt::AlignRight);

	gridGroupBox->setLayout(layout);
	mainLayout->addWidget(gridGroupBox);

#ifdef USE_WEBSOCKETS
	gridGroupBox = new QGroupBox(tr("POV-Ray Banner"));
	povrayBanner = new QTextEdit(this);
	povrayBanner->setReadOnly(true);
	layout = new QGridLayout(this);
	layout->addWidget(povrayBanner);
	gridGroupBox->setLayout(layout);
	mainLayout->addWidget(gridGroupBox);
#else
	mainLayout->addStretch(1);
#endif

#if 0
	QPushButton* benchmarkButton = new QPushButton(tr("Run Benchmark"), this);
	mainLayout->addWidget(benchmarkButton);
#endif

#ifdef USE_WEBSOCKETS
	connect(browseExeButton, SIGNAL(clicked(bool)), this, SLOT(browseExeClicked(bool)));
	connect(povrayExecutable, SIGNAL(textEdited(QString)), this, SLOT(textExeEdited(QString)));
#endif
	connect(browseIncButton, SIGNAL(clicked(bool)), this, SLOT(browseIncClicked(bool)));
	connect(browseInsButton, SIGNAL(clicked(bool)), this, SLOT(browseInsClicked(bool)));
	connect(browseIndButton, SIGNAL(clicked(bool)), this, SLOT(browseIndClicked(bool)));
	connect(browseInhButton, SIGNAL(clicked(bool)), this, SLOT(browseInhClicked(bool)));
	connect(browseInqButton, SIGNAL(clicked(bool)), this, SLOT(browseInqClicked(bool)));
	connect(povrayIncludes, SIGNAL(textEdited(QString)), this, SLOT(textIncEdited(QString)));
	connect(povrayInsertMenu, SIGNAL(textEdited(QString)), this, SLOT(textInsEdited(QString)));
	connect(povraySceneDirectory, SIGNAL(textEdited(QString)), this, SLOT(textIndEdited(QString)));
	connect(povrayHelpDirectory, SIGNAL(textEdited(QString)), this, SLOT(textInhEdited(QString)));
	connect(qtpovrayHelpDirectory, SIGNAL(textEdited(QString)), this, SLOT(textInqEdited(QString)));
#if 0
	connect(benchmarkButton, SIGNAL(clicked(bool)), this, SLOT(benchmarkButtonClicked(bool)));
#endif
	this->setLayout(mainLayout);
}

void SystemTab::validateData() {
	validateInc();
	validateIns();
	validateInd();
	validateInh();
	validateInq();
#ifdef USE_WEBSOCKETS
	validateExe();
#endif
}

#ifdef USE_WEBSOCKETS
void PovrayTab::browseExeClicked(bool) {
	QString dir = QFileDialog::getOpenFileName(this, tr("Select POV-Ray Executable"));
	if (!dir.isEmpty()) {
		povrayExecutable->setText(dir);
		parent->prefData->setPovrayExecutable(dir);
	}
	validateExe();
}
#endif

void SystemTab::browseIncClicked(bool) {
	QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"));
	if (!dir.isEmpty()) {
		povrayIncludes->setText(dir);
		parent->m_prefData->setPovrayIncludes(dir);
	}
	validateInc();
	qDebug() << "browseIncClicked" << dir;
}
void SystemTab::browseInsClicked(bool) {
	QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"));
	if (!dir.isEmpty()) {
		povrayInsertMenu->setText(dir);
		parent->m_prefData->setPovrayInsertMenu(dir);
	}
	validateIns();
}
void SystemTab::browseIndClicked(bool) {
	QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"));
	if (!dir.isEmpty()) {
		povraySceneDirectory->setText(dir);
		parent->m_prefData->setPovraySceneDirectory(dir);
	}
	validateIns();
}
void SystemTab::browseInhClicked(bool) {
	QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"));
	if (!dir.isEmpty()) {
		povrayHelpDirectory->setText(dir);
		parent->m_prefData->setPovrayHelpDirectory(dir);
	}
	validateInh();
}
void SystemTab::browseInqClicked(bool) {
	QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"));
	if (!dir.isEmpty()) {
		qtpovrayHelpDirectory->setText(dir);
		parent->m_prefData->setQtpovrayHelpDirectory(dir);
	}
	validateInq();
}

#ifdef USE_WEBSOCKETS
void SystemTab::textExeEdited(const QString& text) {
	parent->prefData->setPovrayExecutable(text);
	validateExe();
}
#endif

void SystemTab::textIncEdited(const QString& text) {
	parent->m_prefData->setPovrayIncludes(text);
	validateInc();
}
void SystemTab::textInsEdited(const QString& text) {
	parent->m_prefData->setPovrayInsertMenu(text);
	validateIns();
}
void SystemTab::textIndEdited(const QString& text) {
	parent->m_prefData->setPovraySceneDirectory(text);
	validateInd();
}
void SystemTab::textInhEdited(const QString& text) {
	parent->m_prefData->setPovrayHelpDirectory(text);
	validateInh();
}
void SystemTab::textInqEdited(const QString& text) {
	parent->m_prefData->setQtpovrayHelpDirectory(text);
	validateInq();
}

void SystemTab::validateInc() {
	if (Preferences::validateInc(parent->m_prefData->getPovrayIncludes()))
		povrayIncludesStatus->setPixmap(parent->m_iconOk->pixmap(16));
	else
		povrayIncludesStatus->setPixmap(parent->m_iconBad->pixmap(16));
}

bool Preferences::validateInc(const QString &file) {
	bool incStat = false;
	if (!file.isEmpty()) {
		QDir dir(file);
		if (dir.exists("colors.inc"))
			incStat = true;
	}
	return(incStat);
}

void SystemTab::validateIns() {
	if (Preferences::validateIns(parent->m_prefData->getPovrayInsertMenu()))
		povrayInsertMenuStatus->setPixmap(parent->m_iconOk->pixmap(16));
	else
		povrayInsertMenuStatus->setPixmap(parent->m_iconBad->pixmap(16));
}

bool Preferences::validateIns(const QString &file) {
	bool insStat = false;
	if (!file.isEmpty()) {
		QDir dir(file);
		if (dir.exists("Shapes3.pov"))
			insStat = true;
	}
	return(insStat);
}

void SystemTab::validateInd() {
	if (Preferences::validateInd(parent->m_prefData->getPovraySceneDirectory()))
		povraySceneDirectoryStatus->setPixmap(parent->m_iconOk->pixmap(16));
	else
		povraySceneDirectoryStatus->setPixmap(parent->m_iconBad->pixmap(16));
}

bool Preferences::validateInd(const QString &file) {
	bool insStat = false;
	if (!file.isEmpty()) {
		QDir dir(file);
		if (dir.exists("index.htm"))
			insStat = true;
	}
	return(insStat);
}

void SystemTab::validateInh() {
	if (Preferences::validateInh(parent->m_prefData->getPovrayHelpDirectory()))
		povrayHelpDirectoryStatus->setPixmap(parent->m_iconOk->pixmap(16));
	else
		povrayHelpDirectoryStatus->setPixmap(parent->m_iconBad->pixmap(16));
}

bool Preferences::validateInh(const QString &file) {
	bool insStat = false;
	if (!file.isEmpty()) {
		QDir dir(file);
		if (dir.exists("index.html"))
			insStat = true;
	}
	return(insStat);
}

void SystemTab::validateInq() {
	if (Preferences::validateInq(parent->m_prefData->getQtpovrayHelpDirectory()))
		qtpovrayHelpDirectoryStatus->setPixmap(parent->m_iconOk->pixmap(16));
	else
		qtpovrayHelpDirectoryStatus->setPixmap(parent->m_iconBad->pixmap(16));
}

bool Preferences::validateInq(const QString &file) {
	bool inqStat = false;
	if (!file.isEmpty()) {
		QDir dir(file);
		if (dir.exists("index.html"))
			inqStat = true;
	}
	return(inqStat);
}

#ifdef USE_WEBSOCKETS
void SystemTab::validateExe() {
	if (parent->mainWindow->validateExe(parent->prefData->getPovrayExecutable(), povrayBanner))
		povrayExecutableStatus->setPixmap(parent->iconOk->pixmap(16));
	else
		povrayExecutableStatus->setPixmap(parent->iconBad->pixmap(16));
}
#endif

void SystemTab::benchmarkButtonClicked(bool ) {

}

///////////////////////////////////////////////////////////////////
EditorTab::EditorTab(Preferences* parent)
	: QWidget(parent),
	  parent(parent) {
	PreferenceData* prefData = parent->m_prefData;
//	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	QGridLayout* layout = new QGridLayout(this);

	largeIconButton = new QCheckBox(this);
	largeIconButton->setChecked(prefData->getUseLargeIcons());
	largeIconButton->setWhatsThis(tr("Restart required"));
	wrapButton = new QCheckBox(this);
	wrapButton->setChecked(prefData->getEditorWrapText());
	tabWidthSpinner = new QSpinBox(this);
	tabWidthSpinner->setValue(prefData->getEditorTabWidth());
	autoIndentButton = new QCheckBox(this);
	autoIndentButton->setChecked(prefData->getAutoIndent());
	autoBraceButton = new QCheckBox(this);
	autoBraceButton->setChecked(prefData->getAutoBraceCompletion());
	highlightLineButton = new QCheckBox(this);
	highlightLineButton->setChecked(prefData->getEditorHighlightCurrentLine());
	highlightTokensButton = new QCheckBox(this);
	highlightTokensButton->setChecked(prefData->getEditorHighlightTokens());

	connect(largeIconButton, SIGNAL(stateChanged(int)), this, SLOT(largeIconButtonChanged(int)));
	connect(wrapButton, SIGNAL(stateChanged(int)), this, SLOT(wrapButtonChanged(int)));
	connect(tabWidthSpinner, SIGNAL(valueChanged(int)), this, SLOT(tabWidthChanged(int)));
	connect(autoIndentButton, SIGNAL(stateChanged(int)), this, SLOT(autoIndentChanged(int)));
	connect(autoBraceButton, SIGNAL(stateChanged(int)), this, SLOT(autoBraceChanged(int)));
	connect(highlightLineButton, SIGNAL(stateChanged(int)), this, SLOT(highlightLineChanged(int)));
	connect(highlightTokensButton, SIGNAL(stateChanged(int)), this, SLOT(highlightTokensChanged(int)));

	layout->addWidget(new QLabel(tr("Use large icons"), parent), 0, 0, Qt::AlignLeft);
	layout->addWidget(largeIconButton, 0, 1);
	layout->addWidget(new QLabel(tr("Wrap text"), parent), 1, 0, Qt::AlignLeft);
	layout->addWidget(wrapButton, 1, 1);
	layout->addWidget(new QLabel(tr("Tab width"), parent), 2, 0);
	layout->addWidget(tabWidthSpinner, 2, 1);
	layout->addWidget(new QLabel(tr("Auto indent"), parent), 3, 0);
	layout->addWidget(autoIndentButton, 3, 1);
	layout->addWidget(new QLabel(tr("Auto Brace completion"), parent), 4, 0);
	layout->addWidget(autoBraceButton, 4, 1);
	layout->addWidget(new QLabel(tr("Highlight current line"), parent), 5, 0);
	layout->addWidget(highlightLineButton, 5, 1);
	layout->addWidget(new QLabel(tr("Highlight matching tokens"), parent), 6, 0);
	layout->addWidget(highlightTokensButton, 6, 1);
	layout->setVerticalSpacing(5);
	layout->setRowStretch(7, 1);
//	mainLayout->addLayout(layout);
	this->setLayout(layout);
}

void EditorTab::largeIconButtonChanged(int state) {
	parent->m_prefData->setUseLargeIcons(state == Qt::Checked);
}
void EditorTab::wrapButtonChanged(int state) {
	parent->m_prefData->setEditorWrapText(state == Qt::Checked);
}
void EditorTab::tabWidthChanged(int width) {
	parent->m_prefData->setEditorTabWidth(width);
}

void EditorTab::autoIndentChanged(int state) {
	parent->m_prefData->setAutoIndent(state == Qt::Checked);
}
void EditorTab::autoBraceChanged(int state) {
	parent->m_prefData->setAutoBraceCompletion(state == Qt::Checked);
}
void EditorTab::highlightLineChanged(int state) {
	parent->m_prefData->setEditorHighlightCurrentLine(state == Qt::Checked);
}
void EditorTab::highlightTokensChanged(int state) {
	parent->m_prefData->setEditorHighlightTokens(state == Qt::Checked);
}

///////////////////////////////////////////////////////////////////
/// \brief KeyTab::KeyTab
/// \param parent
///
KeyTab::KeyTab(Preferences* parent)
	: QWidget(parent),
	  m_parent(parent) {
	PreferenceData* prefData = parent->m_prefData;
//	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	//	QVBoxLayout* vlayout = new QVBoxLayout;
	//	vlayout->addStretch(1);
	m_newKeyGroupBox = new QGroupBox(tr("Type new key"));
	QVBoxLayout* blayout = new QVBoxLayout;
	m_newKeyEdit = new QKeySequenceEdit(this);
	blayout->addWidget(m_newKeyEdit);
	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
										 | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(newKeyOk()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(newKeyCancel()));
	blayout->addWidget(buttonBox);
	m_newKeyGroupBox->setLayout(blayout);

	QGridLayout* layout = new QGridLayout(this);
	addKey(layout, 0, tr("Start/Stop Render"), &prefData->getGlobalKeys()->keyStartRender);
	addKey(layout, 1, tr("Open configure dialog"), &prefData->getGlobalKeys()->keyOpenPreferences);
	addKey(layout, 2, tr("Toggle bookmark"), &prefData->getGlobalKeys()->keyBookmarkToggle);
	addKey(layout, 3, tr("Move to next bookmark"), &prefData->getGlobalKeys()->keyBookmarkNext);
	addKey(layout, 4, tr("Move to previous bookmark"), &prefData->getGlobalKeys()->keyBookmarkPrevious);
	addKey(layout, 5, tr("Move to next error"), &prefData->getGlobalKeys()->keyErrorNext);
	addKey(layout, 6, tr("Move to previous error"), &prefData->getGlobalKeys()->keyErrorPrevious);
	addKey(layout, 7, tr("Move to next find"), &prefData->getGlobalKeys()->keyFindNext);
	addKey(layout, 8, tr("Move to previous find"), &prefData->getGlobalKeys()->keyFindPrevious);
	layout->setRowStretch(9, 1);
	layout->addWidget(m_newKeyGroupBox, 10, 0, 1, 2);
	m_newKeyGroupBox->hide();
}

void KeyTab::addKey(QGridLayout* layout, int row, const QString& text, QKeySequence* key)
{
	KeyLabelLabel* label = new KeyLabelLabel(text, this);
	layout->addWidget(label, row, 0);
	QKeySequence ks = *key;
	KeyLabel* kl = new KeyLabel(ks.toString(), this);
	kl->m_data = key;
	label->setBuddy(kl);
	layout->addWidget(kl, row, 1);
}

void KeyTab::newKeyCancel()
{
	m_newKeyGroupBox->hide();
}
void KeyTab::newKeyOk()
{
	*m_activeKeyLabel->m_data = m_newKeyEdit->keySequence();
	m_activeKeyLabel->setText(m_newKeyEdit->keySequence().toString());
	m_newKeyGroupBox->hide();
}

void KeyLabel::mousePressEvent(QMouseEvent*)
{
	//QLabel::mousePressEvent(event);
	m_parent->m_newKeyGroupBox->show();
	m_parent->m_newKeyEdit->setFocus();
	m_parent->m_activeKeyLabel = this;
}

void KeyLabelLabel::mousePressEvent(QMouseEvent* event)
{
	((KeyLabel*)buddy())->mousePressEvent(event);
}

//void KeyLineEdit::keyPressEvent(QKeyEvent* event)
//{
//	QString s;
//	qDebug() << "keyPressEvent" << event;
//	if (event->modifiers() & Qt::ControlModifier)
//		s += "Ctrl+";
//	if (event->modifiers() & Qt::AltModifier)
//		s += "Alt+";
//	if (event->modifiers() & Qt::ShiftModifier)
//		s += "Shift+";
//	if (!event->text().isEmpty()) {
//		QKeySequenceEdit
//		s += event->key();
//	}
//	this->setText(s);
//}
