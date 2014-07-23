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

#include "applicationui.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>

#include <bb/data/JsonDataAccess>
#include <QtCore/QMetaType>
#include <QTimer>

#include <bps/battery.h>

Q_DECLARE_METATYPE(QList<QVariantMap>)

using namespace bb::cascades;

ApplicationUI::ApplicationUI(bb::cascades::Application *app) :
        QObject(app)
{
	settings.setValue("lastHours", settings.value("hours", "").toString());
	settings.setValue("lastMins", settings.value("mins", "").toString());
	settings.setValue("lastSecs", settings.value("secs", "").toString());

    // prepare the localization
    m_pTranslator = new QTranslator(this);
    m_pLocaleHandler = new LocaleHandler(this);

    bool res = QObject::connect(m_pLocaleHandler, SIGNAL(systemLanguageChanged()), this, SLOT(onSystemLanguageChanged()));
    // This is only available in Debug builds
    Q_ASSERT(res);
    // Since the variable is not used in the app, this is added to avoid a
    // compiler warning
    Q_UNUSED(res);

    // initial load
    onSystemLanguageChanged();

    // Create scene document from main.qml asset, the parent is set
    // to ensure the document gets destroyed properly at shut down.
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);
	qml->setContextProperty("_app", this);

    // Create root object for the UI
    AbstractPane *root = qml->createRootObject<AbstractPane>();

    // Set created root object as the application scene
    app->setScene(root);

    m_label = root->findChild<Label*>("m_label");
    m_label2 = root->findChild<Label*>("m_label2");
    m_uiConsciousContainer = root->findChild<Container*>("m_uiConscious");
    m_rawDataContainer = root->findChild<Container*>("m_rawData");

    m_batteryName = root->findChild<Label*>("m_batteryName");
    m_temperature = root->findChild<Label*>("m_temperature");
    m_batteryVoltage = root->findChild<Label*>("m_batteryVoltage");
    m_stateOfHealth = root->findChild<Label*>("m_stateOfHealth");
    m_stateOfCharge = root->findChild<Label*>("m_stateOfCharge");
    m_timeToEmpty = root->findChild<Label*>("m_timeToEmpty");
    m_designCapacity = root->findChild<Label*>("m_designCapacity");
    m_maxSystemVoltage = root->findChild<Label*>("m_maxSystemVoltage");
    m_minSystemVoltage = root->findChild<Label*>("m_minSystemVoltage");
    m_averageCurrent = root->findChild<Label*>("m_averageCurrent");
    m_cycleCount = root->findChild<Label*>("m_cycleCount");

    // Watcher for changes in the battery file. ------ Crash the app
//	m_battWatcher = new QFileSystemWatcher(this);
//    m_battWatcher->addPath("//pps/system/BattMgr/");
//    m_battWatcher->addPath("//pps/system/BattMgr/status");
//    connect(m_battWatcher, SIGNAL(fileChanged(const QString&)), this, SLOT(fileChanged(const QString&)));

    secs = 0;
    mins = 0;
    hours = 0;

	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
	timer->start(1000);

	onTimer();

//    getBatteryInfo();
}

void ApplicationUI::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(m_pTranslator);
    // Initiate, load and install the application translation files.
    QString locale_string = QLocale().name();
    QString file_name = QString("BatteryInfo_%1").arg(locale_string);
    if (m_pTranslator->load(file_name, "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(m_pTranslator);
    }
}

void ApplicationUI::fileChanged(const QString & path)
{
//	Q_UNUSED(path);
	qDebug() << "fileChanged" << path;
	// Update display everytime the file change
	getBatteryInfo();
}

void ApplicationUI::onTimer()
{
	// Update display everytime the file change
//	getBatteryInfo();
	battery_info_t *info = NULL;
	if (BPS_SUCCESS == battery_get_info(&info)) {
		secs++;
		if (secs == 60) {
			mins++;
			secs = 0;
		}
		if (mins == 60) {
			hours++;
			mins = 0;
		}
		settings.setValue("hours", hours);
		settings.setValue("mins", mins);
		settings.setValue("secs", secs);

		int avgCurrent = battery_info_get_battery_average_current(info);
		QString text = "YOU NEED TO START THIS APP WHILE BATTERY IS NEAR 100% TO GET RELIABLE RESULTS.\n\nFormula :\nBattery Consumed = (Average Current * Workload Duration) / 1e3\n\n";
		text += "Average Current : " + QString::number(avgCurrent) + " mA\n";
		if (avgCurrent < 0)
			avgCurrent = 0 - avgCurrent;
		double consumedNow = avgCurrent * 0.000277777777777778;
		consumed += consumedNow;
		text += "Battery Consumed (mAh) : " + QString::number(consumed);
		QString displayedSeconds = (secs < 10) ? ("0" + QString::number(secs)) : (QString::number(secs));
		QString displayedMinutes = (mins < 10) ? ("0" + QString::number(mins)) : (QString::number(mins));
		QString displayedHours = QString::number(hours);
		QString time = displayedHours + ":" + displayedMinutes + ":" + displayedSeconds;
		displayedSeconds = (settings.value("lastSecs", "").toDouble() < 10) ? ("0" + QString::number(settings.value("lastSecs", "").toDouble())) : (QString::number(settings.value("lastSecs", "").toDouble()));
		displayedMinutes = (settings.value("lastMins", "").toDouble() < 10) ? ("0" + QString::number(settings.value("lastMins", "").toDouble())) : (QString::number(settings.value("lastMins", "").toDouble()));
		displayedHours = QString::number(settings.value("lastHours", "").toDouble());
		QString lastTime = displayedHours + ":" + displayedMinutes + ":" + displayedSeconds;
		int percent = 100 - ((consumed / settings.value("batterySize", "1800").toDouble()) * 100);
		m_label->setText("Last time : " + lastTime + "\nTime : " + time + "\n\n" + text + "\n\nBattery percent : " + QString::number(percent) + " %");

		battery_free_info(&info);
	}
	else
	{
	  qDebug() << "Unable to get battery info\n";
	}
}

void ApplicationUI::getBatteryInfo()
{
    QFile battFile("//pps/system/BattMgr/status");
    if (battFile.open(QFile::ReadOnly)) {
    	m_label->setText("");

    	// Load the entire file containing json data for battery and charger information
    	QString all(battFile.readAll());

    	/* Split the file in 4 different json string.
    	 * 0- Not used. See the next for(), it starts at 1 instead of 0.
    	 * 1- Battery json string
    	 * 2- Charger json string
    	 * 3- Charger again json string
    	 */
    	QStringList jsonFiles = all.split(":json:");
    	for (int i = 1; i < jsonFiles.size(); i++) {
    		// Get rid of any string at the end of json data
    		int start = jsonFiles[i].lastIndexOf("}") + 1;
    		int length = jsonFiles[i].size() - start;
    		jsonFiles[i].remove(start, length);
//    		qDebug() << jsonFiles[i];

    		// Load the json data in a map
    		bb::data::JsonDataAccess ja;
        	const QVariant jsonva = ja.loadFromBuffer(jsonFiles[i]);
        	QMap<QString, QVariant> mapOfJson = jsonva.toMap();
//    		qDebug() << mapOfJson;

    		// Process the value for each entry
    		foreach (const QVariant &value, mapOfJson) {
    		    QString out;
    		    out.reserve(200);
    		    traverse(value, out);
    	        m_label->setText(m_label->text() + out);
    		}
    		QString out;
    		QString compare = m_label->text();
//            if (compare.indexOf("BatteryName"))
//            	m_batteryName->setText(out);
            if (compare == "Temperature")
            	m_temperature->setText(out);
            if (compare == "BatteryVoltage")
            	m_batteryVoltage->setText(out);
            if (compare == "StateOfHealth")
            	m_stateOfHealth->setText(out);
            if (compare == "StateOfCharge")
            	m_stateOfCharge->setText(out);
            if (compare == "TimeToEmpty")
            	m_timeToEmpty->setText(out);
            if (compare == "DesignCapacity")
            	m_designCapacity->setText(out);
            if (compare == "MaxSystemVoltage")
            	m_maxSystemVoltage->setText(out);
            if (compare == "MinSystemVoltage")
            	m_minSystemVoltage->setText(out);
            if (compare == "AverageCurrent")
            	m_averageCurrent->setText(out);
            if (compare == "CycleCount")
            	m_cycleCount->setText(out);
    		m_label->setText(m_label->text() + "\n-----------------------------------\n\n");
    	}
		m_label->setText(m_label->text() + all);
    }
    else {
    	m_label->setText("Json file not opening");
    	return;
    }
    emit changed();
}

void ApplicationUI::traverse(const QVariant &value, QString &out) const
{
    switch (value.type()) {
        case QVariant::Map:
            {
                const QVariantMap object = value.value<QVariantMap>();
                QMapIterator<QString, QVariant> it(object);
                while(it.hasNext()) {
                    it.next();
                    out += it.key();
                    out += ": ";
                    traverse(it.value(), out);
                    out += "\n";
                }
            }
            break;
        case QVariant::List:
            {
                const QVariantList list = value.value<QVariantList>();
                QListIterator<QVariant> it(list);
                int index = 0;
                while(it.hasNext()) {
                    QVariant value = it.next();
                    out += "item[";
                    out += QString::number(index++);
                    out += "]:\n";
                    traverse(value, out);
                }
                out += "\n";
            }
            break;
        case QVariant::String:
            {
                out += "\"";
                out += value.toString();
                out += "\"";
            }
            break;
        case QVariant::Bool:
            {
                out += value.toString();
            }
            break;
        case QVariant::Int:
            {
                out += value.toString();
            }
            break;
        case QVariant::Double:
            {
                out += value.toString();
            }
            break;
        case QVariant::LongLong:
            {
                out += value.toString();
            }
            break;
        default:
            if (value.canConvert< QList<QVariantMap> >())
            {
                const QList<QVariantMap> list = value.value< QList<QVariantMap> >();
                QListIterator<QVariantMap> it(list);
                int index = 0;
                while(it.hasNext()) {
                    QVariant value = it.next();
                    out += "item[";
                    out += QString::number(index++);
                    out += "]:\n";
                    traverse(value, out);
                }
                out += "\n";
                break;
            }
            qWarning() << "Unsupported property type: " << value.typeName();
            break;
    }
}

void ApplicationUI::saveSettings(const QString key, const QString value)
{
	settings.setValue(key, value);
}

QString ApplicationUI::getSettings(const QString key, const QString defaultValue)
{
	return settings.value(key, defaultValue).toString();
}
