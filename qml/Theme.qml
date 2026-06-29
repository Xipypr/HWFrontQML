pragma Singleton

import QtQuick 2.15
import "themes"

QtObject {
    readonly property bool isLight: themeManager.currentTheme === "light"

    readonly property BasePalette nightPalette: NightPalette {}
    readonly property BasePalette softLightPalette: SoftLightPalette {}
    readonly property BasePalette graphitePalette: GraphitePalette {}
    readonly property BasePalette oledPalette: OledPalette {}
    readonly property BasePalette palette: paletteFor(themeManager.currentTheme)

    function paletteFor(theme) {
        switch (theme) {
        case "light":
            return softLightPalette
        case "graphite":
            return graphitePalette
        case "oled":
            return oledPalette
        case "night":
        default:
            return nightPalette
        }
    }

    readonly property color background: palette.background
    readonly property color surface: palette.surface
    readonly property color surfaceRaised: palette.surfaceRaised
    readonly property color cardSurface: palette.cardSurface
    readonly property color controlSurface: palette.controlSurface
    readonly property color border: palette.border
    readonly property color textPrimary: palette.textPrimary
    readonly property color textSecondary: palette.textSecondary
    readonly property color titleText: palette.titleText
    readonly property color accent: palette.accent
    readonly property real cardAccentOpacity: palette.cardAccentOpacity

    readonly property color headerBackground: palette.headerBackground
    readonly property color headerPressed: palette.headerPressed
    readonly property color headerButton: palette.headerButton
    readonly property color headerButtonPressed: palette.headerButtonPressed
    readonly property color metricTrack: palette.metricTrack

    readonly property color valueOnly: palette.valueOnly
    readonly property color normal: palette.normal
    readonly property color warning: palette.warning
    readonly property color critical: palette.critical
    readonly property color connecting: palette.connecting
    readonly property color reconnecting: palette.reconnecting
    readonly property color inactive: palette.inactive
    readonly property color download: palette.download
    readonly property color upload: palette.upload
}
