/*
    SPDX-FileCopyrightText: 2007, 2008 Nicolas Roffet <nicolas-kde@roffet.com>
    SPDX-FileCopyrightText: 2007 Pino Toscano <toscano.pino@tiscali.it>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "kgamedifficulty.h"

// KF
#include <KActionCollection>
#include <KComboBox>
#include <KLocalizedString>
#include <KMessageBox>
#include <KSelectAction>
#include <KXmlGuiWindow>
// Qt
#include <QIcon>
#include <QStatusBar>
// Std
#include <utility>

#if KDEGAMESPRIVATE_BUILD_DEPRECATED_SINCE(7, 4)

class KGameDifficultyPrivate : public QObject
{
    Q_OBJECT

public:
    ~KGameDifficultyPrivate() override;

    void init(KXmlGuiWindow *window, const QObject *recvr, const char *slotStandard, const char *slotCustom);

    void rebuildActions();

    /**
     * @return standard string for standard level
     */
    QPair<QByteArray, QString> standardLevelString(KGameDifficulty::standardLevel level);

    void setLevel(KGameDifficulty::standardLevel level);
    void setLevelCustom(int key);

    /**
     * @brief Current custom difficulty level
     */
    int m_levelCustom;

    KGameDifficulty::standardLevel m_level;
    QList<KGameDifficulty::standardLevel> m_standardLevels;
    QMap<int, QString> m_customLevels;

    KSelectAction *m_menu;
    KGameDifficulty::onChange m_restartOnChange;
    bool m_running;
    int m_oldSelection;
    KComboBox *m_comboBox;

public Q_SLOTS:
    /**
     * @brief Player wants to change the difficulty level to a standard level
     *
     * The difference with the method "setSelection" is that the player may have to confirm that he agrees to end the current game (if needed).
     * @param newSelection Selected item.
     */
    void changeSelection(int newSelection);

Q_SIGNALS:
    /**
     * @brief Current difficulty level changed to a standard level
     *
     * The game catches this signal and restarts a game with the new standard difficulty level.
     * @param level New standard level.
     */
    void standardLevelChanged(KGameDifficulty::standardLevel level);

    /**
     * @brief Current difficulty level changed to a custom level
     *
     * The game catches this signal and restarts a game with the new standard difficulty level.
     * @param key Custom level identifier.
     */
    void customLevelChanged(int key);

private:
    void setSelection(int newSelection);
};

KGameDifficultyPrivate::~KGameDifficultyPrivate()
{
    delete KGameDifficulty::self();
}

void KGameDifficultyPrivate::init(KXmlGuiWindow *window, const QObject *recvr, const char *slotStandard, const char *slotCustom = nullptr)
{
    Q_ASSERT(recvr != nullptr);

    m_oldSelection = -1; // No valid selection
    m_level = KGameDifficulty::NoLevel;
    m_running = false;

    QObject::connect(this, SIGNAL(standardLevelChanged(KGameDifficulty::standardLevel)), recvr, slotStandard);
    if (slotCustom != nullptr)
        QObject::connect(this, SIGNAL(customLevelChanged(int)), recvr, slotCustom);

    m_menu = new KSelectAction(QIcon::fromTheme(QStringLiteral("games-difficult")), i18nc("Game difficulty level", "Difficulty"), window);
    m_menu->setToolTip(i18n("Set the difficulty level"));
    m_menu->setWhatsThis(i18n("Set the difficulty level of the game."));
    connect(m_menu, &KSelectAction::indexTriggered, this, &KGameDifficultyPrivate::changeSelection);
    m_menu->setObjectName(QStringLiteral("options_game_difficulty"));
    window->actionCollection()->addAction(m_menu->objectName(), m_menu);

    setParent(window);

    m_comboBox = new KComboBox(window);
    m_comboBox->setToolTip(i18n("Difficulty"));
    connect(m_comboBox, &QComboBox::activated, this, &KGameDifficultyPrivate::changeSelection);
    window->statusBar()->addPermanentWidget(m_comboBox);

    KGameDifficulty::setRestartOnChange(KGameDifficulty::RestartOnChange);
}

void KGameDifficultyPrivate::changeSelection(int newSelection)
{
    if (newSelection != m_oldSelection) {
        bool mayChange = true;

        if (mayChange && (m_restartOnChange == KGameDifficulty::RestartOnChange) && m_running)
            mayChange = (KMessageBox::warningContinueCancel(nullptr,
                                                            i18n("Changing the difficulty level will end the current game!"),
                                                            QString(),
                                                            KGuiItem(i18n("Change the difficulty level")))
                         == KMessageBox::Continue);

        if (mayChange) {
            setSelection(newSelection);
        } else {
            // restore current level selection
            setSelection(m_oldSelection);
        }
    }
}

QPair<QByteArray, QString> KGameDifficultyPrivate::standardLevelString(KGameDifficulty::standardLevel level)
{
    // The first entry in the pair is to be used as a key so don't change it. It doesn't have to match the string to be translated
    switch (level) {
    case KGameDifficulty::RidiculouslyEasy:
        return qMakePair(QByteArray("Ridiculously Easy"), i18nc("Game difficulty level 1 out of 8", "Ridiculously Easy"));
    case KGameDifficulty::VeryEasy:
        return qMakePair(QByteArray("Very Easy"), i18nc("Game difficulty level 2 out of 8", "Very Easy"));
    case KGameDifficulty::Easy:
        return qMakePair(QByteArray("Easy"), i18nc("Game difficulty level 3 out of 8", "Easy"));
    case KGameDifficulty::Medium:
        return qMakePair(QByteArray("Medium"), i18nc("Game difficulty level 4 out of 8", "Medium"));
    case KGameDifficulty::Hard:
        return qMakePair(QByteArray("Hard"), i18nc("Game difficulty level 5 out of 8", "Hard"));
    case KGameDifficulty::VeryHard:
        return qMakePair(QByteArray("Very Hard"), i18nc("Game difficulty level 6 out of 8", "Very Hard"));
    case KGameDifficulty::ExtremelyHard:
        return qMakePair(QByteArray("Extremely Hard"), i18nc("Game difficulty level 7 out of 8", "Extremely Hard"));
    case KGameDifficulty::Impossible:
        return qMakePair(QByteArray("Impossible"), i18nc("Game difficulty level 8 out of 8", "Impossible"));
    case KGameDifficulty::Custom:
    case KGameDifficulty::Configurable:
    case KGameDifficulty::NoLevel:
        // Do nothing
        break;
    }
    return qMakePair(QByteArray(), QString());
}

void KGameDifficultyPrivate::rebuildActions()
{
    m_menu->clear();
    m_comboBox->clear();
    std::sort(m_standardLevels.begin(), m_standardLevels.end());

    for (KGameDifficulty::standardLevel level : std::as_const(m_standardLevels)) {
        if (level != KGameDifficulty::Configurable) {
            m_menu->addAction(standardLevelString(level).second);
            m_comboBox->addItem(QIcon::fromTheme(QStringLiteral("games-difficult")), standardLevelString(level).second);
        }
    }

    if (!m_customLevels.isEmpty()) {
        for (const QString &s : std::as_const(m_customLevels)) {
            m_menu->addAction(s);
            m_comboBox->addItem(QIcon::fromTheme(QStringLiteral("games-difficult")), s);
        }
    }

    if (m_standardLevels.contains(KGameDifficulty::Configurable)) {
        QAction *separator = new QAction(m_menu);
        separator->setSeparator(true);
        m_menu->addAction(separator);

        QString s = i18nc("Name of the game difficulty level that is customized by the user by setting up different game parameters", "Custom");
        m_menu->addAction(s);
        m_comboBox->addItem(QIcon::fromTheme(QStringLiteral("games-difficult")), s);
    }

    // reselect the previous selected item.
    if (m_level == KGameDifficulty::Custom)
        KGameDifficulty::setLevelCustom(m_levelCustom);
    else if (m_standardLevels.contains(m_level))
        KGameDifficulty::setLevel(m_level);
}

void KGameDifficultyPrivate::setSelection(int newSelection)
{
    int countWithoutConfigurable = m_standardLevels.count();
    if (m_standardLevels.contains(KGameDifficulty::Configurable))
        countWithoutConfigurable--;

    if ((m_standardLevels.contains(KGameDifficulty::Configurable)) && (newSelection > m_menu->actions().count() - 3))
        KGameDifficulty::setLevel(KGameDifficulty::Configurable);
    else if (newSelection < countWithoutConfigurable)
        KGameDifficulty::setLevel(m_standardLevels[newSelection]);
    else
        KGameDifficulty::setLevelCustom((m_customLevels.keys()).value(newSelection - countWithoutConfigurable));

    m_oldSelection = newSelection;
}

void KGameDifficultyPrivate::setLevel(KGameDifficulty::standardLevel level)
{
    if ((!m_standardLevels.contains(level)) && (level != KGameDifficulty::Custom))
        level = KGameDifficulty::NoLevel;

    if (level == KGameDifficulty::Configurable) {
        m_menu->setCurrentItem(m_menu->actions().count() - 1);
        m_comboBox->setCurrentIndex(m_comboBox->count() - 1);
    } else if (level != KGameDifficulty::Custom) {
        int i = m_standardLevels.indexOf(level);
        m_menu->setCurrentItem(i);
        m_comboBox->setCurrentIndex(i);
    }

    if (level != m_level) {
        m_level = level;
        Q_EMIT standardLevelChanged(level);
    }

    m_oldSelection = m_menu->currentItem();
}

void KGameDifficultyPrivate::setLevelCustom(int key)
{
    m_level = KGameDifficulty::Custom;

    int a = m_standardLevels.count();
    if (m_standardLevels.contains(KGameDifficulty::Configurable))
        a -= 1;

    int i = (m_customLevels.keys()).indexOf(key) + a;
    m_menu->setCurrentItem(i);
    m_comboBox->setCurrentIndex(i);

    if (key != m_levelCustom) {
        m_levelCustom = key;
        Q_EMIT customLevelChanged(key);
    }

    m_oldSelection = m_menu->currentItem();
}

//---//

KGameDifficulty *KGameDifficulty::instance = nullptr;

KGameDifficulty::~KGameDifficulty()
{
    // We do not need to delete d, because d deletes us.
}

void KGameDifficulty::init(KXmlGuiWindow *window, const QObject *recvr, const char *slotStandard, const char *slotCustom)
{
    self()->d->init(window, recvr, slotStandard, slotCustom);
}

void KGameDifficulty::setRestartOnChange(onChange restart)
{
    Q_ASSERT(self()->d);

    self()->d->m_restartOnChange = restart;
    if (restart == RestartOnChange)
        self()->d->m_comboBox->setWhatsThis(
            i18n("Select the <b>difficulty</b> of the game.<br />If you change the difficulty level while a game is running, you will have to cancel it and "
                 "start a new one."));
    else
        self()->d->m_comboBox->setWhatsThis(i18n("Select the <b>difficulty</b> of the game.<br />You can change the difficulty level during a running game."));
}

void KGameDifficulty::addStandardLevel(standardLevel level)
{
    Q_ASSERT(self()->d);

    if ((level != Custom) && (level != NoLevel)) {
        self()->d->m_standardLevels.append(level);
        self()->d->rebuildActions();
    }
}

void KGameDifficulty::removeStandardLevel(standardLevel level)
{
    Q_ASSERT(self()->d);

    self()->d->m_standardLevels.removeAll(level);
    self()->d->rebuildActions();
}

void KGameDifficulty::addCustomLevel(int key, const QString &appellation)
{
    Q_ASSERT(self()->d);

    self()->d->m_customLevels.insert(key, appellation);
    self()->d->rebuildActions();
}

void KGameDifficulty::removeCustomLevel(int key)
{
    Q_ASSERT(self()->d);

    self()->d->m_customLevels.remove(key);
    self()->d->rebuildActions();
}

void KGameDifficulty::setEnabled(bool enabled)
{
    Q_ASSERT(self()->d->m_menu);

    // TODO: Doing this never disable the combobox in the toolbar (just in the menu). It seems to be a bug in the class KSelectAction of kdelibs/kdeui/actions.
    // To check and solve...
    self()->d->m_menu->setEnabled(enabled);
    self()->d->m_comboBox->setEnabled(enabled);
}

void KGameDifficulty::setLevel(standardLevel level)
{
    Q_ASSERT(self()->d);

    self()->d->setLevel(level);
}

void KGameDifficulty::setLevelCustom(int key)
{
    Q_ASSERT(self()->d);

    self()->d->setLevelCustom(key);
}

int KGameDifficulty::levelCustom()
{
    Q_ASSERT(self()->d);

    return self()->d->m_levelCustom;
}

KGameDifficulty::standardLevel KGameDifficulty::level()
{
    Q_ASSERT(self()->d);

    return self()->d->m_level;
}

QString KGameDifficulty::levelString()
{
    Q_ASSERT(self()->d);

    return self()->d->standardLevelString(self()->d->m_level).second;
}

QPair<QByteArray, QString> KGameDifficulty::localizedLevelString()
{
    Q_ASSERT(self()->d);

    return self()->d->standardLevelString(self()->d->m_level);
}

QMap<QByteArray, QString> KGameDifficulty::localizedLevelStrings()
{
    Q_ASSERT(self()->d);

    QMap<QByteArray, QString> levelStrings;

    levelStrings.insert(self()->d->standardLevelString(RidiculouslyEasy).first, self()->d->standardLevelString(RidiculouslyEasy).second);
    levelStrings.insert(self()->d->standardLevelString(VeryEasy).first, self()->d->standardLevelString(VeryEasy).second);
    levelStrings.insert(self()->d->standardLevelString(Easy).first, self()->d->standardLevelString(Easy).second);
    levelStrings.insert(self()->d->standardLevelString(Medium).first, self()->d->standardLevelString(Medium).second);
    levelStrings.insert(self()->d->standardLevelString(Hard).first, self()->d->standardLevelString(Hard).second);
    levelStrings.insert(self()->d->standardLevelString(VeryHard).first, self()->d->standardLevelString(VeryHard).second);
    levelStrings.insert(self()->d->standardLevelString(ExtremelyHard).first, self()->d->standardLevelString(ExtremelyHard).second);
    levelStrings.insert(self()->d->standardLevelString(Impossible).first, self()->d->standardLevelString(Impossible).second);

    return levelStrings;
}

QMap<int, QByteArray> KGameDifficulty::levelWeights()
{
    Q_ASSERT(self()->d);

    QMap<int, QByteArray> weights;
    weights.insert(RidiculouslyEasy, self()->d->standardLevelString(RidiculouslyEasy).first);
    weights.insert(VeryEasy, self()->d->standardLevelString(VeryEasy).first);
    weights.insert(Easy, self()->d->standardLevelString(Easy).first);
    weights.insert(Medium, self()->d->standardLevelString(Medium).first);
    weights.insert(Hard, self()->d->standardLevelString(Hard).first);
    weights.insert(VeryHard, self()->d->standardLevelString(VeryHard).first);
    weights.insert(ExtremelyHard, self()->d->standardLevelString(ExtremelyHard).first);
    weights.insert(Impossible, self()->d->standardLevelString(Impossible).first);

    return weights;
}

void KGameDifficulty::setRunning(bool running)
{
    Q_ASSERT(self()->d);

    self()->d->m_running = running;
}

KGameDifficulty::KGameDifficulty()
    : d(new KGameDifficultyPrivate())
{
}

KGameDifficulty *KGameDifficulty::self()
{
    if (instance == nullptr)
        instance = new KGameDifficulty();
    return instance;
}

#include "kgamedifficulty.moc"

#endif
