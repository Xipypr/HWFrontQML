#ifndef LANGUAGEMANAGER_H
#define LANGUAGEMANAGER_H

#include <QObject>
#include <QString>
#include <QTranslator>
#include <QVariantList>

class QQmlApplicationEngine;

class LanguageManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentLanguage READ currentLanguage WRITE setCurrentLanguage NOTIFY currentLanguageChanged)
    Q_PROPERTY(QVariantList availableLanguages READ availableLanguages CONSTANT)

public:
    explicit LanguageManager(QQmlApplicationEngine *engine, QObject *parent = nullptr);

    QString currentLanguage() const;
    void setCurrentLanguage(const QString &language);
    QVariantList availableLanguages() const;

signals:
    void currentLanguageChanged();

private:
    QString normalizedLanguage(const QString &language) const;
    bool installTranslatorForLanguage(const QString &language);

    QQmlApplicationEngine *m_engine = nullptr;
    QTranslator m_translator;
    QTranslator m_qtTranslator;
    QString m_currentLanguage;
};

#endif // LANGUAGEMANAGER_H
