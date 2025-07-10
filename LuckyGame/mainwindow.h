#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QStackedWidget>
#include <QVector>
#include <QRandomGenerator>
#include <QDoubleValidator>
#include <QIntValidator>
#include <QMessageBox>
#include <QTimer>
#include <QPixmap>
#include <QMediaPlayer>
#include <QAudioOutput>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Main Menu Slots
    void showSlotsGame();
    void showRouletteGame();
    void exitGame();
    void setInitialDeposit();

    void spinSlots();
    void returnToMainMenuFromSlots();
    void updateSlotsAnimation();
    void stopSlotsAnimation();

    void spinRoulette();
    void returnToMainMenuFromRoulette();
    void placeBetRoulette(int type);
    void updateRouletteAnimation();
    void stopRouletteAnimation();

private:
    QStackedWidget *stackedWidget;
    double playerBalance;

    QWidget *mainMenuWidget;
    QLabel *mainMenuBalanceLabel;
    QLineEdit *mainMenuDepositInput;
    QPushButton *setDepositMainMenuButton;
    QPushButton *slotsButton;
    QPushButton *rouletteButton;
    QPushButton *exitButton;

    QWidget *slotsGameWidget;
    QLabel *slotsBalanceLabel;
    QLabel *slot1Label;
    QLabel *slot2Label;
    QLabel *slot3Label;
    QPushButton *spinSlotsButton;
    QPushButton *backFromSlotsButton;
    QLabel *slotsResultLabel;
    QLineEdit *slotsBetLineEdit;

    QVector<QString> fruits = {"🍒", "🍋", "🍊", "🍇", "🍓"};
    QTimer *slotsAnimationTimer;
    int slotsSpinCount;
    int finalSlot1Index, finalSlot2Index, finalSlot3Index;

    QWidget *rouletteGameWidget;
    QLabel *rouletteBalanceLabel;
    QLabel *rouletteNumberDisplay;
    QPushButton *spinRouletteButton;
    QPushButton *betEvenButton;
    QPushButton *betOddButton;
    QPushButton *betRedButton;
    QPushButton *betBlackButton;
    QLineEdit *betNumberLineEdit;
    QPushButton *betOnNumberButton;
    QPushButton *backFromRouletteButton;
    QLabel *rouletteResultLabel;
    QLineEdit *rouletteBetLineEdit;

    QTimer *rouletteAnimationTimer;
    int rouletteSpinCount;
    int finalRouletteNumber;
    int currentBetType;

    QMediaPlayer* audioPlayer;
    QAudioOutput* audioOutput;

    void setupMainMenu();
    void setupSlotsGame();
    void setupRouletteGame();
    void updateBalanceDisplay();
    int getRouletteNumberColor(int number);
    void updateRouletteResult(int winningNumber);
    bool checkBetValidity(double betAmount);
    bool checkDepositValidity(double depositAmount);
};

#endif // MAINWINDOW_H
