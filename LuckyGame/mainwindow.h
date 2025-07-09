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
#include <QPixmap> // NEW: For image icons

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
    void setInitialDeposit(); // Modified: Single deposit function for main menu

    // Slots Game Slots
    void spinSlots();
    void returnToMainMenuFromSlots();
    void updateSlotsAnimation();
    void stopSlotsAnimation();

    // Roulette Game Slots
    void spinRoulette();
    void returnToMainMenuFromRoulette();
    // void setInitialDepositRoulette(); // Removed: Deposit handled in main menu
    void placeBetRoulette(int type);
    void updateRouletteAnimation();
    void stopRouletteAnimation();

private:
    QStackedWidget *stackedWidget;
    double playerBalance; // Player's virtual currency balance

    // --- Main Menu Widgets ---
    QWidget *mainMenuWidget;
    QLabel *mainMenuBalanceLabel; // NEW: Balance label in main menu
    QLineEdit *mainMenuDepositInput; // NEW: Deposit input in main menu
    QPushButton *setDepositMainMenuButton; // NEW: Deposit button in main menu
    QPushButton *slotsButton;
    QPushButton *rouletteButton;
    QPushButton *exitButton;

    // --- Slots Game Widgets ---
    QWidget *slotsGameWidget;
    QLabel *slotsBalanceLabel; // Kept for display in game screen
    QLabel *slot1Label;
    QLabel *slot2Label;
    QLabel *slot3Label;
    QPushButton *spinSlotsButton;
    QPushButton *backFromSlotsButton;
    QLabel *slotsResultLabel;
    QLineEdit *slotsBetLineEdit;
    // QPushButton *setDepositSlotsButton; // Removed
    // QLineEdit *depositInputSlots;       // Removed

    QVector<QString> fruits = {"🍒", "🍋", "🍊", "🍇", "🍓"}; // Sticker "fruits"
    QTimer *slotsAnimationTimer;
    int slotsSpinCount;
    int finalSlot1Index, finalSlot2Index, finalSlot3Index;

    // --- Roulette Game Widgets ---
    QWidget *rouletteGameWidget;
    QLabel *rouletteBalanceLabel; // Kept for display in game screen
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
    // QPushButton *setDepositRouletteButton; // Removed
    // QLineEdit *depositInputRoulette;       // Removed

    QTimer *rouletteAnimationTimer;
    int rouletteSpinCount;
    int finalRouletteNumber;
    int currentBetType;


    // Helper functions
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
