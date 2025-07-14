#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <algorithm>
#include <QApplication>
#include <QFont>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), playerBalance(0.0) {
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    stackedWidget->setStyleSheet("background-color: #1A1A1A; "
                                 "color: #e0e0e0;");

    slotsAnimationTimer = new QTimer(this);
    connect(slotsAnimationTimer, &QTimer::timeout, this, &MainWindow::updateSlotsAnimation);

    rouletteAnimationTimer = new QTimer(this);
    connect(rouletteAnimationTimer, &QTimer::timeout, this, &MainWindow::updateRouletteAnimation);

    setupMainMenu();
    setupSlotsGame();
    setupRouletteGame();

    stackedWidget->addWidget(mainMenuWidget);
    stackedWidget->addWidget(slotsGameWidget);
    stackedWidget->addWidget(rouletteGameWidget);

    stackedWidget->setCurrentWidget(mainMenuWidget);
    setWindowTitle("Lucky Game - Casino Edition");
    setFixedSize(800, 600);

    audioPlayer = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);

    audioPlayer->setAudioOutput(audioOutput);
    audioPlayer->setSource(QUrl::fromLocalFile("background_music.mp3"));
    audioPlayer->play();
}

MainWindow::~MainWindow() {
    delete slotsAnimationTimer;
    delete rouletteAnimationTimer;
}

void MainWindow::setupMainMenu() {
    mainMenuWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(mainMenuWidget);
    layout->setAlignment(Qt::AlignCenter);

    QLabel *title = new QLabel("Lucky Casino");
    title->setFont(QFont("Impact", 60, QFont::Bold));
    title->setStyleSheet("color: #FFD700; "
                         "text-shadow: 2px 2px 4px #000000;");
    layout->addWidget(title, 0, Qt::AlignCenter);

    QHBoxLayout *depositLayout = new QHBoxLayout();
    depositLayout->setAlignment(Qt::AlignCenter);
    QLabel *depositLabel = new QLabel("Deposit Funds:");
    depositLabel->setFont(QFont("Arial", 16));
    depositLabel->setStyleSheet("color: #e0e0e0;");
    depositLayout->addWidget(depositLabel);

    mainMenuDepositInput = new QLineEdit();
    mainMenuDepositInput->setPlaceholderText("Enter deposit amount");
    mainMenuDepositInput->setValidator(new QDoubleValidator(0.0, 1000000.0, 2, this));
    mainMenuDepositInput->setFixedSize(150, 30);
    mainMenuDepositInput->setStyleSheet("QLineEdit { "
                                        "background-color: #f0f0f0;"
                                        " color: #333333; "
                                        "border: 1px solid #555; "
                                        "padding: 5px; border-radius: 3px; "
                                        "}");
    depositLayout->addWidget(mainMenuDepositInput);

    setDepositMainMenuButton = new QPushButton("Set Deposit");
    setDepositMainMenuButton->setFixedSize(120, 30);
    setDepositMainMenuButton->setStyleSheet("QPushButton {"
                                            "    background-color: #4CAF50;"
                                            "    color: white;"
                                            "    font-size: 14px;"
                                            "    border-radius: 5px;"
                                            "}"
                                            "QPushButton:hover { background-color: #45a049; }");
    depositLayout->addWidget(setDepositMainMenuButton);
    layout->addLayout(depositLayout);
    layout->addSpacing(20);

    QHBoxLayout *balanceLayout = new QHBoxLayout();
    balanceLayout->setAlignment(Qt::AlignCenter);

    QLabel *coinIconLabel = new QLabel();
    QPixmap coinPixmap("coin.png");
    if (!coinPixmap.isNull()) {
        coinIconLabel->setPixmap(coinPixmap.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        coinIconLabel->setText("💰");
        coinIconLabel->setFont(QFont("Segoe UI Emoji", 24));
    }
    balanceLayout->addWidget(coinIconLabel);

    mainMenuBalanceLabel = new QLabel(QString("Balance: %1").arg(playerBalance, 0, 'f', 2));
    mainMenuBalanceLabel->setFont(QFont("Arial", 22, QFont::Bold));
    mainMenuBalanceLabel->setStyleSheet("color: #FFD700; "
                                        "background-color: #333333;"
                                        " padding: 5px 10px; "
                                        "border-radius: 5px;");
    balanceLayout->addWidget(mainMenuBalanceLabel);
    layout->addLayout(balanceLayout);
    layout->addSpacing(30); // More space

    // Strict button styling
    QString strictButtonStyle = "QPushButton {"
                                "    background-color: #B71C1C;"
                                "    color: #FFD700;"
                                "    font-size: 24px;"
                                "    font-weight: bold;"
                                "    border: 3px solid #FFD700;"
                                "    border-radius: 10px;"
                                "    padding: 15px 30px;"
                                "}"
                                "QPushButton:hover {"
                                "    background-color: #C62828;"
                                "}"
                                "QPushButton:pressed {"
                                "    background-color: #A11818;"
                                "    border: 3px solid #FFA000;"
                                "}";

    slotsButton = new QPushButton("PLAY SLOTS");
    slotsButton->setFixedSize(300, 70);
    slotsButton->setStyleSheet(strictButtonStyle);
    layout->addWidget(slotsButton, 0, Qt::AlignCenter);

    rouletteButton = new QPushButton("PLAY ROULETTE");
    rouletteButton->setFixedSize(300, 70);
    rouletteButton->setStyleSheet(strictButtonStyle);
    layout->addWidget(rouletteButton, 0, Qt::AlignCenter);

    exitButton = new QPushButton("EXIT GAME");
    exitButton->setFixedSize(300, 70);
    exitButton->setStyleSheet("QPushButton {"
                              "    background-color: #424242;"
                              "    color: #E0E0E0;"
                              "    font-size: 20px;"
                              "    font-weight: bold;"
                              "    border: 2px solid #757575;"
                              "    border-radius: 8px;"
                              "    padding: 10px 20px;"
                              "}"
                              "QPushButton:hover {"
                              "    background-color: #616161;"
                              "}"
                              "QPushButton:pressed {"
                              "    background-color: #212121;"
                              "}");
    layout->addWidget(exitButton, 0, Qt::AlignCenter);

    connect(slotsButton, &QPushButton::clicked, this, &MainWindow::showSlotsGame);
    connect(rouletteButton, &QPushButton::clicked, this, &MainWindow::showRouletteGame);
    connect(exitButton, &QPushButton::clicked, this, &QApplication::quit);
    connect(setDepositMainMenuButton, &QPushButton::clicked, this, &MainWindow::setInitialDeposit);
}

void MainWindow::setupSlotsGame() {
    slotsGameWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(slotsGameWidget);
    layout->setAlignment(Qt::AlignCenter);

    slotsBalanceLabel = new QLabel(QString("Balance: %1").arg(playerBalance, 0, 'f', 2));
    slotsBalanceLabel->setFont(QFont("Arial", 22, QFont::Bold));
    slotsBalanceLabel->setStyleSheet("color: #FFD700;"
                                     "background-color: #333333; "
                                     "padding: 5px; "
                                     "border-radius: 5px;");
    layout->addWidget(slotsBalanceLabel, 0, Qt::AlignLeft);

    QHBoxLayout *slotsLayout = new QHBoxLayout();
    slot1Label = new QLabel("🍒");
    slot2Label = new QLabel("🍋");
    slot3Label = new QLabel("🍊");

    QFont slotFont("Segoe UI Emoji", 80);
    slot1Label->setFont(slotFont);
    slot2Label->setFont(slotFont);
    slot3Label->setFont(slotFont);

    QString slotStyle = "QLabel { "
                        "background-color: #222222; "
                        "border: 3px solid #FFD700; "
                        "border-radius: 10px; "
                        "padding: 10px; "
                        "}";
    slot1Label->setStyleSheet(slotStyle);
    slot2Label->setStyleSheet(slotStyle);
    slot3Label->setStyleSheet(slotStyle);

    slot1Label->setAlignment(Qt::AlignCenter);
    slot2Label->setAlignment(Qt::AlignCenter);
    slot3Label->setAlignment(Qt::AlignCenter);

    slotsLayout->addWidget(slot1Label);
    slotsLayout->addWidget(slot2Label);
    slotsLayout->addWidget(slot3Label);
    layout->addLayout(slotsLayout);

    QHBoxLayout *betLayout = new QHBoxLayout();
    QLabel *betLabel = new QLabel("Bet Amount:");
    betLabel->setStyleSheet("color: #e0e0e0;");
    slotsBetLineEdit = new QLineEdit();
    slotsBetLineEdit->setPlaceholderText("e.g., 10");
    slotsBetLineEdit->setValidator(new QDoubleValidator(0.0, 1000000.0, 2, this));
    slotsBetLineEdit->setStyleSheet("QLineEdit {"
                                    "background-color: #f0f0f0; "
                                    "color: #333333; "
                                    "border: 1px solid #555; "
                                    "padding: 5px; "
                                    "border-radius: 3px; "
                                    "}");
    betLayout->addWidget(betLabel);
    betLayout->addWidget(slotsBetLineEdit);
    layout->addLayout(betLayout);

    spinSlotsButton = new QPushButton("Spin!");
    spinSlotsButton->setFixedSize(180, 50);
    spinSlotsButton->setStyleSheet("QPushButton {"
                                   "    background-color: #FF5722;"
                                   "    color: white;"
                                   "    font-size: 24px;"
                                   "    border-radius: 10px;"
                                   "    padding: 8px 16px;"
                                   "}"
                                   "QPushButton:hover {"
                                   "    background-color: #e64a19;"
                                   "}");
    layout->addWidget(spinSlotsButton, 0, Qt::AlignCenter);

    slotsResultLabel = new QLabel("Spin to win!");
    slotsResultLabel->setFont(QFont("Arial", 18, QFont::Bold));
    slotsResultLabel->setStyleSheet("color: #FFEB3B;");
    layout->addWidget(slotsResultLabel, 0, Qt::AlignCenter);

    backFromSlotsButton = new QPushButton("Back to Main Menu");
    backFromSlotsButton->setStyleSheet("QPushButton {"
                                       "    background-color: #607D8B;"
                                       "    color: white;"
                                       "    font-size: 16px;"
                                       "    border-radius: 8px;"
                                       "    padding: 6px 12px;"
                                       "}"
                                       "QPushButton:hover {"
                                       "    background-color: #455A64;"
                                       "}");
    layout->addWidget(backFromSlotsButton, 0, Qt::AlignRight);

    connect(spinSlotsButton, &QPushButton::clicked, this, &MainWindow::spinSlots);
    connect(backFromSlotsButton, &QPushButton::clicked, this, &MainWindow::returnToMainMenuFromSlots);
}

void MainWindow::setupRouletteGame() {
    rouletteGameWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(rouletteGameWidget);
    layout->setAlignment(Qt::AlignCenter);

    rouletteBalanceLabel = new QLabel(QString("Balance: %1").arg(playerBalance, 0, 'f', 2));
    rouletteBalanceLabel->setFont(QFont("Arial", 22, QFont::Bold));
    rouletteBalanceLabel->setStyleSheet("color: #FFD700; "
                                        "background-color: #333333;"
                                        " padding: 5px; "
                                        "border-radius: 5px;");
    layout->addWidget(rouletteBalanceLabel, 0, Qt::AlignLeft);

    rouletteNumberDisplay = new QLabel("?");
    rouletteNumberDisplay->setFont(QFont("Arial", 80, QFont::Bold));
    rouletteNumberDisplay->setAlignment(Qt::AlignCenter);
    rouletteNumberDisplay->setFixedSize(180, 180);
    rouletteNumberDisplay->setStyleSheet("border: 5px solid #FFD700; "
                                         "background-color: #222222; "
                                         "border-radius: 90px; "
                                         "color: #e0e0e0;");
    layout->addWidget(rouletteNumberDisplay, 0, Qt::AlignCenter);

    spinRouletteButton = new QPushButton("Spin Roulette!");
    spinRouletteButton->setFixedSize(220, 60);
    spinRouletteButton->setStyleSheet("QPushButton {"
                                      "    background-color: #FF5722;"
                                      "    color: white;"
                                      "    font-size: 24px;"
                                      "    border-radius: 10px;"
                                      "    padding: 8px 16px;"
                                      "}"
                                      "QPushButton:hover {"
                                      "    background-color: #e64a19;"
                                      "}");
    layout->addWidget(spinRouletteButton, 0, Qt::AlignCenter);

    QHBoxLayout *betOptionsLayout = new QHBoxLayout();
    QLabel *betAmountLabel = new QLabel("Bet Amount:");
    betAmountLabel->setStyleSheet("color: #e0e0e0;");
    rouletteBetLineEdit = new QLineEdit();
    rouletteBetLineEdit->setPlaceholderText("e.g., 20");
    rouletteBetLineEdit->setValidator(new QDoubleValidator(0.0, 1000000.0, 2, this));
    rouletteBetLineEdit->setStyleSheet("QLineEdit {"
                                       " background-color: #f0f0f0; "
                                       "color: #333333; "
                                       "border: 1px solid #555;"
                                       " padding: 5px; "
                                       "border-radius: 3px; "
                                       "}");
    betOptionsLayout->addWidget(betAmountLabel);
    betOptionsLayout->addWidget(rouletteBetLineEdit);
    layout->addLayout(betOptionsLayout);

    QString betButtonStyle = "QPushButton {"
                             "    background-color: #1a73e8;"
                             "    color: white;"
                             "    font-size: 16px;"
                             "    border-radius: 8px;"
                             "    padding: 6px 12px;"
                             "}"
                             "QPushButton:hover {"
                             "    background-color: #166cd7;"
                             "}";

    QHBoxLayout *quickBetLayout = new QHBoxLayout();
    betEvenButton = new QPushButton("Bet Even");
    betEvenButton->setStyleSheet(betButtonStyle);
    betOddButton = new QPushButton("Bet Odd");
    betOddButton->setStyleSheet(betButtonStyle);
    betRedButton = new QPushButton("Bet Red");
    betRedButton->setStyleSheet(betButtonStyle);
    betBlackButton = new QPushButton("Bet Black");
    betBlackButton->setStyleSheet(betButtonStyle);
    quickBetLayout->addWidget(betEvenButton);
    quickBetLayout->addWidget(betOddButton);
    quickBetLayout->addWidget(betRedButton);
    quickBetLayout->addWidget(betBlackButton);
    layout->addLayout(quickBetLayout);

    QHBoxLayout *numberBetLayout = new QHBoxLayout();
    QLabel *betNumberLabel = new QLabel("Bet on Number (0-36):");
    betNumberLabel->setStyleSheet("color: #e0e0e0;");
    betNumberLineEdit = new QLineEdit();
    betNumberLineEdit->setPlaceholderText("e.g., 17");
    betNumberLineEdit->setValidator(new QIntValidator(0, 36, this));
    betNumberLineEdit->setStyleSheet("QLineEdit { "
                                     "background-color: #f0f0f0; "
                                     "color: #333333;"
                                     "border: 1px solid #555;"
                                     "padding: 5px;"
                                     "border-radius: 3px; "
                                     "}");
    betOnNumberButton = new QPushButton("Place Number Bet");
    betOnNumberButton->setStyleSheet(betButtonStyle);
    numberBetLayout->addWidget(betNumberLabel);
    numberBetLayout->addWidget(betNumberLineEdit);
    numberBetLayout->addWidget(betOnNumberButton);
    layout->addLayout(numberBetLayout);

    rouletteResultLabel = new QLabel("Spin to win!");
    rouletteResultLabel->setFont(QFont("Arial", 18, QFont::Bold));
    rouletteResultLabel->setStyleSheet("color: #FFEB3B;");
    layout->addWidget(rouletteResultLabel, 0, Qt::AlignCenter);

    backFromRouletteButton = new QPushButton("Back to Main Menu");
    backFromRouletteButton->setStyleSheet("QPushButton {"
                                          "    background-color: #607D8B;"
                                          "    color: white;"
                                          "    font-size: 16px;"
                                          "    border-radius: 8px;"
                                          "    padding: 6px 12px;"
                                          "}"
                                          "QPushButton:hover {"
                                          "    background-color: #455A64;"
                                          "}");
    layout->addWidget(backFromRouletteButton, 0, Qt::AlignRight);

    connect(spinRouletteButton, &QPushButton::clicked, this, &MainWindow::spinRoulette);
    connect(betEvenButton, &QPushButton::clicked, [this]() {
        currentBetType = 0;
        spinRoulette();
    });
    connect(betOddButton, &QPushButton::clicked, [this]() {
        currentBetType = 1;
        spinRoulette();
    });
    connect(betRedButton, &QPushButton::clicked, [this]() {
        currentBetType = 2;
        spinRoulette();
    });
    connect(betBlackButton, &QPushButton::clicked, [this]() {
        currentBetType = 3;
        spinRoulette();
    });
    connect(betOnNumberButton, &QPushButton::clicked, [this]() {
        currentBetType = 4;
        spinRoulette();
    });
    connect(backFromRouletteButton, &QPushButton::clicked, this, &MainWindow::returnToMainMenuFromRoulette);
}

void MainWindow::updateBalanceDisplay() {
    mainMenuBalanceLabel->setText(QString("Balance: %1").arg(playerBalance, 0, 'f', 2));
    slotsBalanceLabel->setText(QString("Balance: %1").arg(playerBalance, 0, 'f', 2));
    rouletteBalanceLabel->setText(QString("Balance: %1").arg(playerBalance, 0, 'f', 2));
}

bool MainWindow::checkBetValidity(double betAmount) {
    if (betAmount <= 0) {
        QMessageBox::warning(this, "Invalid Bet", "Bet amount must be positive.");
        return false;
    }
    if (playerBalance <= 0) {
        QMessageBox::warning(this, "No Funds", "Your balance is zero. Please deposit more funds.");
        return false;
    }
    if (betAmount > playerBalance) {
        QMessageBox::warning(this, "Insufficient Funds", "You cannot bet more than your current balance.");
        return false;
    }
    return true;
}

bool MainWindow::checkDepositValidity(double depositAmount) {
    if (depositAmount <= 0) {
        QMessageBox::warning(this, "Invalid Deposit", "Deposit amount must be positive.");
        return false;
    }
    return true;
}

void MainWindow::showSlotsGame() {
    stackedWidget->setCurrentWidget(slotsGameWidget);
    updateBalanceDisplay();
    slotsResultLabel->setText("Spin to win!");
    slotsBetLineEdit->clear();
    slot1Label->setText("🍒");
    slot2Label->setText("🍋");
    slot3Label->setText("🍊");
    slotsResultLabel->setStyleSheet("color: #FFEB3B;");
}

void MainWindow::showRouletteGame() {
    stackedWidget->setCurrentWidget(rouletteGameWidget);
    updateBalanceDisplay();
    rouletteResultLabel->setText("Spin to win!");
    rouletteBetLineEdit->clear();
    betNumberLineEdit->clear();
    rouletteNumberDisplay->setText("?");
    rouletteNumberDisplay->setStyleSheet("border: 5px solid #FFD700;"
                                         "background-color: #222222;"
                                         "border-radius: 90px; "
                                         "color: #e0e0e0;");
    rouletteResultLabel->setStyleSheet("color: #FFEB3B;");
    currentBetType = -1;
}

void MainWindow::exitGame() {
    QApplication::quit();
}

void MainWindow::setInitialDeposit() {
    bool ok;
    double deposit = mainMenuDepositInput->text().toDouble(&ok);
    if (checkDepositValidity(deposit)) {
        playerBalance = deposit;
        updateBalanceDisplay();
        QMessageBox::information(this, "Deposit Set", QString("Your balance is now: %1").arg(playerBalance, 0, 'f', 2));
    }
}

void MainWindow::spinSlots() {
    double betAmount = slotsBetLineEdit->text().toDouble();
    if (!checkBetValidity(betAmount)) {
        return;
    }

    playerBalance -= betAmount;
    updateBalanceDisplay();

    finalSlot1Index = QRandomGenerator::global()->bounded(fruits.size());
    finalSlot2Index = QRandomGenerator::global()->bounded(fruits.size());
    finalSlot3Index = QRandomGenerator::global()->bounded(fruits.size());

    slotsSpinCount = 0;
    slotsAnimationTimer->start(50);
    spinSlotsButton->setEnabled(false);
}

void MainWindow::updateSlotsAnimation() {
    slot1Label->setText(fruits[QRandomGenerator::global()->bounded(fruits.size())]);
    slot2Label->setText(fruits[QRandomGenerator::global()->bounded(fruits.size())]);
    slot3Label->setText(fruits[QRandomGenerator::global()->bounded(fruits.size())]);

    slotsSpinCount++;
    if (slotsSpinCount >= 40) {
        stopSlotsAnimation();
    }
}

void MainWindow::stopSlotsAnimation() {
    slotsAnimationTimer->stop();
    spinSlotsButton->setEnabled(true);

    slot1Label->setText(fruits[finalSlot1Index]);
    slot2Label->setText(fruits[finalSlot2Index]);
    slot3Label->setText(fruits[finalSlot3Index]);

    if (fruits[finalSlot1Index] == fruits[finalSlot2Index] && fruits[finalSlot2Index] == fruits[finalSlot3Index]) {
        double winnings = slotsBetLineEdit->text().toDouble() * 5;
        playerBalance += winnings;
        slotsResultLabel->setText(QString("🎉 YOU WIN! +%1 🎉").arg(winnings, 0, 'f', 2));
        slotsResultLabel->setStyleSheet("color: #00FF00;");
    } else {
        slotsResultLabel->setText("😔 YOU LOSE! Try again. 😔");
        slotsResultLabel->setStyleSheet("color: #FF0000;");
    }
    updateBalanceDisplay();
}

void MainWindow::returnToMainMenuFromSlots() {
    slotsAnimationTimer->stop();
    spinSlotsButton->setEnabled(true);
    stackedWidget->setCurrentWidget(mainMenuWidget);
    updateBalanceDisplay();
}

void MainWindow::spinRoulette() {
    double betAmount = rouletteBetLineEdit->text().toDouble();
    if (!checkBetValidity(betAmount)) {
        return;
    }

    playerBalance -= betAmount;
    updateBalanceDisplay();

    finalRouletteNumber = QRandomGenerator::global()->bounded(37);

    rouletteSpinCount = 0;
    rouletteAnimationTimer->start(30);
    spinRouletteButton->setEnabled(false);
    betEvenButton->setEnabled(false);
    betOddButton->setEnabled(false);
    betRedButton->setEnabled(false);
    betBlackButton->setEnabled(false);
    betOnNumberButton->setEnabled(false);
}

void MainWindow::updateRouletteAnimation() {
    int randomNum = QRandomGenerator::global()->bounded(37);
    rouletteNumberDisplay->setText(QString::number(randomNum));

    int color = getRouletteNumberColor(randomNum);
    QString styleSheet = "border: 5px solid #FFD700; "
                         "border-radius: 90px; "
                         "color: white; "
                         "font-size: 80px; "
                         "font-weight: bold;";
    if (color == 0) {
        styleSheet += "background-color: green;";
    } else if (color == 1) {
        styleSheet += "background-color: red;";
    } else {
        styleSheet += "background-color: black;";
    }
    rouletteNumberDisplay->setStyleSheet(styleSheet);

    rouletteSpinCount++;
    if (rouletteSpinCount >= 100) {
        stopRouletteAnimation();
    }
}

void MainWindow::stopRouletteAnimation() {
    rouletteAnimationTimer->stop();
    spinRouletteButton->setEnabled(true);
    betEvenButton->setEnabled(true);
    betOddButton->setEnabled(true);
    betRedButton->setEnabled(true);
    betBlackButton->setEnabled(true);
    betOnNumberButton->setEnabled(true);

    rouletteNumberDisplay->setText(QString::number(finalRouletteNumber));

    int color = getRouletteNumberColor(finalRouletteNumber);
    QString styleSheet = "border: 5px solid #FFD700;"
                         "border-radius: 90px;"
                         "color: white;"
                         "font-size: 80px;"
                         "font-weight: bold;";
    if (color == 0) {
        styleSheet += "background-color: green;";
    } else if (color == 1) {
        styleSheet += "background-color: red;";
    } else {
        styleSheet += "background-color: black;";
    }
    rouletteNumberDisplay->setStyleSheet(styleSheet);

    placeBetRoulette(currentBetType);
}

void MainWindow::placeBetRoulette(int type) {
    bool win = false;
    double winnings = 0;
    double betAmount = rouletteBetLineEdit->text().toDouble();

    if (type == 0) {
        if (finalRouletteNumber != 0 && finalRouletteNumber % 2 == 0) {
            win = true;
            winnings = betAmount * 2;
        }
    } else if (type == 1) {
        if (finalRouletteNumber % 2 != 0) {
            win = true;
            winnings = betAmount * 2;
        }
    } else if (type == 2) {
        QVector<int> redNumbers = {1, 3, 5, 7, 9, 12, 14, 16, 18, 19, 21, 23, 25, 27, 30, 32, 34, 36};
        if (std::find(redNumbers.begin(), redNumbers.end(), finalRouletteNumber) != redNumbers.end()) {
            win = true;
            winnings = betAmount * 2;
        }
    } else if (type == 3) {
        QVector<int> blackNumbers = {2, 4, 6, 8, 10, 11, 13, 15, 17, 20, 22, 24, 26, 28, 29, 31, 33, 35};
        if (std::find(blackNumbers.begin(), blackNumbers.end(), finalRouletteNumber) != blackNumbers.end()) {
            win = true;
            winnings = betAmount * 2;
        }
    } else if (type == 4) {
        bool ok;
        int betNumber = betNumberLineEdit->text().toInt(&ok);
        if (ok && betNumber >= 0 && betNumber <= 36) {
            if (finalRouletteNumber == betNumber) {
                win = true;
                winnings = betAmount * 36;
            }
        } else {
            QMessageBox::warning(this, "Invalid Number Bet", "Please enter a valid number between 0 and 36.");
            playerBalance += betAmount;
            updateBalanceDisplay();
            return;
        }
    } else if (type == -1) {
        rouletteResultLabel->setText(QString("Winning Number: %1").arg(finalRouletteNumber));
        rouletteResultLabel->setStyleSheet("color: #FFEB3B;");
        updateBalanceDisplay();
        return;
    }

    if (win) {
        playerBalance += winnings;
        rouletteResultLabel->setText(QString("🎉 YOU WIN! Winning Number: %1. +%2 🎉").arg(finalRouletteNumber).arg(winnings, 0, 'f', 2));
        rouletteResultLabel->setStyleSheet("color: #00FF00;");
    } else {
        playerBalance -= betAmount;
        rouletteResultLabel->setText(QString("😔 YOU LOSE! Winning Number: %1. 😔").arg(finalRouletteNumber));
        rouletteResultLabel->setStyleSheet("color: #FF0000;");
    }
    updateBalanceDisplay();
}

void MainWindow::returnToMainMenuFromRoulette() {
    rouletteAnimationTimer->stop();
    spinRouletteButton->setEnabled(true);
    betEvenButton->setEnabled(true);
    betOddButton->setEnabled(true);
    betRedButton->setEnabled(true);
    betBlackButton->setEnabled(true);
    betOnNumberButton->setEnabled(true);
    stackedWidget->setCurrentWidget(mainMenuWidget);
    updateBalanceDisplay();
}

int MainWindow::getRouletteNumberColor(int number) {
    if (number == 0) return 0;
    QVector<int> redNumbers = {1, 3, 5, 7, 9, 12, 14, 16, 18, 19, 21, 23, 25, 27, 30, 32, 34, 36};
    if (std::find(redNumbers.begin(), redNumbers.end(), number) != redNumbers.end()) {
        return 1;
    }
    return 2;
}

void MainWindow::updateRouletteResult(int winningNumber) {
}
