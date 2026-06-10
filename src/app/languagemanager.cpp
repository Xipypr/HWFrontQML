#include "languagemanager.h"

#include <QCoreApplication>
#include <QLibraryInfo>
#include <QQmlApplicationEngine>
#include <QSettings>

namespace {
constexpr auto LanguageKey = "app/language";
constexpr auto DefaultLanguage = "ru";
constexpr auto EnglishLanguage = "en";
constexpr auto RussianLanguage = "ru";
}

LanguageManager::LanguageManager(QQmlApplicationEngine *engine, QObject *parent)
    : QObject(parent)
    , m_engine(engine)
{
    const QString savedLanguage = QSettings().value(QString::fromLatin1(LanguageKey), QString::fromLatin1(DefaultLanguage)).toString();
    setCurrentLanguage(savedLanguage);
}

QString LanguageManager::currentLanguage() const
{
    return m_currentLanguage;
}

void LanguageManager::setCurrentLanguage(const QString &language)
{
    const QString normalized = normalizedLanguage(language);
    if (normalized == m_currentLanguage) {
        return;
    }

    if (!installTranslatorForLanguage(normalized)) {
        return;
    }

    m_currentLanguage = normalized;
    QSettings().setValue(QString::fromLatin1(LanguageKey), m_currentLanguage);

    if (m_engine) {
        m_engine->retranslate();
    }

    emit currentLanguageChanged();
}

QVariantList LanguageManager::availableLanguages() const
{
    return {
        QVariantMap {
            { QStringLiteral("code"), QString::fromLatin1(RussianLanguage) },
            { QStringLiteral("label"), tr("Русский") },
        },
        QVariantMap {
            { QStringLiteral("code"), QString::fromLatin1(EnglishLanguage) },
            { QStringLiteral("label"), tr("English") },
        },
    };
}

QString LanguageManager::normalizedLanguage(const QString &language) const
{
    const QString normalized = language.trimmed().toLower();
    if (normalized.startsWith(QString::fromLatin1(RussianLanguage))) {
        return QString::fromLatin1(RussianLanguage);
    }
    if (normalized.startsWith(QString::fromLatin1(EnglishLanguage))) {
        return QString::fromLatin1(EnglishLanguage);
    }

    return QString::fromLatin1(DefaultLanguage);
}

bool LanguageManager::installTranslatorForLanguage(const QString &language)
{
    QCoreApplication::removeTranslator(&m_translator);
    QCoreApplication::removeTranslator(&m_qtTranslator);

    if (language == QString::fromLatin1(EnglishLanguage)) {
        return true;
    }

    if (language == QString::fromLatin1(RussianLanguage)
        && m_translator.load(QStringLiteral(":/i18n/HWFrontQML_ru.qm"))) {
        QCoreApplication::installTranslator(&m_translator);
        if (m_qtTranslator.load(QStringLiteral("qtbase_ru"), QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
            QCoreApplication::installTranslator(&m_qtTranslator);
        }
        return true;
    }

    return false;
}
