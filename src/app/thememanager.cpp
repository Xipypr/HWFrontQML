#include "thememanager.h"

#include <QSettings>

namespace {
constexpr auto ThemeKey = "app/theme";
constexpr auto DefaultTheme = "night";
constexpr auto LightTheme = "light";
constexpr auto GraphiteTheme = "graphite";
constexpr auto OledTheme = "oled";
}

ThemeManager::ThemeManager(QObject *parent)
    : QObject(parent)
{
    const QString savedTheme = QSettings().value(QString::fromLatin1(ThemeKey),
                                                  QString::fromLatin1(DefaultTheme)).toString();
    m_currentTheme = normalizedTheme(savedTheme);
    if (savedTheme != m_currentTheme) {
        QSettings().setValue(QString::fromLatin1(ThemeKey), m_currentTheme);
    }
}

QString ThemeManager::currentTheme() const
{
    return m_currentTheme;
}

void ThemeManager::setCurrentTheme(const QString &theme)
{
    const QString normalized = normalizedTheme(theme);
    if (normalized == m_currentTheme) {
        return;
    }

    m_currentTheme = normalized;
    QSettings().setValue(QString::fromLatin1(ThemeKey), m_currentTheme);
    emit currentThemeChanged();
}

QString ThemeManager::normalizedTheme(const QString &theme) const
{
    const QString normalized = theme.trimmed().toLower();
    if (normalized == QString::fromLatin1(LightTheme)
        || normalized == QString::fromLatin1(GraphiteTheme)
        || normalized == QString::fromLatin1(OledTheme)) {
        return normalized;
    }

    return QString::fromLatin1(DefaultTheme);
}
