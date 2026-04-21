# Multi-device session contract

## Цель

Зафиксировать контракт между слоем домена/интеграции с устройствами и UI для поддержки нескольких параллельных сессий без взаимного влияния.

## Модель Session

`Session` описывает одну логическую сессию подключения к конкретному устройству.

| Поле | Тип (рекоменд.) | Описание |
|---|---|---|
| `sessionId` | `string` | Уникальный идентификатор сессии в рамках процесса приложения. |
| `target` | `string \| object` | Цель подключения (IP/host/device descriptor). |
| `displayName` | `string` | Имя сессии для отображения в UI. |
| `state` | `SessionState` | Текущее состояние сессии. |

### SessionState

Допустимые значения `state`:

- `idle`
- `connecting`
- `connected`
- `error`
- `disconnected`

### Переходы состояний (одна сессия)

Разрешенные переходы должны быть явными и проверяться guard-логикой внутри `Core`:

- `idle -> connecting`
- `connecting -> connected`
- `connecting -> error`
- `connecting -> disconnected`
- `connected -> disconnected`
- `connected -> error`
- `error -> connecting` (повторная попытка)
- `disconnected -> connecting` (reconnect)

Недопустимые переходы (например, `idle -> connected` без этапа `connecting`) должны:

1. Блокироваться без изменения текущего `state`.
2. Логироваться с `sessionId`, исходным и целевым состоянием.
3. Не порождать дополнительных глобальных side effects.

## UI-события

События, которые должен получать UI:

- `sessionAdded(sessionId)`
- `sessionStateChanged(sessionId, state)`
- `deviceReady(sessionId, deviceRef)`
- `sessionRemoved(sessionId)`

## Инварианты

1. **Все сигналы, относящиеся к устройству, несут `sessionId`.**  
   Это обязательно для детерминированного роутинга событий к конкретной карточке/экрану сессии в UI.

2. **Ошибка или `disconnected` одной сессии не меняет состояние других.**  
   Каждая `Session` управляется и отображается независимо.

3. **UI не использует глобальный singleton `core.device()` как источник истины.**  
   Источник истины для UI — коллекция `Session` (например, `sessionsById`/`sessionList`) и события, параметризованные `sessionId`.

## Рекомендации по применению

- Любые callback/сигналы из транспортного слоя нормализуются в события с `sessionId`.
- Экран списка устройств рендерится по списку сессий, а не по одному "текущему" устройству.
- Для операций reconnect/remove использовать `sessionId` как единственный ключ адресации.
- Ошибки/дисконнекты обрабатываются локально в том же `Core`, который ведет конкретную `Session`; допускаются только скоупленные события с `sessionId` (например, `sessionStateChanged(sessionId, "error")`), без глобальных переключений состояния приложения.
