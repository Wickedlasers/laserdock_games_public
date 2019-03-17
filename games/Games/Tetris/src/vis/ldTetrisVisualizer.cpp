//
//  ldTetrisVisualizer.cpp
//  LaserdockVisualizer
//
//  Created by MEO 30/03/2017
//  Copyright (c) 2017 Wicked Lasers. All rights reserved.
//

#include "ldTetrisVisualizer.h"

#include <QtCore/QTime>

#include "ldCore/ldCore.h"
#include "ldCore/Helpers/Text/ldTextLabel.h"

/*
 * Scan all the pixels in the array:
 * for each ‘1’ pixel {
 * if the pixel to its left is ‘0’ or out of bounds then add a line from its bottom left to its top left
 * if the pixel above is ‘0’ or out of bounds then add a line from its top left to its top right
 * if the pixel to its right is ‘0’ or out of bounds then add a line from its top right to its bottom right
 * if the pixel below is ‘0’ or out of bounds then add a line from its bottom right to its bottom left }

 * Then compare all lines with each other and if the start and end of one line is equal to the end and start of another,
 * then they are the same line and one can be deleted and the other extended
*/

//ToDo: to save laser movement, maybe next Tetromino should be indicated by colour of fallen bricks
//ToDo: assign a colour to each line

// ldTetrisVisualizer
ldTetrisVisualizer::ldTetrisVisualizer() : ldAbstractGameVisualizer() {
    m_soundEffects.insert(SFX::DIE, ldCore::instance()->resourceDir() + "/sound/tetrisdie.wav");
    m_soundEffects.insert(SFX::ONELINE, ldCore::instance()->resourceDir() + "/sound/tetrisoneline.wav");
    m_soundEffects.insert(SFX::TWOLINE, ldCore::instance()->resourceDir() + "/sound/tetristwoline.wav");
    m_soundEffects.insert(SFX::THREELINE, ldCore::instance()->resourceDir() + "/sound/tetristhreeline.wav");
    m_soundEffects.insert(SFX::FOURLINE, ldCore::instance()->resourceDir() + "/sound/tetrisfourline.wav");
    m_soundEffects.insert(SFX::LAND, ldCore::instance()->resourceDir() + "/sound/tetristurn.wav");
    m_soundEffects.insert(SFX::TURN, ldCore::instance()->resourceDir() + "/sound/tetrisland.wav");
    m_soundEffects[SFX::LAND]->setForce(false);

    m_crashedTimer = 0;

    keyPressedLeft = false;
    keyPressedRight = false;
    keyPressedDown = false;
    keyPressedUp = false;
    keyWaitTimer = 0;

    m_scoreLabel.reset(new ldTextLabel());
    m_scoreLabel->setColor(COLOUR_WHITE);

    m_readyToStartGameTimer.setInterval(1000); // 1 sec
    connect(&m_readyToStartGameTimer, &QTimer::timeout, this, &ldTetrisVisualizer::onReadyToStartGameTimerTimeout);

    m_stateLabel.reset(new ldTextLabel());
    m_stateLabel->setColor(COLOUR_WHITE);

    init();

    connect(this, &ldTetrisVisualizer::scoreChanged, this, &ldTetrisVisualizer::updateScoreLabel); //event?

    resetMatch();
}

ldTetrisVisualizer::~ldTetrisVisualizer() {
}

bool ldTetrisVisualizer::init() {
    if (ldVisualizer::init()) {
        setPosition(ccp(1, 1));
        return true;
    }
    return false;
}

// onShouldStart
void ldTetrisVisualizer::onShouldStart() {
    QMutexLocker lock(&m_mutex);
    m_renderer->setRenderParamsQuality();
}

void ldTetrisVisualizer::onShouldStop()
{
    QMutexLocker lock(&m_mutex);
    if(m_isPlaying)
        togglePlay();
}

void ldTetrisVisualizer::moveX(double x)
{
    QMutexLocker lock(&m_mutex);

    keyPressedLeft = (x < -0.2f);
    keyPressedRight = (x > 0.2f);
}

void ldTetrisVisualizer::moveY(double y)
{
    QMutexLocker lock(&m_mutex);

    keyPressedDown = (y < -0.4f);
    keyPressedUp = (y > 0.4f);
}

void ldTetrisVisualizer::moveLeft(bool keyPress) {
    QMutexLocker lock(&m_mutex);

    keyPressedLeft = keyPress;
}

void ldTetrisVisualizer::moveRight(bool keyPress) {
    QMutexLocker lock(&m_mutex);

    keyPressedRight = keyPress;
}

void ldTetrisVisualizer::moveDown(bool keyPress) {
    QMutexLocker lock(&m_mutex);

    keyPressedDown = keyPress;
}

void ldTetrisVisualizer::moveRotate(bool keyPress) {
    QMutexLocker lock(&m_mutex);

    keyPressedUp = keyPress;
}

void ldTetrisVisualizer::togglePlay() {
    QMutexLocker lock(&m_mutex);

    m_isPlaying = !m_isPlaying;
    if(m_isPlaying) {
        startReadyToStartGameTimer();
    } else {
        m_readyToStartGameTimer.stop();
        m_stateLabel->setText("PAUSE");

        centerReadyToStartGameTimerLabel();
    }
}

void ldTetrisVisualizer::reset() {
    QMutexLocker lock(&m_mutex);

    resetMatch();
}

// draw
void ldTetrisVisualizer::draw() {
    QMutexLocker lock(&m_mutex);

    //initialisation
    if (!m_isInitialised) {
        resetMatch();
    }


    // hit wall timers
    m_bangLeftWallTimer = (m_bangLeftWallTimer > 0.0f) ? (m_bangLeftWallTimer - BANG_WALL_DECAY):0.0f;
    m_bangRightWallTimer = (m_bangRightWallTimer > 0.0f) ? (m_bangRightWallTimer - BANG_WALL_DECAY):0.0f;
    m_crashedTimer = (m_crashedTimer > 0.0f) ? (m_crashedTimer - CRASHED_EXPLODE_DECAY):0;

    //3D effect changes
    //threeDeeEffectCalculations();

    float decay_timer = 0.08f;
    m_scoredTimer = (m_scoredTimer > 0.0f) ? (m_scoredTimer - decay_timer) : 0.0f;

    m_renderer->loadIdentity3();
    m_renderer->translate3(0, 0, -2.1f);

    m_renderer->pushMatrix3();

    //3D effects
    m_renderer->rotate3X(m_wobble3dEffect);
    m_renderer->rotate3Y(m_wibble3dEffect);
    m_renderer->rotate3Z(m_rotation3dEffect);

    m_renderer->translate3(m_transX, m_transY, m_transZ);

    if(m_isPlaying && !m_readyToStartGameTimer.isActive()) {
        m_isReset = false;

        if ((keyWaitTimer == 0) && (m_crashedTimer <= 0.0f)) {
            if (keyPressedLeft) {
                MoveTetromino(LEFT);
                m_soundEffects.play(SFX::TURN);
                keyWaitTimer = KEY_WAIT;
            }
            if (keyPressedRight) {
                MoveTetromino(RIGHT);
                keyWaitTimer = KEY_WAIT;
                m_soundEffects.play(SFX::TURN);
            }
            if (keyPressedDown) {
                m_score++; //not sure if best to score here
                MoveTetromino(DOWN);
                keyWaitTimer = KEY_WAIT;
            }
            if (keyPressedUp) {
                RotateTetromino();
                keyWaitTimer = KEY_WAIT;
                m_soundEffects.play(SFX::TURN);
            }
        } else {
            keyWaitTimer--;
        }

        //Tetris Loop
        if((!m_isGameOver) || (m_crashedTimer <= 0.0f)) {
            if(++m_counter >= m_gameSpeed) {
                m_counter = 0;
                MoveTetromino(DOWN);
            } else {
                //still need to display
                MoveTetromino(REFRESH);
            }
        }
        if(m_isGameOver) {
            UpdateTetrominoAndGrid(6,0);
            m_crashedTimer = CRASHED_EXPLODE_TIME;
            m_soundEffects.play(SFX::DIE);
            resetMatch();

            emit finished();
        }
    }

    QTime time = QTime::currentTime();
    float second = time.second() + 0.001f * time.msec();
    float colourLeftWall, colourRightWall;
    colourLeftWall = -1.0f * second / 60.0f;
    colourRightWall = -1.0f * second / 60.0f;
    drawSideWalls(colourLeftWall, true);
    drawSideWalls(colourRightWall, false);

    DisplayFieldGrid(); //ToDo: doesn't display bottom row
    DisplayNextTetromino(); //redo function to no recalculate each time

    // score
    if ((m_crashedTimer > 0.0f) || m_isReset || (m_scoredTimer > 0.0f)) {
        m_scoreLabel->innerDraw(m_renderer);
    }
    // timer
    if(m_isPlaying || (!m_isPlaying && !m_isReset)) {
        m_stateLabel->innerDraw(m_renderer);
    } else {
        m_stateLabel->setText("READY");
        centerReadyToStartGameTimerLabel();
        m_stateLabel->innerDraw(m_renderer);
    }

    m_renderer->popMatrix3();
}


// drawWall
void ldTetrisVisualizer::drawSideWalls(float p_color_stepper, bool isLeftWall) {
    Vec2 p = Vec2();
    int pointsCount = 20;
    float sign = 1.0f;
    if (!isLeftWall) sign = -1.0f;
    m_renderer->begin(OL_LINESTRIP);
    for (int j = 0; j < pointsCount + 1; j++) {
        float step = 1.0f * j / pointsCount;
        if (!isLeftWall) step = 1.0f - step;
        p.y = (2.0f * step - 1.0f) * BOUNDS; //note swapped x/y from other games because of rotation in brick array
        p.x = isLeftWall ? 0.65f : -1.0f * 0.52f;
        int color = ldColorUtil::colorForStep(p_color_stepper + sign * step * 0.5f);

        if (isLeftWall) {
            if (m_bangRightWallTimer > 0.0f) {
                color = 0xFFFFFF;
            }
        } else {
            if (m_bangLeftWallTimer > 0.0f) {
                color = 0xFFFFFF;
            }
        }

        int repeat = 1;
        if(j == 0 || j == pointsCount) {
            repeat = 3;
        }
        m_renderer->vertex3(p.x, p.y, 0.0f, color, repeat);
    }
    m_renderer->end();
}

void ldTetrisVisualizer::drawFuzzyPoint(float x, float y, uint32_t pointColour, int hold, float size) {
    y = 1.1f - y; //invert, and offset
    x = x - 0.5; //offset
    m_renderer->begin(OL_LINESTRIP);
    for (int i = 0; i < hold; i++) {
        float jx = (float)(rand() % 100) / 100.0f * 2.0f - 1.0f;
        float jy = (float)(rand() % 100) / 100.0f * 2.0f - 1.0f;
        Vec2 p = Vec2();
        p.x = x + jx * size;
        p.y = y + jy * size;
        if (ldMaths::isValidLaserPoint(p)) m_renderer->vertex3(p.x, p.y, 0.0f, pointColour);
    }
    m_renderer->end();
}

void ldTetrisVisualizer::centerReadyToStartGameTimerLabel() {
    float w1 = m_stateLabel->getWidth();

    m_stateLabel->setPosition(Vec2(0.5f - (w1 / 2.0f), 0.05f));
    m_stateLabel->setColor(NOTIFICATION_COLOR);
}

void ldTetrisVisualizer::resetGame() {
    m_isReset = true;
    m_rotationDirection = !m_rotationDirection;

    m_isPlaying = false;
    m_isGameOver = false;
    m_counter = 0;
    m_BX = 7;
    m_BY = 0;
    m_linesCleared = 0;
    m_level = 1;
    m_lineScore = 100;
    m_gameSpeed = STARTING_SPEED;
    m_score = 0;
    emit scoreChanged(m_score);

    SetupDisplayGrid();

    int firstTetrominoShape = GetRandomTetronimo();
    AssignTetromino(firstTetrominoShape);
    GetOutline(THIS_TETRONIMO);

    m_nextTetromino = GetRandomTetronimo();
    AssignNextTetromino(m_nextTetromino);
    GetOutline(NEXT_TETRONIMO);
    //DisplayNextTetromino();

    MoveTetromino(LEFT);

    m_isInitialised = true;
}

void ldTetrisVisualizer::resetMatch() {
    resetGame();
}

void ldTetrisVisualizer::startReadyToStartGameTimer() {
    m_startGameTimerValue = GAME_DEFAULT_RESET_TIME;
    QMetaObject::invokeMethod(&m_readyToStartGameTimer, "start", Qt::QueuedConnection);
    updateReadyToStartGameTimerLabel();
}

void ldTetrisVisualizer::updateScoreLabel() {
    QString scoreString;

    scoreString = QString::number(m_score);
    scoreString = QString(5 - scoreString.length(), '0') + scoreString; //pad left
    m_scoreLabel->setText(scoreString);
    float w1 = m_scoreLabel->getWidth();
    m_scoreLabel->setPosition(Vec2(0.5f - (w1/2.0f), 0.9f));
}

void ldTetrisVisualizer::updateReadyToStartGameTimerLabel() {
    QString timerString;

    if(m_startGameTimerValue > 0) {
        timerString = QString::number(m_startGameTimerValue);
    }
    m_stateLabel->setText(timerString);
    centerReadyToStartGameTimerLabel();
}

void ldTetrisVisualizer::onReadyToStartGameTimerTimeout() {
    m_startGameTimerValue--;
    updateReadyToStartGameTimerLabel();
    if(m_startGameTimerValue == 0) {
        m_readyToStartGameTimer.stop();
    }
}

void ldTetrisVisualizer::threeDeeEffectCalculations()
{
    //spin faster when not playing
    if ((m_crashedTimer > 0.0) || (m_readyToStartGameTimer.isActive())) {
        m_rotationIncrement = 0.25f;
    } else {
        m_rotationIncrement = 0.03f;
    }

    if (m_rotationDirection) {
        m_rotation3dEffect += m_rotationIncrement;
    } else {
        m_rotation3dEffect -= m_rotationIncrement;
    }
    if (m_wobbleDirection) {
        m_wobble3dEffect += m_wobbleIncrement;
        if (m_wobble3dEffect >= m_wobbleUpperLimit) {
            m_wobbleDirection = false;
        }
    } else {
        m_wobble3dEffect -= m_wobbleIncrement;
        if (m_wobble3dEffect <= m_wobbleLowerLimit) {
            m_wobbleDirection = true;
        }
    }

    if (m_wibbleDirection) {
        m_wibble3dEffect += m_wibbleIncrement;
        if (m_wibble3dEffect >= m_wibbleUpperLimit) {
            m_wibbleDirection = false;
        }
    } else {
        m_wibble3dEffect -= m_wibbleIncrement;
        if (m_wibble3dEffect <= m_wibbleLowerLimit) {
            m_wibbleDirection = true;
        }
    }
}

//NEW TETRIS STUFF

void ldTetrisVisualizer::SetupDisplayGrid() {
    for(int x = 0; x < NUM_COLUMNS; x++)
       for(int y = 0; y < NUM_ROWS; y++) {
          frontGrid[x][y] = EMPTY;
          lastBackGrid[x][y] = EMPTY;
          backGrid[x][y] = EMPTY;
       }
}

void ldTetrisVisualizer::AssignTetromino(int Shape) {
    int i,j;
    int colour;
    m_thisTetromino = Shape;
    colour = GetTetrominoColor(Shape);
    for(i = 0; i < TSIZE; i++)
       for(j = 0; j < TSIZE; j++) {
            switch(Shape) {
                case SHAPE_O:
                    thisTetromino[j][i] = ShapeO[i][j]*colour;
                    break;
                case SHAPE_TI:
                    thisTetromino[j][i] = ShapeTI[i][j]*colour;
                    break;
                case SHAPE_TL:
                    thisTetromino[j][i] = ShapeTL[i][j]*colour;
                    break;
                case SHAPE_PX:
                    thisTetromino[j][i] = ShapePX[i][j]*colour;
                    break;
                case SHAPE_PC:
                    thisTetromino[j][i] = ShapePC[i][j]*colour;
                    break;
                case SHAPE_PS:
                    thisTetromino[j][i] = ShapePS[i][j]*colour;
                    break;
                case SHAPE_PZ:
                    thisTetromino[j][i] = ShapePZ[i][j]*colour;
                    break;
                case SHAPE_PO1:
                    thisTetromino[j][i] = ShapePO1[i][j]*colour;
                    break;
                case SHAPE_PO2:
                    thisTetromino[j][i] = ShapePO2[i][j]*colour;
                    break;
                case SHAPE_PI1:
                    thisTetromino[j][i] = ShapePI1[i][j]*colour;
                    break;
                case SHAPE_PI2:
                    thisTetromino[j][i] = ShapePI2[i][j]*colour;
                break;
                default:
                    break;
            }
    }
}

void ldTetrisVisualizer::AssignNextTetromino(int Shape) {
    int i,j;
    int colour;
    //m_nextTetromino = Shape;
    colour = GetTetrominoColor(Shape);
    for(i = 0; i < TSIZE; i++)
       for(j = 0; j < TSIZE; j++) {
            switch(Shape) {
                case SHAPE_O:
                    nextTetromino[j][i] = ShapeO[i][j]*colour;
                    break;
                case SHAPE_TI:
                    nextTetromino[j][i] = ShapeTI[i][j]*colour;
                    break;
                case SHAPE_TL:
                    nextTetromino[j][i] = ShapeTL[i][j]*colour;
                    break;
                case SHAPE_PX:
                    nextTetromino[j][i] = ShapePX[i][j]*colour;
                    break;
                case SHAPE_PC:
                    nextTetromino[j][i] = ShapePC[i][j]*colour;
                    break;
                case SHAPE_PS:
                    nextTetromino[j][i] = ShapePS[i][j]*colour;
                    break;
                case SHAPE_PZ:
                    nextTetromino[j][i] = ShapePZ[i][j]*colour;
                    break;
                case SHAPE_PO1:
                    nextTetromino[j][i] = ShapePO1[i][j]*colour;
                    break;
                case SHAPE_PO2:
                    nextTetromino[j][i] = ShapePO2[i][j]*colour;
                    break;
                case SHAPE_PI1:
                    nextTetromino[j][i] = ShapePI1[i][j]*colour;
                    break;
                case SHAPE_PI2:
                    nextTetromino[j][i] = ShapePI2[i][j]*colour;
                    break;
                default:
                    break;
            }
    }
}

int ldTetrisVisualizer::GetTetrominoColor(int TetrominoShape) {
    switch(TetrominoShape) {
        case SHAPE_O :
           return COLOUR_WHITE;
        case SHAPE_TI :
           return COLOUR_CYAN;
        case SHAPE_TL :
           return COLOUR_MAGENTA;
        case SHAPE_PX:
           return COLOUR_GREEN;
        case SHAPE_PC:
           return COLOUR_YELLOW;
        case SHAPE_PS:
           return COLOUR_LIGHT_BLUE;
        case SHAPE_PZ:
           return COLOUR_WHITE;
        case SHAPE_PO1 :
           return COLOUR_ORANGE;
        case SHAPE_PO2 :
           return COLOUR_BLUE;
        case SHAPE_PI1 :
           return COLOUR_YELLOW;
        case SHAPE_PI2 :
           return COLOUR_RED;
    }
    return 0;
}

int ldTetrisVisualizer::GetRandomTetronimo() {
    int Shape = rand() % 11;
    switch(Shape) {
        case 0:
           return SHAPE_O;
        case 1:
           return SHAPE_TI;
        case 2:
           return SHAPE_TL;
        case 3:
           return SHAPE_PX;
        case 4:
           return SHAPE_PC;
        case 5:
           return SHAPE_PS;
        case 6:
           return SHAPE_PZ;
        case 7:
           return SHAPE_PO1;
        case 8:
           return SHAPE_PO2;
        case 9:
           return SHAPE_PI1;
        case 10:
           return SHAPE_PI2;
    }
    return 0;
}

//ToDo: to save laser movement, maybe next Tetromino should be indicated by colour of fallen bricks
void ldTetrisVisualizer::DisplayNextTetromino() {
//    float NextTop = 0.1f; // where to display next tetronimo (top left)
//    float NextLeft = -0.40f;
//    int TempMatrix[TSIZE][TSIZE];
//    int i,j;

//    int nextColour;
//    nextColour = GetTetrominoColor(m_nextTetromino);

//    for(i = 0; i < TSIZE; i++)
//       for(j = 0; j < TSIZE; j++) {
//            switch(m_nextTetromino) {
//                case SHAPE_O:
//                    TempMatrix[j][i] = ShapeO[i][j]*nextColour;
//                    break;
//                case SHAPE_TI:
//                    TempMatrix[j][i] = ShapeTI[i][j]*nextColour;
//                    break;
//                case SHAPE_TL:
//                    TempMatrix[j][i] = ShapeTL[i][j]*nextColour;
//                    break;
//                case SHAPE_PX:
//                    TempMatrix[j][i] = ShapePX[i][j]*nextColour;
//                    break;
//                case SHAPE_PC:
//                    TempMatrix[j][i] = ShapePC[i][j]*nextColour;
//                    break;
//                case SHAPE_PS:
//                    TempMatrix[j][i] = ShapePS[i][j]*nextColour;
//                    break;
//                case SHAPE_PZ:
//                    TempMatrix[j][i] = ShapePZ[i][j]*nextColour;
//                    break;
//                case SHAPE_PO1:
//                    TempMatrix[j][i] = ShapePO1[i][j]*nextColour;
//                    break;
//                case SHAPE_PO2:
//                    TempMatrix[j][i] = ShapePO2[i][j]*nextColour;
//                    break;
//                case SHAPE_PI1:
//                    TempMatrix[j][i] = ShapePI1[i][j]*nextColour;
//                    break;
//                case SHAPE_PI2:
//                    TempMatrix[j][i] = ShapePI2[i][j]*nextColour;
//                    break;
//                default:
//                    break;
//            }
//       }

    if ((m_isPlaying) && (m_startGameTimerValue == 0)) {
        //DisplayNextTOutline(); Disabled for copyright protection
    }
}

bool ldTetrisVisualizer::MoveTetromino(int Direction) {
    switch(Direction) {
        case LEFT:
            if(CollisionDetection(LEFT)) {
                m_bangLeftWallTimer = BANG_WALL_TIME;
                m_soundEffects.play(SFX::LAND);
                return true;
            }
            UpdateTetrominoAndGrid(--m_BX, m_BY);
            break;
        case RIGHT:
            if(CollisionDetection(RIGHT)) {
                m_bangRightWallTimer = BANG_WALL_TIME;
                m_soundEffects.play(SFX::LAND);
                return true;
            }
            UpdateTetrominoAndGrid(++m_BX, m_BY);
            break;
        case DOWN:
            if(CollisionDetection(DOWN)) {
                m_soundEffects.play(SFX::LAND);
                GetNextTetromino();
                return true;
            }
            UpdateTetrominoAndGrid(m_BX, ++m_BY);
            break;
        case REFRESH:
            UpdateTetrominoAndGrid(m_BX, m_BY);
            break;
    }
    return false;
}

void ldTetrisVisualizer::UpdateTetrominoAndGrid(int x, int y) {
    int i, j;
    for( i= 0; i < NUM_ROWS; i++)
       for(j = 0; j < NUM_COLUMNS; j++)
           frontGrid[j][i] = backGrid[j][i];

    for(i = 0; i < TSIZE; i++) {
       if((x + i)<0 || (x + i) > NUM_COLUMNS)
          continue;
       for(j = 0; j < TSIZE; j++) {
           if((y + j)>NUM_ROWS)
                continue;
           if(thisTetromino[i][j] == 0)
                continue;
           frontGrid[x+i][y+j] = thisTetromino[i][j];
       }
    }
    UpdateGrid();
}

bool ldTetrisVisualizer::CollisionDetection(int Direction) {
    int x, y;
    int Bx = m_BX;
    int By = m_BY;
    switch(Direction) {
        case LEFT:
            --Bx;
            break;
        case RIGHT:
            ++Bx;
            break;
        case DOWN:
            By++;
            break;
        default:
            break;
    }

    // Left Boundry check
    if(Bx < 0) {
       for(x = 0; (x + Bx) < 0; x++)
          for(y=0; y < TSIZE; y++)
             if(thisTetromino[x][y] != 0)
                return true;
    }
    // Right Boundry check
    if(Bx > NUM_COLUMNS - TSIZE) {
       for(x = Bx + 3; x >= NUM_COLUMNS; x--)
          for(y = 0; y < TSIZE; y++)
             if(thisTetromino[x-Bx][y] != 0)
                return true;
    }
    // Bottom boundry check
    for(x = 0; x < TSIZE; x++)
          for(y = 3; y >= 0; y--) {
             if((backGrid[Bx+x][By+y] != EMPTY &&
                 thisTetromino[x][y] != EMPTY) ||
                 ((By + y) >= NUM_ROWS && thisTetromino[x][y] != EMPTY))
                return true;
    }
    return false;
}

void ldTetrisVisualizer::UpdateGrid() {
    for(int x = 0; x < NUM_COLUMNS; x++)
       for(int y = 0; y < NUM_ROWS; y++) {
          if(lastBackGrid[x][y] == frontGrid[x][y])
              continue;   //No need to draw the same image again
          lastBackGrid[x][y] = frontGrid[x][y];
       }
}


void ldTetrisVisualizer::DisplayFieldGrid() {
    GetOutline(FRONT_GRID);
    DisplayGridOutline();
}

void ldTetrisVisualizer::GetNextTetromino() {
    for(int x = 0; x < TSIZE; x++)
       for(int y = 0; y < TSIZE; y++)
          if(thisTetromino[x][y] != EMPTY)
              backGrid[m_BX + x][m_BY + y] = thisTetromino[x][y];
    CompletedLineCheck();
    AssignTetromino(m_nextTetromino);
    m_nextTetromino = GetRandomTetronimo();
    AssignNextTetromino(m_nextTetromino);
    GetOutline(NEXT_TETRONIMO);
    DisplayNextTetromino();
    m_BX = 7; //ToDo: 7??
    m_BY = 0; //ToDo: 0??
    if(MoveTetromino(LEFT)) {
       m_isGameOver = true;
    }
}

void ldTetrisVisualizer::CompletedLineCheck() {
    int Remove;
    int factor = 1;
    int x, y;
    for(y = 0; y < NUM_ROWS; y++) {
       Remove = 1;
       for(x=0; x < NUM_COLUMNS; x++) {
          if(frontGrid[x][y] == EMPTY)
             Remove = 0;
       }
       if(Remove) {
          ClearCompletedLine(y);
          m_score += m_lineScore * factor;
          emit scoreChanged(m_score);
          m_scoredTimer = 2.0;
          factor++;
       }

       switch (factor - 1) {
       case 1:
           m_soundEffects.play(SFX::ONELINE);
           break;
       case 2:
           m_soundEffects.play(SFX::TWOLINE);
           break;
       case 3:
           m_soundEffects.play(SFX::THREELINE);
           break;
       case 4:
           m_soundEffects.play(SFX::FOURLINE);
           break;
       }
    }
}

void ldTetrisVisualizer::ClearCompletedLine(int LineNumber) {
    int i, j;
    int count = 0;
    int Temp[NUM_COLUMNS];
    for(i = 0; i < NUM_COLUMNS; i++) {
       Temp[i] = frontGrid[i][LineNumber];
    }
    for(j=0; j < TSIZE + 2; j++) { //ToDo: TSIZE + 2 was '6'
        for(i = 0; i < NUM_COLUMNS; i++) {
          frontGrid[i][LineNumber] = (count ? Temp[i] : EMPTY);
        }
        count = !count;
        UpdateGrid();
    }
    for(i = LineNumber; i > 0; i--)
       for(j = 0; j<NUM_COLUMNS; j++) {
          backGrid[j][i] = backGrid[j][i-1];
       }
    for(j = 0; j < NUM_COLUMNS; j++)
       backGrid[j][0] = EMPTY;
    for(i = 0; i < NUM_COLUMNS; i++)
       for(j = 0; j < NUM_ROWS; j++)
          frontGrid[i][j] = backGrid[i][j];
    ++m_linesCleared;
    if(!(m_linesCleared % 20))
       IncreaseSpeed();
}

bool ldTetrisVisualizer::IncreaseSpeed() {
    if(m_gameSpeed - SPEED_INCREASE <= 0)
        return true;
    m_gameSpeed -= SPEED_INCREASE;  //increase the speed... lower the number, higher the speed
    m_level++;
    return false;
}

void ldTetrisVisualizer::RotateTetromino() {
    // Code to rotate the falling block
    int tempTetronimo[TSIZE][TSIZE];
    for(int i = 0; i < TSIZE; i++)
        for(int j = 0; j < TSIZE; j++)
           tempTetronimo[i][j] = thisTetromino[i][j];
    switch(m_thisTetromino) {
    case SHAPE_O:
    case SHAPE_PX:
        return;
    case SHAPE_PC:
    case SHAPE_PS:
    case SHAPE_PZ:
    case SHAPE_PO1:
    case SHAPE_PO2:
        thisTetromino[0][0] = tempTetronimo[2][0];
        thisTetromino[0][2] = tempTetronimo[0][0];
        thisTetromino[2][2] = tempTetronimo[0][2];
        thisTetromino[2][0] = tempTetronimo[2][2];

        thisTetromino[0][1] = tempTetronimo[1][0];
        thisTetromino[1][2] = tempTetronimo[0][1];
        thisTetromino[2][1] = tempTetronimo[1][2];
        thisTetromino[1][0] = tempTetronimo[2][1];
        break;

    case SHAPE_TI:
        thisTetromino[1][0] = tempTetronimo[0][1];
        thisTetromino[2][0] = tempTetronimo[0][2];
        thisTetromino[3][0] = tempTetronimo[0][3];

        thisTetromino[0][1] = tempTetronimo[1][0];
        thisTetromino[0][2] = tempTetronimo[2][0];
        thisTetromino[0][3] = tempTetronimo[3][0];
        break;

    case SHAPE_TL:
        thisTetromino[1][1] = tempTetronimo[2][1];
        thisTetromino[2][1] = tempTetronimo[2][2];
        thisTetromino[2][2] = tempTetronimo[1][2];
        thisTetromino[1][2] = tempTetronimo[1][1];
        break;

    case SHAPE_PI1:
    case SHAPE_PI2:
        // Center
        thisTetromino[1][1] = tempTetronimo[2][1];
        thisTetromino[2][1] = tempTetronimo[2][2];
        thisTetromino[2][2] = tempTetronimo[1][2];
        thisTetromino[1][2] = tempTetronimo[1][1];

        // Borders
        thisTetromino[1][0] = tempTetronimo[3][1];
        thisTetromino[2][0] = tempTetronimo[3][2];
        thisTetromino[3][0] = tempTetronimo[3][3];

        thisTetromino[3][1] = tempTetronimo[2][3];
        thisTetromino[3][2] = tempTetronimo[1][3];
        thisTetromino[3][3] = tempTetronimo[0][3];

        thisTetromino[2][3] = tempTetronimo[0][2];
        thisTetromino[1][3] = tempTetronimo[0][1];
        thisTetromino[0][3] = tempTetronimo[0][0];

        thisTetromino[0][2] = tempTetronimo[1][0];
        thisTetromino[0][1] = tempTetronimo[2][0];
        thisTetromino[0][0] = tempTetronimo[3][0];
        break;
    }
    if(CollisionDetection(REFRESH)) {
        for(int i = 0; i < TSIZE; i++)
            for(int j = 0; j < TSIZE; j++)
               thisTetromino[i][j] = tempTetronimo[i][j];
        return;
    }
    MoveTetromino(REFRESH);
}

void ldTetrisVisualizer::GetOutline(LIST_TYPE listType) {
    int maxX, maxY;

    switch(listType) {
        case (THIS_TETRONIMO):
            maxX = TSIZE;
            maxY = TSIZE;
            lineListThisTetronimo.clear();
            colourListThisTetronimo.clear();
            for(int i = 0; i <  maxX; i++) {
               for(int j = 0; j < maxY ; j++) {
                  if (thisTetromino[i][j] != 0) {
                      //* if the pixel to its left is ‘0’ or out of bounds then add a line from its bottom left to its top left
                      if (((i > 0) && (thisTetromino[i - 1][j] == 0)) || (i == 0)) {
                          //store left line:
                          lineListThisTetronimo.append(Vec2((float) i, (float) j));
                          lineListThisTetronimo.append(Vec2((float) i, (float) j + 1.0f));
                          colourListThisTetronimo.append(thisTetromino[i][j]);
                      }
                      //* if the pixel below is ‘0’ or out of bounds then add a line from its bottom right to its bottom left
                      if (((j > 0) && (thisTetromino[i][j - 1] == 0)) || (j == 0)) {
                          //store top line:
                          lineListThisTetronimo.append(Vec2((float) i + 1.0f, (float) j));
                          lineListThisTetronimo.append(Vec2((float) i, (float) j));
                          colourListThisTetronimo.append(thisTetromino[i][j]);
                      }
                      //* if the pixel to its right is ‘0’ or out of bounds then add a line from its top right to its bottom right
                      if (((i < maxX - 1) && (thisTetromino[i + 1][j] == 0)) || (i == maxX - 1)) {
                          //store right line:
                          lineListThisTetronimo.append(Vec2((float) i + 1.0f, (float) j + 1.0f));
                          lineListThisTetronimo.append(Vec2((float) i + 1.0f, (float) j));
                          colourListThisTetronimo.append(thisTetromino[i][j]);
                      }
                      //* if the pixel above is ‘0’ or out of bounds then add a line from its top left to its top right
                      if (((j < maxY - 1) && (thisTetromino[i][j + 1] == 0)) || (j == maxY - 1)) {
                          //store right line:
                          lineListThisTetronimo.append(Vec2((float) i, (float) j + 1.0f));
                          lineListThisTetronimo.append(Vec2((float) i + 1.0f, (float) j + 1.0f));
                          colourListThisTetronimo.append(thisTetromino[i][j]);
                      }
                  }
              }
           }
            break;
        case (NEXT_TETRONIMO):
        maxX = TSIZE;
        maxY = TSIZE;
        lineListNextTetronimo.clear();
        colourListNextTetronimo.clear();
        for(int i = 0; i <  maxX; i++) {
           for(int j = 0; j < maxY ; j++) {
              if (nextTetromino[i][j] != 0) {
                  //* if the pixel to its left is ‘0’ or out of bounds then add a line from its bottom left to its top left
                  if (((i > 0) && (nextTetromino[i - 1][j] == 0)) || (i == 0)) {
                      //store left line:
                      lineListNextTetronimo.append(Vec2((float) i, (float) j));
                      lineListNextTetronimo.append(Vec2((float) i, (float) j + 1.0f));
                      colourListNextTetronimo.append(nextTetromino[i][j]);
                  }
                  //* if the pixel below is ‘0’ or out of bounds then add a line from its bottom right to its bottom left
                  if (((j > 0) && (nextTetromino[i][j - 1] == 0)) || (j == 0)) {
                      //store top line:
                      lineListNextTetronimo.append(Vec2((float) i + 1.0f, (float) j));
                      lineListNextTetronimo.append(Vec2((float) i, (float) j));
                      colourListNextTetronimo.append(nextTetromino[i][j]);
                  }
                  //* if the pixel to its right is ‘0’ or out of bounds then add a line from its top right to its bottom right
                  if (((i < maxX - 1) && (nextTetromino[i + 1][j] == 0)) || (i == maxX - 1)) {
                      //store right line:
                      lineListNextTetronimo.append(Vec2((float) i + 1.0f, (float) j + 1.0f));
                      lineListNextTetronimo.append(Vec2((float) i + 1.0f, (float) j));
                      colourListNextTetronimo.append(nextTetromino[i][j]);
                  }
                  //* if the pixel above is ‘0’ or out of bounds then add a line from its top left to its top right
                  if (((j < maxY - 1) && (nextTetromino[i][j + 1] == 0)) || (j == maxY - 1)) {
                      //store right line:
                      lineListNextTetronimo.append(Vec2((float) i, (float) j + 1.0f));
                      lineListNextTetronimo.append(Vec2((float) i + 1.0f, (float) j + 1.0f));
                      colourListNextTetronimo.append(nextTetromino[i][j]);
                  }
              }
          }
       }
        break;
    case (FRONT_GRID): {
            maxX = NUM_COLUMNS;
            maxY = NUM_ROWS;
            QList<Vec2> lineListFrontGrid;

            colourListFrontGrid.clear();
            for(int i = 0; i <  maxX; i++) {
               for(int j = 0; j < maxY ; j++) {
                  if (frontGrid[i][j] != 0) {
                      //* if the pixel to its left is ‘0’ or out of bounds then add a line from its bottom left to its top left
                      if (((i > 0) && (frontGrid[i - 1][j] == 0)) || (i == 0)) {
                          //store left line:
                          lineListFrontGrid.append(Vec2((float) i, (float) j));
                          lineListFrontGrid.append(Vec2((float) i, (float) j + 1.0f));
                          colourListFrontGrid.append(frontGrid[i][j]);
                      }
                      //* if the pixel below is ‘0’ or out of bounds then add a line from its bottom right to its bottom left
                      if (((j > 0) && (frontGrid[i][j - 1] == 0)) || (j == 0)) {
                          //store top line:
                          lineListFrontGrid.append(Vec2((float) i + 1.0f, (float) j));
                          lineListFrontGrid.append(Vec2((float) i, (float) j));
                          colourListFrontGrid.append(frontGrid[i][j]);
                      }
                      //* if the pixel to its right is ‘0’ or out of bounds then add a line from its top right to its bottom right
                      if (((i < maxX - 1) && (frontGrid[i + 1][j] == 0)) || (i == maxX - 1)) {
                          //store right line:
                          lineListFrontGrid.append(Vec2((float) i + 1.0f, (float) j + 1.0f));
                          lineListFrontGrid.append(Vec2((float) i + 1.0f, (float) j));
                          colourListFrontGrid.append(frontGrid[i][j]);
                      }
                      //* if the pixel above is ‘0’ or out of bounds then add a line from its top left to its top right
                      if (((j < maxY - 1) && (frontGrid[i][j + 1] == 0)) || (j == maxY - 1)) {
                          //store right line:
                          lineListFrontGrid.append(Vec2((float) i, (float) j + 1.0f));
                          lineListFrontGrid.append(Vec2((float) i + 1.0f, (float) j + 1.0f));
                          colourListFrontGrid.append(frontGrid[i][j]);
                      }
                  }
              }
           }

            QList<QList<Vec2>> linePathParts = ldAbstractGameVisualizer::lineListToVertexShapes(lineListFrontGrid);
            QList<QList<Vec2>> laserLinePathParts = ldAbstractGameVisualizer::optimizeShapesToLaser(linePathParts, 6);
            m_lineListFrontGrid = laserLinePathParts;

            break;
    }
    }

    //Then compare all lines with each other and if the start and end of one line is equal to the end and start of another, they are the same line and one can be deleted

    /*

    // Remove all odd numbers from a QList<int>
    QMutableListIterator<Vec2> tetItemsOuter(tetronimoLineList);
    int checkOddOuter = 0;
    while (tetItemsOuter.hasNext()) {
        tetItemsOuter.next();
        if (checkOddOuter % 2 == 1) {//lines are point pairs - want to compare end points of outer to start points of inner (odd = end points)
            QMutableListIterator<Vec2> tetItemsInner(tetronimoLineList);
            int checkEvenInner = 0;
            while (tetItemsInner.hasNext()) {
                tetItemsInner.next();
                if (checkEvenInner % 2 == 0) {//lines are point pairs - want to compare end points of outer to start points of inner (even = start points)

                    //if end point is same as another start point
                    if ((tetItemsInner.value().x == tetItemsOuter.value().x) && (tetItemsInner.value().y == tetItemsOuter.value().y)) {
                        //don't count bends (i.e. start/end match, but is 90 degrees - so all x must me the same, or all y must be the same
                        if (((tetItemsInner.peekNext().x == tetItemsInner.value().x) && (tetItemsInner.value().x == tetItemsOuter.value().x) && (tetItemsOuter.value().x == tetItemsOuter.peekPrevious().x))
                                || ((tetItemsInner.peekNext().y == tetItemsInner.value().y) && (tetItemsInner.value().y == tetItemsOuter.value().y) && (tetItemsOuter.value().y == tetItemsOuter.peekPrevious().y))) {

                            //make 1st end point same as 2nd end point and delete other pair:
                            //(0, 0) (0, 1) (0, 1) (0, 2) => (0, 0) (0, 2)

                        }

                    }
                }

                //if (tetItemsOuter.next().x == tetItemsInner.next().x) {
                //    tetItemsOuter.remove();
                //}
            }
            checkEvenInner += 1;
        }
        checkOddOuter += 1;
    }
    */
}

void ldTetrisVisualizer::DisplayGridOutline() {
    m_renderer->pushMatrix3();
    m_renderer->translate3(-0.5f, 1.0f, 0.0f);
    m_renderer->pushMatrix3();
    m_renderer->scale3(0.075f, 0.1f, 1.0f); //tetronimoLineList currently is full units - ascpect ratio means have to further adjust x
    m_renderer->rotate3X(M_PIf); //originally upside-down

    int lineColour;
    int colourCounter = 0;
    for (const QList<Vec2> &shape : m_lineListFrontGrid) {
        m_renderer->begin(OL_LINESTRIP);
        lineColour = colourListFrontGrid.value(colourCounter);
        colourCounter += 1; //not pairs, so half as quick
        for (const Vec2 &point : shape) {
            m_renderer->vertex3(point.x, point.y, 0.0f, lineColour);
        }
        m_renderer->end();
   }



    m_renderer->popMatrix3();
    m_renderer->popMatrix3();
}

void ldTetrisVisualizer::DisplayNextTOutline() {
    m_renderer->pushMatrix3();
    m_renderer->translate3(-0.8f, 1.0f, 0.0f);
    m_renderer->pushMatrix3();
    m_renderer->scale3(0.075f, 0.1f, 1.0f); //tetronimoLineList currently is full units - ascpect ratio means have to further adjust x
    m_renderer->rotate3X(M_PIf); //originally upside-down

    int pointPairs = 0; //these point pairs, i.e. lines so do not want to draw from, say, point 1 to point 2 (0 based)
    Vec2 startPoint, endPoint;
    int lineColour = COLOUR_BLACK;
    int colourCounter = 0;
    m_renderer->begin(OL_LINESTRIP);
    foreach (Vec2 point, lineListNextTetronimo) {
        if (pointPairs % 2 == 0) {
            startPoint = point;
            lineColour = colourListNextTetronimo.value(colourCounter);
            colourCounter += 1; //not pairs, so half as quick

        } else {
            endPoint = point;
            m_renderer->vertex3(startPoint.x, startPoint.y, 0.0f, lineColour);
            m_renderer->vertex3(endPoint.x, endPoint.y, 0.0f, lineColour);
        }
        pointPairs += 1;
    }
    m_renderer->end();
    m_renderer->popMatrix3();
    m_renderer->popMatrix3();
}
