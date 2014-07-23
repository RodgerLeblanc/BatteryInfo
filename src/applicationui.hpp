/*
 * Copyright (c) 2011-2013 BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include <QObject>
#include <QFileSystemWatcher>
#include <bb/cascades/Label>
#include <bb/cascades/Container>
#include <QtScript/qscriptengine.h>
#include <QSettings>

namespace bb
{
    namespace cascades
    {
        class Application;
        class LocaleHandler;
    }
}

class QTranslator;

/*!
 * @brief Application object
 *
 *
 */

class ApplicationUI : public QObject
{
    Q_OBJECT

public:
    ApplicationUI(bb::cascades::Application *app);
    virtual ~ApplicationUI() { }

    Q_INVOKABLE void saveSettings(const QString key, const QString value);
    Q_INVOKABLE QString getSettings(const QString key, const QString defaultValue);

Q_SIGNALS:
	// The change notification signal
    void changed();

private slots:
    void onSystemLanguageChanged();

    void fileChanged(const QString & path);
    void onTimer();

private:
    QTranslator* m_pTranslator;
    bb::cascades::LocaleHandler* m_pLocaleHandler;

    void getBatteryInfo();
    QMap<QString, QVariant> decodeInner(QScriptValue object);
	QList<QVariant> decodeInnerToList(QScriptValue arrayValue);
	void traverse(const QVariant &value, QString &out) const;
	bb::cascades::Label* m_label;
	bb::cascades::Label* m_label2;
	bb::cascades::Container* m_uiConsciousContainer;
	bb::cascades::Container* m_rawDataContainer;
    QFileSystemWatcher* m_battWatcher;

	bb::cascades::Label* m_batteryName;
	bb::cascades::Label* m_temperature;
	bb::cascades::Label* m_batteryVoltage;
	bb::cascades::Label* m_stateOfHealth;
	bb::cascades::Label* m_stateOfCharge;
	bb::cascades::Label* m_timeToEmpty;
	bb::cascades::Label* m_designCapacity;
	bb::cascades::Label* m_maxSystemVoltage;
	bb::cascades::Label* m_minSystemVoltage;
	bb::cascades::Label* m_averageCurrent;
	bb::cascades::Label* m_cycleCount;

	double consumed;
	int secs;
	int mins;
	int hours;
	QSettings settings;
};

#endif /* ApplicationUI_HPP_ */
