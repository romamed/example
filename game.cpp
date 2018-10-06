#include "game.h"

void Game::createGame()
{
    procWay = QApplication::applicationDirPath();

    readDataFromFile();

    menuFlag = false;
    screensaverFlag = false;
    settingFlag = false;
    recordsFlag = false;
    gameMode = 0;
    blackRow = 0;

    sound = new QSound(procWay+"/sounds/KimBuran.wav");
    playSound('s');

    fillMatrix(1);

    // для анимации фона
    // [0] - красная составляющая 1го градиента, синяя 3го
    // [1] - синяя 1го градиента, красная 3го
    // [2] - направление изменения красной составляющей 1го градиента, противоположное направление красной 3го
    // [3] - красная 2го градиента
    // [4] - синяя 2го градиента
    // [5] - направление изменения красной составляющей 2го градиента
    // [6] - для определения центра градиента
    bgColor[0] = 20; bgColor[1] = 220; bgColor[2] = -1;
    bgColor[3] = 120; bgColor[4] = 120; bgColor[5] = -1;
    bgColor[6] = 3;

    screensaverFlag = true;
    movie = new QMovie(":/screensaver.gif");
    movie->start();
}

void Game::fillMatrix(int matrix)
{
    if (matrix == 1)
    {
        glassMatrix = new int*[17];
        for (int i = 0; i < 16; i++)
            glassMatrix[i] = new int [sizeGlass];
        glassMatrix[16] = new int [6];
    }
    else if (matrix == 2)
    {
        // ширина облака стакана = нутро стакана (sizeGlass - 6) +
        // + правая рамка единиц толщиной 3 (чтоб палки длинные ложить)
        cloudGlass = new int* [5];
        for (int i = 0; i < 4; i++)
            cloudGlass[i] = new int [sizeGlass - 3];
        // первое число - номер столбца в 4й строке(точка отсчета фигуры)
        // второе число - тип фигуры
        // третье число - положение фигуры
        cloudGlass[4] = new int [3];
    }
}

bool Game::readDataFromFile()
{
    // потому что вызывается в main, без createGame()
    procWay = QApplication::applicationDirPath();

    QFile file(procWay+"/data.txt");

    if (!file.open(QIODevice::ReadOnly))
    {
        file.close();
        return false;
    }
    else
    {
        QTextStream in(&file);

        in >> sizeGlass;

        records = new int[10];

        for (int i = 0; i < 10; i++)
            in >> records[i];

        file.close();

        return true;
    }
}

void Game::writeDataFromFile()
{
    QFile file(procWay+"/data.txt");

    if (!file.open(QIODevice::WriteOnly))
    {
        QSound::play(procWay+"/sounds/error.wav");
        QMessageBox::critical(0, "Ошибка", "Запись данных невозможна!", QMessageBox::Ok | QMessageBox::Default | QMessageBox::Escape);
    }
    else
    {
        QTextStream in(&file);

        in << sizeGlass << " ";

        for (int i = 0; i < 10; i++)
            in << records[i] << " ";
    }

    file.close();
}

void Game::redrawScreensaver(QPixmap *pm, QPainter *painter)
{
    QRadialGradient radialGrad(QPointF(pm->width()/2, pm->height()/2), pm->height());
    radialGrad.setColorAt(0, QColor(bgColor[0], 0, bgColor[1]));
    radialGrad.setColorAt(0.5, QColor(bgColor[3], 0, bgColor[4]));
    radialGrad.setColorAt(1, QColor(bgColor[1], 0, bgColor[0]));

    QBrush brush(radialGrad);

    painter->fillRect(painter->viewport(), brush);

    painter->drawPixmap(pm->width() / 2 - 999 / 2, pm->height() / 2 - 114 / 2, movie->currentPixmap());
}

void Game::clearGlass(bool resizeFlag)
{
    finishGame = 0;
    score = 0;

    if (resizeFlag)
    {
        int color = glassMatrix[16][0];

        for (int i = 0; i < 17; i++)
            delete [] glassMatrix[i];

        for (int i = 0; i < 16; i++)
            glassMatrix[i] = new int [sizeGlass];
        glassMatrix[16] = new int [6];
        glassMatrix[16][0] = color;
    }

    // заполняем матрицу стакана
    for (int i = 3; i < 13; i++) // заполняем тело стакана нулями
        for (int j = 3; j < sizeGlass - 3; j++)
            glassMatrix[i][j] = 0;
    for (int i = 0; i < 3; i++) // заполняем рамку стакана единицами(верх+низ)
        for (int j = 0; j < sizeGlass; j++)
            glassMatrix[i][j] = 1;
    for (int i = 13; i < 16; i++)
        for (int j = 0; j < sizeGlass; j++)
            glassMatrix[i][j] = 1;
    for (int i = 0; i < 16; i++) // заполняем рамку стакана единицами(слева+справа)
        for (int j = 0; j < 3; j++)
            glassMatrix[i][j] = 1;
    for (int i = 0; i < 16; i++)
        for (int j = sizeGlass - 3; j < sizeGlass; j++)
            glassMatrix[i][j] = 1;
    for (int j = 3; j < sizeGlass - 3; j++) // верхушку стакана заполняем двойками
        glassMatrix[2][j] = 2;
}

void Game::clearCloud()
{
    for (int i = 0; i < 4; i++) // заполняем тело стакана нулями
        for (int j = 0; j < sizeGlass - 6; j++)
            cloudGlass[i][j] = 0;

    for (int i = 0; i < 4; i++) // заполняем правую рамку
        for (int j = sizeGlass - 6; j < sizeGlass - 3; j++)
            cloudGlass[i][j] = 2;
}

void Game::redrawMenu(QPixmap *pm, QPainter *painter)
{
    menuFlag = true;

    RedrawGlass *redraw = new RedrawMenu(new RedrawBackground);
    redraw->redraw(pm, painter, bgColor, glassMatrix, &sizeGlass, NULL, &finishGame, 0, &score);
    delete redraw;
}

void Game::redrawRecords(QPixmap *pm, QPainter *painter)
{
    RedrawGlass *redraw = new RedrawRecords(new RedrawBackground);
    // вместо sizeGlass отправляем records
    redraw->redraw(pm, painter, bgColor, glassMatrix, records, NULL, &finishGame, 0, &score);
    delete redraw;
}

void Game::redrawSetting(QPixmap *pm, QPainter *painter)
{
    RedrawGlass *redraw = new RedrawSetting(new RedrawBackground);
    redraw->redraw(pm, painter, bgColor, glassMatrix, &sizeGlass, NULL, &finishGame, 0, &score);
    delete redraw;
}

void Game::redrawGlassCP(QPixmap *pm, QPainter *painter)
{
    RedrawGlass *redraw = new RedrawComputerPlayerInfo(new RedrawFirstPlayerInfo(new RedrawFrame(new RedrawBackground)));
    redraw->redraw(pm, painter, bgColor, glassMatrix, &sizeGlass, NULL, &finishGame, blackRow, &score);
    delete redraw;
}

void Game::redrawGlassSP(QPixmap *pm, QPainter *painter)
{
    RedrawGlass *redraw = new RedrawSecondPlayerInfo(new RedrawFirstPlayerInfo(new RedrawFrame(new RedrawBackground)));
    redraw->redraw(pm, painter, bgColor, glassMatrix, &sizeGlass, cloudGlass, &finishGame, blackRow, &score);
    delete redraw;
}

// работа с фигурой № 1  ооо
//                       о
int Game::insertFigure1CP()
{
    //perfectFlag-флаг идеального местоположения фигуры
    bool perfectFlag = true;
    //место утопленной фигуры, первая ячейка-самое глубокое место(строка)
    int sinkPlace[8];
    sinkPlace[0] = 0;
    qDebug()<<"работа с 1й фигурой";

    for (int i = 12; i > 2; i--)
    {
        for (int j = 3; j < sizeGlass - 3; j++)
            if (glassMatrix[i][j] == 0)
            {
                // обработка 3го положения ооо
                //                         еее
                if (glassMatrix[i][j+1] % 10 == 1)
                    perfectFlag = false;
                if (glassMatrix[i][j+2] % 10 == 1)
                    perfectFlag = false;
                for (int a = i-1; a > 2; a--)
                    for (int b = j; b < j+3; b++)
                        if (glassMatrix[a][b] % 10 == 1)
                            perfectFlag = false;

                if (perfectFlag)
                {
                    if (sinkPlace[0] == 0)
                    {
                        sinkPlace[0] = i; sinkPlace[1] = j;
                        sinkPlace[2] = i; sinkPlace[3] = j+1;
                        sinkPlace[4] = i; sinkPlace[5] = j+2;
                        sinkPlace[6] = i-1; sinkPlace[7] = j+2;
                    }

                    if (glassMatrix[i+1][j] % 10 == 1 && glassMatrix[i+1][j+1] % 10 == 1 && glassMatrix[i+1][j+2] % 10 == 1)
                    {
                        glassMatrix[i][j] = glassMatrix[16][1] * 10 + 1;
                        glassMatrix[i][j+1] = glassMatrix[16][1] * 10 + 1;
                        glassMatrix[i][j+2] = glassMatrix[16][1] * 10 + 1;
                        glassMatrix[i-1][j+2] = glassMatrix[16][1] * 10 + 1;

                        score+=10;

                        qDebug()<<"фигура 1 положение 3";
                        return 0;
                    }
                }
                else
                    perfectFlag = true;

                // обработка 4го положения оо
                //                         ее
                if (glassMatrix[i][j+1] % 10 == 1)
                    perfectFlag = false;
                for (int a = i-1; a > 2; a--)
                    for (int b = j; b < j+2; b++)
                        if (glassMatrix[a][b] % 10 == 1)
                            perfectFlag = false;

                if (perfectFlag)
                {
                    if (sinkPlace[0] == 0)
                    {
                        sinkPlace[0] = i; sinkPlace[1] = j;
                        sinkPlace[2] = i; sinkPlace[3] = j+1;
                        sinkPlace[4] = i-1; sinkPlace[5] = j;
                        sinkPlace[6] = i-2; sinkPlace[7] = j;
                    }

                    if (glassMatrix[i+1][j] % 10 == 1 && glassMatrix[i+1][j+1] % 10 == 1)
                    {
                        glassMatrix[i][j] = glassMatrix[16][1] * 10 + 1;
                        glassMatrix[i][j+1] = glassMatrix[16][1] * 10 + 1;
                        glassMatrix[i-1][j] = glassMatrix[16][1] * 10 + 1;
                        glassMatrix[i-2][j] = glassMatrix[16][1] * 10 + 1;

                        score+=10;

                        qDebug()<<"фигура 1 положение 4";
                        return 0;
                    }
                }
                else
                    perfectFlag = true;

                // обработка 1го положения ооо
                //                         оее
                //                         е
                for (int a = i-1; a > 2; a--)
                    for (int b = j; b < j+3; b++)
                        if (glassMatrix[a][b] % 10 == 1)
                            perfectFlag = false;

                if (perfectFlag)
                {
                    if (sinkPlace[0] == 0)
                    {
                        sinkPlace[0] = i; sinkPlace[1] = j;
                        sinkPlace[2] = i-1; sinkPlace[3] = j;
                        sinkPlace[4] = i-1; sinkPlace[5] = j+1;
                        sinkPlace[6] = i-1; sinkPlace[7] = j+2;
                    }

                    if (glassMatrix[i+1][j] % 10 == 1 && glassMatrix[i][j+1] % 10 == 1 && glassMatrix[i][j+2] % 10 == 1)
                    {
                        glassMatrix[i][j] = glassMatrix[16][1] * 10 + 1;
                        glassMatrix[i-1][j] = glassMatrix[16][1] * 10 + 1;
                        glassMatrix[i-1][j+1] = glassMatrix[16][1] * 10 + 1;
                        glassMatrix[i-1][j+2] = glassMatrix[16][1] * 10 + 1;

                        score+=10;

                        qDebug()<<"фигура 1 положение 1";
                        return 0;
                    }
                }
                else
                    perfectFlag = true;

                // обработка 2го положения оо
                //                         ео
                //                         ео
                //                          е
                if (glassMatrix[i-1][j] % 10 == 1)
                    perfectFlag = false;
                for (int a = i-2; a > 2; a--)
                    for (int b = j-1; b < j+1; b++)
                        if (glassMatrix[a][b] % 10 == 1)
                            perfectFlag = false;

                if (perfectFlag)
                {
                    if (sinkPlace[0] == 0)
                    {
                        sinkPlace[0] = i; sinkPlace[1] = j;
                        sinkPlace[2] = i-1; sinkPlace[3] = j;
                        sinkPlace[4] = i-2; sinkPlace[5] = j-1;
                        sinkPlace[6] = i-2; sinkPlace[7] = j;
                    }

                    if (glassMatrix[i+1][j] % 10 == 1 && glassMatrix[i][j-1] % 10 == 1 && glassMatrix[i-1][j-1] % 10 == 1)
                    {
                        glassMatrix[i-2][j-1] = glassMatrix[16][1] * 10 + 1;
                        glassMatrix[i-2][j] = glassMatrix[16][1] * 10 + 1;
                        glassMatrix[i-1][j] = glassMatrix[16][1] * 10 + 1;
                        glassMatrix[i][j] = glassMatrix[16][1] * 10 + 1;

                        score+=10;

                        qDebug()<<"фигура 1 положение 2";
                        return 0;
                    }
                }
                else
                    perfectFlag = true;
            }

        if (sinkPlace[0] != 0 && 12 - i > 3)
        {
            for (int k = 0; k < 8; k+=2)
                glassMatrix[sinkPlace[k]][sinkPlace[k+1]] = glassMatrix[16][1] * 10 + 1;

            score++;

            qDebug()<<"фигура 1 утоплена";
            return 0;
        }
    }
    finishGame = 1;
    return 1;
}

// работа с фигурой № 2   оо
//                       оо
int Game::insertFigure2CP()
{
    //perfectFlag-флаг идеального местоположения фигуры
    bool perfectFlag = true;
    //место утопленной фигуры, первая ячейка-самое глубокое место(строка)
    int sinkPlace[8];
    sinkPlace[0] = 0;
    qDebug()<<"работа со 2й фигурой";

    for (int i = 12; i > 2; i--)
    {
        for (int j = 3; j < sizeGlass - 3; j++)
            if (glassMatrix[i][j] == 0)
            {
                // обработка 1го положения  оо
                //                         оое
                //                         ее
                if (glassMatrix[i][j+1] % 10 == 1)
                    perfectFlag = false;
                for (int a = i-1; a > 2; a--)
                    for (int b = j; b < j+3; b++)
                        if (glassMatrix[a][b] % 10 == 1)
                            perfectFlag = false;

                if (perfectFlag)
                {
                    if (sinkPlace[0] == 0)
                    {
                        sinkPlace[0] = i; sinkPlace[1] = j;
                        sinkPlace[2] = i; sinkPlace[3] = j+1;
                        sinkPlace[4] = i-1; sinkPlace[5] = j+1;
                        sinkPlace[6] = i-1; sinkPlace[7] = j+2;
                    }

                    if (glassMatrix[i+1][j] % 10 == 1 && glassMatrix[i+1][j+1] % 10 == 1 && glassMatrix[i][j+2] % 10 == 1)
                    {
                        glassMatrix[i][j] = glassMatrix[16][2] * 10 + 1;
                        glassMatrix[i][j+1] = glassMatrix[16][2] * 10 + 1;
                        glassMatrix[i-1][j+1] = glassMatrix[16][2] * 10 + 1;
                        glassMatrix[i-1][j+2] = glassMatrix[16][2] * 10 + 1;

                        score+=10;

                        qDebug()<<"фигура 2 положение 1";
                        return 0;
                    }
                }
                else
                    perfectFlag = true;

                // обработка 2го положения о
                //                         оо
                //                         ео
                //                          е
                for (int a = i-1; a > 2; a--)
                    for (int b = j-1; b < j+1; b++)
                        if (glassMatrix[a][b] % 10 == 1)
                            perfectFlag = false;

                if (perfectFlag)
                {
                    if (sinkPlace[0] == 0)
                    {
                        sinkPlace[0] = i; sinkPlace[1] = j;
                        sinkPlace[2] = i-1; sinkPlace[3] = j;
                        sinkPlace[4] = i-1; sinkPlace[5] = j-1;
                        sinkPlace[6] = i-2; sinkPlace[7] = j-1;
                    }

                    if (glassMatrix[i][j-1] % 10 == 1 && glassMatrix[i+1][j] % 10 == 1)
                    {
                        glassMatrix[i][j] = glassMatrix[16][2] * 10 + 1;
                        glassMatrix[i-1][j] = glassMatrix[16][2] * 10 + 1;
                        glassMatrix[i-1][j-1] = glassMatrix[16][2] * 10 + 1;
                        glassMatrix[i-2][j-1] = glassMatrix[16][2] * 10 + 1;

                        score+=10;

                        qDebug()<<"фигура 2 положение 2";
                        return 0;
                    }
                }
                else
                    perfectFlag = true;
            }

        if (sinkPlace[0] != 0 && 12 - i > 3)
        {
            for (int k = 0; k < 8; k+=2)
                glassMatrix[sinkPlace[k]][sinkPlace[k+1]] = glassMatrix[16][2] * 10 + 1;

            score++;

            qDebug()<<"фигура 2 утоплена";
            return 0;
        }
    }
    finishGame = 1;
    return 1;
}

// работа с фигурой № 3  оо
//                       оо
int Game::insertFigure3CP()
{
    //perfectFlag-флаг идеального местоположения фигуры
    bool perfectFlag = true;
    //место утопленной фигуры, первая ячейка-самое глубокое место(строка)
    int sinkPlace[8];
    sinkPlace[0] = 0;
    qDebug()<<"работа с 3й фигурой";

    for (int i = 12; i > 2; i--)
    {
        for (int j = 3; j < sizeGlass - 3; j++)
            if (glassMatrix[i][j] == 0)
            {
                // обработка 1го положения оо
                //                         ее
                if (glassMatrix[i][j+1] % 10 == 1)
                    perfectFlag = false;
                for (int a = i-1; a > 2; a--)
                    for (int b = j; b < j+2; b++)
                        if (glassMatrix[a][b] % 10 == 1)
                            perfectFlag = false;

                if (perfectFlag)
                {
                    if (sinkPlace[0] == 0)
                    {
                        sinkPlace[0] = i; sinkPlace[1] = j;
                        sinkPlace[2] = i; sinkPlace[3] = j+1;
                        sinkPlace[4] = i-1; sinkPlace[5] = j;
                        sinkPlace[6] = i-1; sinkPlace[7] = j+1;
                    }

                    if (glassMatrix[i+1][j] % 10 == 1 && glassMatrix[i+1][j+1] % 10 == 1)
                    {
                        glassMatrix[i][j] = glassMatrix[16][3] * 10 + 1;
                        glassMatrix[i][j+1] = glassMatrix[16][3] * 10 + 1;
                        glassMatrix[i-1][j] = glassMatrix[16][3] * 10 + 1;
                        glassMatrix[i-1][j+1] = glassMatrix[16][3] * 10 + 1;

                        score+=10;

                        qDebug()<<"фигура 3 положение 1";
                        return 0;
                    }
                }
                else
                    perfectFlag = true;
            }

        if (sinkPlace[0] != 0 && 12 - i > 3)
        {
            for (int k = 0; k < 8; k+=2)
                glassMatrix[sinkPlace[k]][sinkPlace[k+1]] = glassMatrix[16][3] * 10 + 1;

            score++;

            qDebug()<<"фигура 3 утоплена";
            return 0;
        }
    }
    finishGame = 1;
    return 1;
}

// работа с фигурой № 4 оооо
int Game::insertFigure4CP()
{
    //perfectFlag-флаг идеального местоположения фигуры
    bool perfectFlag = true;
    //место утопленной фигуры, первая ячейка-самое глубокое место(строка)
    int sinkPlace[8];
    sinkPlace[0] = 0;
    qDebug()<<"работа с 4й фигурой";

    for (int i = 12; i > 2; i--)
    {
        for (int j = 3; j < sizeGlass - 3; j++)
            if (glassMatrix[i][j] == 0)
            {
                // обработка 1го положения оооо
                //                         ееее
                if (glassMatrix[i][j+1] % 10 == 1)
                    perfectFlag = false;
                if (glassMatrix[i][j+2] % 10 == 1)
                    perfectFlag = false;
                if (glassMatrix[i][j+3] % 10 == 1)
                    perfectFlag = false;
                for (int a = i-1; a > 2; a--)
                    for (int b = j; b < j+4; b++)
                        if (glassMatrix[a][b] % 10 == 1)
                            perfectFlag = false;

                if (perfectFlag)
                {
                    if (sinkPlace[0] == 0)
                    {
                        sinkPlace[0] = i; sinkPlace[1] = j;
                        sinkPlace[2] = i; sinkPlace[3] = j+1;
                        sinkPlace[4] = i; sinkPlace[5] = j+2;
                        sinkPlace[6] = i; sinkPlace[7] = j+3;
                    }

                    if (glassMatrix[i+1][j] % 10 == 1 && glassMatrix[i+1][j+1] % 10 == 1 && glassMatrix[i+1][j+2] % 10 == 1 && glassMatrix[i+1][j+3] % 10 == 1)
                    {
                        glassMatrix[i][j] = glassMatrix[16][4] * 10 + 1;
                        glassMatrix[i][j+1] = glassMatrix[16][4] * 10 + 1;
                        glassMatrix[i][j+2] = glassMatrix[16][4] * 10 + 1;
                        glassMatrix[i][j+3] = glassMatrix[16][4] * 10 + 1;

                        score+=10;

                        qDebug()<<"фигура 4 положение 1";
                        return 0;
                    }
                }
                else
                    perfectFlag = true;

                // обработка 2го положения о
                //                         о
                //                         о
                //                         о
                //                         е
                if (glassMatrix[i-1][j] % 10 == 1)
                    perfectFlag = false;
                if (glassMatrix[i-2][j] % 10 == 1)
                    perfectFlag = false;
                if (glassMatrix[i-3][j] % 10 == 1)
                    perfectFlag = false;
                for (int a = i-4; a > 2; a--)
                    if (glassMatrix[a][j] % 10 == 1)
                        perfectFlag = false;

                if (perfectFlag)
                {
                    if (sinkPlace[0] == 0)
                    {
                        sinkPlace[0] = i; sinkPlace[1] = j;
                        sinkPlace[2] = i-1; sinkPlace[3] = j;
                        sinkPlace[4] = i-2; sinkPlace[5] = j;
                        sinkPlace[6] = i-3; sinkPlace[7] = j;
                    }

                    if (glassMatrix[i+1][j] % 10 == 1)
                    {
                        glassMatrix[i][j] = glassMatrix[16][4] * 10 + 1;
                        glassMatrix[i-1][j] = glassMatrix[16][4] * 10 + 1;
                        glassMatrix[i-2][j] = glassMatrix[16][4] * 10 + 1;
                        glassMatrix[i-3][j] = glassMatrix[16][4] * 10 + 1;

                        score+=10;

                        qDebug()<<"фигура 4 положение 2";
                        return 0;
                    }
                }
                else
                    perfectFlag = true;
            }

        if (sinkPlace[0] != 0 && 12 - i > 3)
        {
            for (int k = 0; k < 8; k+=2)
                glassMatrix[sinkPlace[k]][sinkPlace[k+1]] = glassMatrix[16][4] * 10 + 1;

            score++;

            qDebug()<<"фигура 4 утоплена";
            return 0;
        }
    }
    finishGame = 1;
    return 1;
}

// работа с фигурой № 5  ооо
//                        о
int Game::insertFigure5CP()
{
    //perfectFlag-флаг идеального местоположения фигуры
    bool perfectFlag = true;
    //место утопленной фигуры, первая ячейка-самое глубокое место(строка)
    int sinkPlace[8];
    sinkPlace[0] = 0;
    qDebug()<<"работа с 5й фигурой";

    for (int i = 12; i > 2; i--)
    {
        for (int j = 3; j < sizeGlass - 3; j++)
            if (glassMatrix[i][j] == 0)
            {
                // обработка 1го положения ооо
                //                         еое
                //                          е
                for (int a = i-1; a > 2; a--)
                    for (int b = j-1; b < j+2; b++)
                        if (glassMatrix[a][b] % 10 == 1)
                            perfectFlag = false;

                if (perfectFlag)
                {
                    if (sinkPlace[0] == 0)
                    {
                        sinkPlace[0] = i; sinkPlace[1] = j;
                        sinkPlace[2] = i-1; sinkPlace[3] = j-1;
                        sinkPlace[4] = i-1; sinkPlace[5] = j;
                        sinkPlace[6] = i-1; sinkPlace[7] = j+1;
                    }

                    if (glassMatrix[i][j-1] % 10 == 1 && glassMatrix[i+1][j] % 10 == 1 && glassMatrix[i][j+1] % 10 == 1)
                    {
                        glassMatrix[i][j] = glassMatrix[16][5] * 10 + 1;
                        glassMatrix[i-1][j-1] = glassMatrix[16][5] * 10 + 1;
                        glassMatrix[i-1][j] = glassMatrix[16][5] * 10 + 1;
                        glassMatrix[i-1][j+1] = glassMatrix[16][5] * 10 + 1;

                        score+=10;

                        qDebug()<<"фигура 5 положение 1";
                        return 0;
                    }
                }
                else
                    perfectFlag = true;

                // обработка 2го положения  о
                //                         ооо
                //                         еее
                if (glassMatrix[i][j+1] % 10 == 1)
                    perfectFlag = false;
                if (glassMatrix[i][j+2] % 10 == 1)
                    perfectFlag = false;
                for (int a = i-1; a > 2; a--)
                    for (int b = j; b < j+3; b++)
                        if (glassMatrix[a][b] % 10 == 1)
                            perfectFlag = false;

                if (perfectFlag)
                {
                    if (sinkPlace[0] == 0)
                    {
                        sinkPlace[0] = i; sinkPlace[1] = j;
                        sinkPlace[2] = i; sinkPlace[3] = j+1;
                        sinkPlace[4] = i; sinkPlace[5] = j+2;
                        sinkPlace[6] = i-1; sinkPlace[7] = j+1;
                    }

                    if (glassMatrix[i+1][j] % 10 == 1 && glassMatrix[i+1][j+1] % 10 == 1 && glassMatrix[i+1][j+2] % 10 == 1)
                    {
                        glassMatrix[i][j] = glassMatrix[16][5] * 10 + 1;
                        glassMatrix[i][j+1] = glassMatrix[16][5] * 10 + 1;
                        glassMatrix[i][j+2] = glassMatrix[16][5] * 10 + 1;
                        glassMatrix[i-1][j+1] = glassMatrix[16][5] * 10 + 1;

                        score+=10;

                        qDebug()<<"фигура 5 положение 2";
                        return 0;
                    }
                }
                else
                    perfectFlag = true;

                // обработка 3го положения о
                //                         оо
                //                         ое
                //                         е
                for (int a = i-1; a > 2; a--)
                    for (int b = j; b < j+2; b++)
                        if (glassMatrix[a][b] % 10 == 1)
                            perfectFlag = false;

                if (perfectFlag)
                {
                    if (sinkPlace[0] == 0)
                    {
                        sinkPlace[0] = i; sinkPlace[1] = j;
                        sinkPlace[2] = i-1; sinkPlace[3] = j;
                        sinkPlace[4] = i-1; sinkPlace[5] = j+1;
                        sinkPlace[6] = i-2; sinkPlace[7] = j;
                    }

                    if (glassMatrix[i+1][j] % 10 == 1 && glassMatrix[i][j+1] % 10 == 1)
                    {
                        glassMatrix[i][j] = glassMatrix[16][5] * 10 + 1;
                        glassMatrix[i-1][j] = glassMatrix[16][5] * 10 + 1;
                        glassMatrix[i-1][j+1] = glassMatrix[16][5] * 10 + 1;
                        glassMatrix[i-2][j] = glassMatrix[16][5] * 10 + 1;

                        score+=10;

                        qDebug()<<"фигура 5 положение 3";
                        return 0;
                    }
                }
                else
                    perfectFlag = true;

                // обработка 4го положения  о
                //                         оо
                //                         ео
                //                          е
                for (int a = i-1; a > 2; a--)
                    for (int b = j-1; b < j+1; b++)
                        if (glassMatrix[a][b] % 10 == 1)
                            perfectFlag = false;

                if (perfectFlag)
                {
                    if (sinkPlace[0] == 0)
                    {
                        sinkPlace[0] = i; sinkPlace[1] = j;
                        sinkPlace[2] = i-1; sinkPlace[3] = j-1;
                        sinkPlace[4] = i-1; sinkPlace[5] = j;
                        sinkPlace[6] = i-2; sinkPlace[7] = j;
                    }

                    if (glassMatrix[i][j-1] % 10 == 1 && glassMatrix[i+1][j] % 10 == 1)
                    {
                        glassMatrix[i][j] = glassMatrix[16][5] * 10 + 1;
                        glassMatrix[i-1][j-1] = glassMatrix[16][5] * 10 + 1;
                        glassMatrix[i-1][j] = glassMatrix[16][5] * 10 + 1;
                        glassMatrix[i-2][j] = glassMatrix[16][5] * 10 + 1;

                        score+=10;

                        qDebug()<<"фигура 5 положение 4";
                        return 0;
                    }
                }
                else
                    perfectFlag = true;
            }

        if (sinkPlace[0] != 0 && 12 - i > 3)
        {
            for (int k = 0; k < 8; k+=2)
                glassMatrix[sinkPlace[k]][sinkPlace[k+1]] = glassMatrix[16][5] * 10 + 1;

            score++;

            qDebug()<<"фигура 5 утоплена";
            return 0;
        }
    }
    finishGame = 1;
    return 1;
}

void Game::insertFigureCloud(int figure)
{
    int shift = 4;

    if (sizeGlass == 26)
        shift = 9;

    switch (figure)
    {
    case 1:
        cloudGlass[3][shift] = glassMatrix[16][1] * 10 + 1;
        cloudGlass[3][shift + 1] = glassMatrix[16][1] * 10 + 1;
        cloudGlass[2][shift] = glassMatrix[16][1] * 10 + 1;
        cloudGlass[1][shift] = glassMatrix[16][1] * 10 + 1;

        cloudGlass[4][0] = shift;
        cloudGlass[4][1] = 1;
        cloudGlass[4][2] = 1;
        break;
    case 2:
        cloudGlass[3][shift + 1] = glassMatrix[16][2] * 10 + 1;
        cloudGlass[2][shift] = glassMatrix[16][2] * 10 + 1;
        cloudGlass[2][shift + 1] = glassMatrix[16][2] * 10 + 1;
        cloudGlass[1][shift] = glassMatrix[16][2] * 10 + 1;

        cloudGlass[4][0] = shift;
        cloudGlass[4][1] = 2;
        cloudGlass[4][2] = 1;
        break;
    case 3:
        cloudGlass[3][shift] = glassMatrix[16][3] * 10 + 1;
        cloudGlass[3][shift + 1] = glassMatrix[16][3] * 10 + 1;
        cloudGlass[2][shift] = glassMatrix[16][3] * 10 + 1;
        cloudGlass[2][shift + 1] = glassMatrix[16][3] * 10 + 1;

        cloudGlass[4][0] = shift;
        cloudGlass[4][1] = 3;
        cloudGlass[4][2] = 1;
        break;
    case 4:
        cloudGlass[3][shift] = glassMatrix[16][4] * 10 + 1;
        cloudGlass[3][shift + 1] = glassMatrix[16][4] * 10 + 1;
        cloudGlass[3][shift + 2] = glassMatrix[16][4] * 10 + 1;
        cloudGlass[3][shift + 3] = glassMatrix[16][4] * 10 + 1;

        cloudGlass[4][0] = shift;
        cloudGlass[4][1] = 4;
        cloudGlass[4][2] = 1;
        break;
    case 5:
        cloudGlass[3][shift] = glassMatrix[16][5] * 10 + 1;
        cloudGlass[2][shift] = glassMatrix[16][5] * 10 + 1;
        cloudGlass[2][shift + 1] = glassMatrix[16][5] * 10 + 1;
        cloudGlass[1][shift] = glassMatrix[16][5] * 10 + 1;

        cloudGlass[4][0] = shift;
        cloudGlass[4][1] = 5;
        cloudGlass[4][2] = 1;
        break;
    }
}

void Game::changeFigureCloud(char mode)
{
    bool moveFlag = true;

    switch (mode)
    {
    case 'l':
        for (int i = 0; i < 4; i++)
            if (cloudGlass[i][0] % 10 == 1)
            {
                moveFlag = false;
                break;
            }

        if (!moveFlag)
            break;

        for (int i = 0; i < 4; i++)
            for (int j = 0; j < sizeGlass - 6; j++)
                if (cloudGlass[i][j] % 10 == 1)
                {
                    cloudGlass[i][j - 1] = cloudGlass[i][j];
                    cloudGlass[i][j] = 0;
                }

        cloudGlass[4][0]--;
        break;

    case 'r':
        for (int i = 0; i < 4; i++)
            if (cloudGlass[i][sizeGlass - 7] % 10 == 1)
            {
                moveFlag = false;
                break;
            }

        if (!moveFlag)
            break;

        for (int i = 0; i < 4; i++)
            for (int j = sizeGlass - 7; j > -1; j--)
                if (cloudGlass[i][j] % 10 == 1)
                {
                    cloudGlass[i][j + 1] = cloudGlass[i][j];
                    cloudGlass[i][j] = 0;
                }

        cloudGlass[4][0]++;
        break;

    case 'u':
        switch (cloudGlass[4][1]) // тип фигуры тут
        {
        case 1: // 1я фигура
            switch (cloudGlass[4][2]) // положение фигуры
            {
            case 1: // 1я фигура 1е положение
                if (cloudGlass[3][cloudGlass[4][0] + 2] == 0)
                {
                    cloudGlass[3][cloudGlass[4][0] + 1] = 0;
                    cloudGlass[2][cloudGlass[4][0] + 1] = glassMatrix[16][1] * 10 + 1;
                    cloudGlass[2][cloudGlass[4][0] + 2] = glassMatrix[16][1] * 10 + 1;
                    cloudGlass[1][cloudGlass[4][0]] = 0;

                    cloudGlass[4][2]++;
                }
                else
                {
                    cloudGlass[3][cloudGlass[4][0] - 1] = glassMatrix[16][1] * 10 + 1;
                    cloudGlass[3][cloudGlass[4][0]] = 0;
                    cloudGlass[3][cloudGlass[4][0] + 1] = 0;
                    cloudGlass[2][cloudGlass[4][0] - 1] = glassMatrix[16][1] * 10 + 1;
                    cloudGlass[2][cloudGlass[4][0] + 1] = glassMatrix[16][1] * 10 + 1;
                    cloudGlass[1][cloudGlass[4][0]] = 0;

                    cloudGlass[4][0]--;
                    cloudGlass[4][2]++;
                }
                break;

            case 2: // 1я фигура 2е положение
                cloudGlass[3][cloudGlass[4][0]] = 0;
                cloudGlass[3][cloudGlass[4][0] + 1] = glassMatrix[16][1] * 10 + 1;
                cloudGlass[2][cloudGlass[4][0]] = 0;
                cloudGlass[2][cloudGlass[4][0] + 2] = 0;
                cloudGlass[1][cloudGlass[4][0]] = glassMatrix[16][1] * 10 + 1;
                cloudGlass[1][cloudGlass[4][0] + 1] = glassMatrix[16][1] * 10 + 1;

                cloudGlass[4][2]++;
                break;

            case 3: // 1я фигура 3е положение
                if (cloudGlass[3][cloudGlass[4][0] + 2] == 0)
                {
                    cloudGlass[3][cloudGlass[4][0]] = glassMatrix[16][1] * 10 + 1;
                    cloudGlass[3][cloudGlass[4][0] + 2] = glassMatrix[16][1] * 10 + 1;
                    cloudGlass[2][cloudGlass[4][0] + 1] = 0;
                    cloudGlass[2][cloudGlass[4][0] + 2] = glassMatrix[16][1] * 10 + 1;
                    cloudGlass[1][cloudGlass[4][0]] = 0;
                    cloudGlass[1][cloudGlass[4][0] + 1] = 0;

                    cloudGlass[4][2]++;
                }
                else
                {
                    cloudGlass[3][cloudGlass[4][0] - 1] = glassMatrix[16][1] * 10 + 1;
                    cloudGlass[3][cloudGlass[4][0]] = glassMatrix[16][1] * 10 + 1;
                    cloudGlass[1][cloudGlass[4][0]] = 0;
                    cloudGlass[1][cloudGlass[4][0] + 1] = 0;

                    cloudGlass[4][0]--;
                    cloudGlass[4][2]++;
                }
                break;

            case 4: // 1я фигура 4е положение
                cloudGlass[3][cloudGlass[4][0] + 2] = 0;
                cloudGlass[2][cloudGlass[4][0]] = glassMatrix[16][1] * 10 + 1;
                cloudGlass[2][cloudGlass[4][0] + 2] = 0;
                cloudGlass[1][cloudGlass[4][0]] = glassMatrix[16][1] * 10 + 1;

                cloudGlass[4][2] -= 3;
                break;
            }
            break;

        case 2: // 2я фигура
            switch (cloudGlass[4][2]) // положение фигуры
            {
            case 1: // 2я фигура 1е положение
                if (cloudGlass[3][cloudGlass[4][0] + 2] == 0)
                {
                    cloudGlass[3][cloudGlass[4][0]] = glassMatrix[16][2] * 10 + 1;
                    cloudGlass[2][cloudGlass[4][0]] = 0;
                    cloudGlass[2][cloudGlass[4][0] + 2] = glassMatrix[16][2] * 10 + 1;
                    cloudGlass[1][cloudGlass[4][0]] = 0;

                    cloudGlass[4][2]++;
                }
                else
                {
                    cloudGlass[3][cloudGlass[4][0] - 1] = glassMatrix[16][2] * 10 + 1;
                    cloudGlass[3][cloudGlass[4][0]] = glassMatrix[16][2] * 10 + 1;
                    cloudGlass[3][cloudGlass[4][0] + 1] = 0;
                    cloudGlass[1][cloudGlass[4][0]] = 0;

                    cloudGlass[4][0]--;
                    cloudGlass[4][2]++;
                }
                break;

            case 2: // 2я фигура 2е положение
                cloudGlass[3][cloudGlass[4][0]] = 0;
                cloudGlass[2][cloudGlass[4][0]] = glassMatrix[16][2] * 10 + 1;
                cloudGlass[2][cloudGlass[4][0] + 2] = 0;
                cloudGlass[1][cloudGlass[4][0]] = glassMatrix[16][2] * 10 + 1;

                cloudGlass[4][2]--;
                break;
            }
            break;

        case 3: // 3я фигура
            // эта фигура - квадрат
            break;

        case 4: // 4я фигура
            switch (cloudGlass[4][2]) // положение фигуры
            {
            case 1: // 4я фигура 1е положение
                cloudGlass[3][cloudGlass[4][0] + 1] = 0;
                cloudGlass[3][cloudGlass[4][0] + 2] = 0;
                cloudGlass[3][cloudGlass[4][0] + 3] = 0;
                cloudGlass[2][cloudGlass[4][0]] = glassMatrix[16][4] * 10 + 1;
                cloudGlass[1][cloudGlass[4][0]] = glassMatrix[16][4] * 10 + 1;
                cloudGlass[0][cloudGlass[4][0]] = glassMatrix[16][4] * 10 + 1;

                cloudGlass[4][2]++;
                break;

            case 2: // 4я фигура 2е положение
                if (cloudGlass[3][cloudGlass[4][0] + 3] == 0)
                {
                    cloudGlass[3][cloudGlass[4][0] + 1] = glassMatrix[16][4] * 10 + 1;
                    cloudGlass[3][cloudGlass[4][0] + 2] = glassMatrix[16][4] * 10 + 1;
                    cloudGlass[3][cloudGlass[4][0] + 3] = glassMatrix[16][4] * 10 + 1;
                    cloudGlass[2][cloudGlass[4][0]] = 0;
                    cloudGlass[1][cloudGlass[4][0]] = 0;
                    cloudGlass[0][cloudGlass[4][0]] = 0;

                    cloudGlass[4][2]--;
                }
                else if (cloudGlass[3][cloudGlass[4][0] + 2] == 0)
                {
                    cloudGlass[3][cloudGlass[4][0] - 1] = glassMatrix[16][4] * 10 + 1;
                    cloudGlass[3][cloudGlass[4][0] + 1] = glassMatrix[16][4] * 10 + 1;
                    cloudGlass[3][cloudGlass[4][0] + 2] = glassMatrix[16][4] * 10 + 1;
                    cloudGlass[2][cloudGlass[4][0]] = 0;
                    cloudGlass[1][cloudGlass[4][0]] = 0;
                    cloudGlass[0][cloudGlass[4][0]] = 0;

                    cloudGlass[4][0]--;
                    cloudGlass[4][2]--;
                }
                else if (cloudGlass[3][cloudGlass[4][0] + 1] == 0)
                {
                    cloudGlass[3][cloudGlass[4][0] - 2] = glassMatrix[16][4] * 10 + 1;
                    cloudGlass[3][cloudGlass[4][0] - 1] = glassMatrix[16][4] * 10 + 1;
                    cloudGlass[3][cloudGlass[4][0] + 1] = glassMatrix[16][4] * 10 + 1;
                    cloudGlass[2][cloudGlass[4][0]] = 0;
                    cloudGlass[1][cloudGlass[4][0]] = 0;
                    cloudGlass[0][cloudGlass[4][0]] = 0;

                    cloudGlass[4][0] -= 2;
                    cloudGlass[4][2]--;

                }
                else
                {
                    cloudGlass[3][cloudGlass[4][0] - 3] = glassMatrix[16][4] * 10 + 1;
                    cloudGlass[3][cloudGlass[4][0] - 2] = glassMatrix[16][4] * 10 + 1;
                    cloudGlass[3][cloudGlass[4][0] - 1] = glassMatrix[16][4] * 10 + 1;
                    cloudGlass[2][cloudGlass[4][0]] = 0;
                    cloudGlass[1][cloudGlass[4][0]] = 0;
                    cloudGlass[0][cloudGlass[4][0]] = 0;

                    cloudGlass[4][0] -= 3;
                    cloudGlass[4][2]--;

                }
                break;
            }
            break;

        case 5: // 5я фигура
            switch (cloudGlass[4][2]) // положение фигуры
            {
            case 1: // 5я фигура 1е положение
                if (cloudGlass[3][cloudGlass[4][0] + 2] == 0)
                {
                    cloudGlass[3][cloudGlass[4][0]] = 0;
                    cloudGlass[3][cloudGlass[4][0] + 1] = glassMatrix[16][5] * 10 + 1;
                    cloudGlass[2][cloudGlass[4][0] + 2] = glassMatrix[16][5] * 10 + 1;
                    cloudGlass[1][cloudGlass[4][0]] = 0;

                    cloudGlass[4][2]++;
                }
                else
                {
                    cloudGlass[2][cloudGlass[4][0] - 1] = glassMatrix[16][5] * 10 + 1;
                    cloudGlass[1][cloudGlass[4][0]] = 0;

                    cloudGlass[4][0]--;
                    cloudGlass[4][2]++;
                }
                break;

            case 2: // 5я фигура 2е положение
                cloudGlass[2][cloudGlass[4][0] + 2] = 0;
                cloudGlass[1][cloudGlass[4][0] + 1] = glassMatrix[16][5] * 10 + 1;

                cloudGlass[4][2]++;
                break;

            case 3: // 5я фигура 3е положение
                if (cloudGlass[3][cloudGlass[4][0] + 2] == 0)
                {
                    cloudGlass[3][cloudGlass[4][0]] = glassMatrix[16][5] * 10 + 1;
                    cloudGlass[3][cloudGlass[4][0] + 2] = glassMatrix[16][5] * 10 + 1;
                    cloudGlass[2][cloudGlass[4][0]] = 0;
                    cloudGlass[1][cloudGlass[4][0] + 1] = 0;

                    cloudGlass[4][2]++;
                }
                else
                {
                    cloudGlass[3][cloudGlass[4][0] - 1] = glassMatrix[16][5] * 10 + 1;
                    cloudGlass[3][cloudGlass[4][0]] = glassMatrix[16][5] * 10 + 1;
                    cloudGlass[2][cloudGlass[4][0] + 1] = 0;
                    cloudGlass[1][cloudGlass[4][0] + 1] = 0;

                    cloudGlass[4][0]--;
                    cloudGlass[4][2]++;
                }
                break;

            case 4: // 5я фигура 4е положение
                cloudGlass[3][cloudGlass[4][0] + 1] = 0;
                cloudGlass[3][cloudGlass[4][0] + 2] = 0;
                cloudGlass[2][cloudGlass[4][0]] = glassMatrix[16][5] * 10 + 1;
                cloudGlass[1][cloudGlass[4][0]] = glassMatrix[16][5] * 10 + 1;

                cloudGlass[4][2] -= 3;
                break;
            }
            break;
        }
        break;
    }
}

void Game::changeColorFigure(int object)
{
    // цвет фигур 1го игроке
    if (object == 1)
    {
        for (int j = 1; j <6; j++)
            glassMatrix[16][j] = 0;

        glassMatrix[16][1] = qrand()%4;
        glassMatrix[16][2] = qrand()%4;
        while (glassMatrix[16][1] == glassMatrix[16][2])
            glassMatrix[16][2] = qrand()%4;
        glassMatrix[16][3] = qrand()%4;
        while (glassMatrix[16][1] == glassMatrix[16][3] || glassMatrix[16][2] == glassMatrix[16][3])
            glassMatrix[16][3] = qrand()%4;
        glassMatrix[16][4] = 6 - glassMatrix[16][1] - glassMatrix[16][2] - glassMatrix[16][3];
        glassMatrix[16][5] = qrand()%4;
        while (glassMatrix[16][4] == glassMatrix[16][5])
            glassMatrix[16][5] = qrand()%4;
    }

    // цвет кубика в настройках
    if (object == 2)
        glassMatrix[16][0] = qrand()%4;
}

int Game::insertFigureSP()
{
    int minShift = 10; // мин. смещение 4й строки облака от верхушки стакана

    for (int i = 3; i > -1; i--)
        for (int j = cloudGlass[4][0]; j < cloudGlass[4][0] + 4; j++)
            if (cloudGlass[i][j] % 10 == 1)
            {
                for (int k = 1; k < 11; k++)
                    if (glassMatrix[k + 2][j + 3] % 10 == 1)
                    {
                        if (glassMatrix[k + 1][j + 3] == 0)
                        {
                            if (k + 2 - i < minShift)
                            {
                                minShift = k + 2 - i;
                            }
                        }
                        else
                        {
                            finishGame = 1;

                            return 0;
                        }

                        break;
                    }
            }

    for (int i = 3; i > -1; i--)
    {
        for (int j = cloudGlass[4][0]; j < cloudGlass[4][0] + 4; j++)
            if (cloudGlass[i][j] % 10 == 1)
            {
                if (glassMatrix[2 + minShift][j + 3] == 0)
                {
                    glassMatrix[2 + minShift][j + 3] = glassMatrix[16][cloudGlass[4][1]] * 10 + 1;
                    cloudGlass[i][j] = 0;
                }
                else
                {
                    finishGame = 1;

                    clearCloud();

                    return 0;
                }
            }

        minShift--;
    }
}

// удаляем полностью заполненные строки
void Game::findblackRow()
{
    bool rowFlag;
    for (int i = 3; i < 13; i++)
    {
        rowFlag = true;
        for (int j = 3; j < sizeGlass - 3; j++)
        {
            if (glassMatrix[i][j] == 0)
            {
                rowFlag = false;
                break;
            }
        }

        if (rowFlag == true)
        {
            score+=100;

            blackRow = i;

            return;
        }
    }
}

void Game::deleteRow()
{
    if (blackRow != 0)
    {
        for (int a = blackRow; a > 3; a--)
            for (int b = 3; b < sizeGlass - 3; b++)
                glassMatrix[a][b] = glassMatrix[a-1][b];
        for (int b = 3; b < sizeGlass - 3; b++)
            glassMatrix[3][b] = 0;

        blackRow = 0;
    }
}

void Game::resetRecords()
{
    for (int i = 0; i < 9; i++)
        records[i] = 99999;
}

// проверка победы игрока
void Game::stopGame()
{
    bool stopFlag = false;
    for (int j = 3; j < sizeGlass - 3; j++)
        if (glassMatrix[2][j] % 10 == 1)
        {
            stopFlag = true;
            break;
        }

    if (stopFlag)
    {
        finishGame = 1;

        int shift = 0;
        if (sizeGlass == 26)
            shift = 5;

        for (int i = shift; i < shift + 5; i++)
            if (score < records[i])
            {
                for (int j = 3; j > i - 1; j--)
                    records[j+1] = records[j];

                records[i] = score;
                break;
            }
    }
}

void Game::save()
{
    QFile file(procWay+"/save.txt");

    if (!file.open(QIODevice::WriteOnly))
    {
        QSound::play(procWay+"/sounds/error.wav");
        QMessageBox::critical(0, "Ошибка", "Сохранение невозможно!", QMessageBox::Ok | QMessageBox::Default | QMessageBox::Escape);
    }
    else
    {
        QTextStream in(&file);

        in << sizeGlass << " ";

        for (int i = 3; i < 13; i++)
            for (int j = 3; j < sizeGlass - 3; j++)
                in << glassMatrix[i][j] << " ";

        for (int j = 0; j < 6; j++)
            in << glassMatrix[16][j] << " ";

        in << gameMode << " ";
        in << score;
    }

    file.close();
}

void Game::playSound(char type)
{
    if (type == 's')
    {
        sound->setLoops(999);
        sound->play();
    }
    else if (type == 'm')
        QSound::play(procWay+"/sounds/click.wav");
    else if (type == 'f')
        QSound::play(procWay+"/sounds/figure.wav");
    else if (type == 'a')
        QSound::play(procWay+"/sounds/applause.wav");
}

bool Game::load(QPixmap *pm, QPainter *painter)
{
    QFile file(procWay+"/save.txt");

    if (!file.open(QIODevice::ReadOnly))
    {
        QSound::play(procWay+"/sounds/error.wav");
        file.close();
        QMessageBox::critical(0, "Ошибка", "Файл сохранения не найден!", QMessageBox::Ok | QMessageBox::Default | QMessageBox::Escape);
        return false;
    }
    else
    {
        QTextStream in(&file);

        int newSizeGlass;
        in >> newSizeGlass;

        if (newSizeGlass != sizeGlass)
        {
            sizeGlass = newSizeGlass;

            for (int i = 0; i < 17; i++)
                delete [] glassMatrix[i];

            for (int i = 0; i < 16; i++)
                glassMatrix[i] = new int [sizeGlass];
            glassMatrix[16] = new int [6];

            for (int i = 0; i < 3; i++) // заполняем рамку стакана единицами(верх+низ)
                for (int j = 0; j < sizeGlass; j++)
                    glassMatrix[i][j] = 1;
            for (int i = 13; i < 16; i++)
                for (int j = 0; j < sizeGlass; j++)
                    glassMatrix[i][j] = 1;
            for (int i = 0; i < 16; i++) // заполняем рамку стакана единицами(слева+справа)
                for (int j = 0; j < 3; j++)
                    glassMatrix[i][j] = 1;
            for (int i = 0; i < 16; i++)
                for (int j = sizeGlass - 3; j < sizeGlass; j++)
                    glassMatrix[i][j] = 1;
            for (int j = 3; j < sizeGlass - 3; j++) // верхушку стакана заполняем двойками
                glassMatrix[2][j] = 2;
        }

        for (int i = 3; i < 13; i++)
            for (int j = 3; j < sizeGlass - 3; j++)
                in >> glassMatrix[i][j];

        for (int j = 0; j < 6; j++)
            in >> glassMatrix[16][j];

        in >> gameMode;
        in >> score;

        menuFlag = false;

        file.close();

        return true;
    }
}

Memento* Game::createMemento()
{
    return new Memento(glassMatrix, &sizeGlass, &score);
}

void Game::reinstate(Memento *memento)
{
    int **oldData = memento->getState();

    for (int i = 3; i < 13; i++)
        for (int j = 3; j < sizeGlass - 3; j++)
            glassMatrix[i][j] = oldData[i - 3][j - 3];

    score = oldData[10][0];
}

void Game::deleteMatrix(int matrix)
{
    if (matrix == 1)
    {
        writeDataFromFile();

        for (int i = 0; i < 17; i++)
            delete [] glassMatrix[i];
        delete [] glassMatrix;
    }

    if (matrix == 2)
    {
        for (int i = 0; i < 5; i++)
            delete [] cloudGlass[i];
        delete [] cloudGlass;
    }
}
