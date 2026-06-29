#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QObject>
#include <QString>

class ThemeManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentTheme READ currentTheme WRITE setCurrentTheme NOTIFY currentThemeChanged)

public:
    explicit ThemeManager(QObject *parent = nullptr);

    QString currentTheme() const;
    void setCurrentTheme(const QString &theme);

signals:
    void currentThemeChanged();

private:
    QString normalizedTheme(const QString &theme) const;

    QString m_currentTheme;
};

#endif // THEMEMANAGER_H
