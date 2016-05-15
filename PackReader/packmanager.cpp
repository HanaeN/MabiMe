#include "packmanager.h"

#include <QSettings>
#include <QDebug>

PackManager::PackManager()
{
    if (!findMabinogiPath()) {
        qDebug() << "could not find path";
    } else {
        qDebug() << path;
    }
}

PackManager::PackManager(QString path) {
    // load from path
}

#if defined(Q_OS_WIN)
bool PackManager::findMabinogiPath() {
    QSettings m("HKEY_CURRENT_USER\\Software\\Nexon\\Mabinogi", QSettings::NativeFormat); // get the default path, not used in nexon launcher installs
    QString p = m.value("Default","").toString();
    if (p.length() == 0) { // failed first check, check directplay location for 64bit
        QSettings m("HdKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Microsoft\\DirectPlay8\\Applications\\", QSettings::NativeFormat);
        foreach (const QString &group, m.childGroups()) {
            m.beginGroup(group);
            if (m.value("ExecutableFilename", "").toString().compare("mabinogi.exe", Qt::CaseInsensitive) == 0) {
                p = m.value("ExecutablePath", "").toString();
                m.endGroup();
                break;
            }
            m.endGroup();
        }
        if (p.length() == 0) { // failed again, try 32 bit
            QSettings m("HKdEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\DirectPlay8\\Applications\\", QSettings::NativeFormat);
            foreach (const QString &group, m.childGroups()) {
                m.beginGroup(group);
                if (m.value("ExecutableFilename", "").toString().compare("mabinogi.exe", Qt::CaseInsensitive) == 0) {
                    p = m.value("ExecutablePath", "").toString();
                    m.endGroup();
                    break;
                }
                m.endGroup();
            }

        }
    }
    if (p.length() == 0) { // failed directplay locations, look at compat flags
        QSettings m("HKEY_USERS", QSettings::NativeFormat);
        foreach (const QString &group, m.childGroups()) {
            QSettings m2("HKEY_USERS\\" + group + "\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Compatibility Assistant\\Store", QSettings::NativeFormat);
            foreach (const QString &key, m2.childKeys()) {
                if (key.endsWith("mabinogi.exe", Qt::CaseInsensitive)) {
                    p = key;
                    p.remove("mabinogi.exe", Qt::CaseInsensitive);
                    break;
                }
            }
            if (p.length() > 0) break;
        }
    }
    if (p.length() == 0) return false;
    p.replace("\\", "/");
    p = p.endsWith("/") ? p + "package/" : p + "/package/";
    if (QFile(p).exists()) {
        path = p;
        return true;
    } else {
        return false;
    }
}
#elif defined (Q_OS_LINUX)
bool PackManager::findMabinogiPath() {
    return false;
}
#endif
