//
//  ldTetrisVisualizer.h
//  LaserdockVisualizer
//
//  Created by MEO 30/03/2017
//  Copyright (c) 2017 Wicked Lasers. All rights reserved.
//

#ifndef __LaserdockVisualizer__ldTetrisVisualizer__
#define __LaserdockVisualizer__ldTetrisVisualizer__

#include <QtCore/QTimer>

#include "ldCore/Helpers/Maths/ldMaths.h"
#include "ldCore/Helpers/Color/ldColorUtil.h"
#include "ldCore/Helpers/Sound/ldQSound.h"

#include "ldCore/Visualizations/Visualizers/Games/ldAbstractGameVisualizer.h"

class ldTextLabel;

class ldTetrisVisualizer : public ldAbstractGameVisualizer
{
    Q_OBJECT
public:
    explicit ldTetrisVisualizer();
    virtual ~ldTetrisVisualizer();

    bool init();

    // ldVisualizer
    
    virtual QString visualizerName() const override { return "Lasertris A"; }
    virtual void onShouldStart() override;
    virtual void onShouldStop() override;

    void threeDeeEffectCalculations();

public slots:
    // ldAbstractGameVisualizer
    virtual void reset() override;
    virtual void togglePlay()  override;

    void moveX(double x) override;
    void moveY(double y) override;

    void moveLeft(bool keyPress);
    void moveRight(bool keyPress);
    void moveDown(bool keyPress);
    void moveRotate(bool keyPress);

signals:
    void scoreChanged(const int &score);

protected:
    virtual void draw() override;
    OLRenderParams m_params;

private slots:
    void onReadyToStartGameTimerTimeout();

private:
    enum SFX{
        DIE,
        ONELINE,
        TWOLINE,
        THREELINE,
        FOURLINE,
        LAND,
        TURN
    };

    bool keyPressedLeft = false;
    bool keyPressedRight = false;
    bool keyPressedUp = false;
    bool keyPressedDown = false;
    int keyWaitTimer;
    static const int KEY_WAIT = 2;

    constexpr static const float AVOIDED_FLASH_TIME = 0.75;
    constexpr static const float AVOIDED_FLASH_DECAY = 0.08f;
    constexpr static const float CRASHED_EXPLODE_TIME = 2.0f;
    constexpr static const float CRASHED_EXPLODE_DECAY = 0.17f;
    constexpr static const float BANG_WALL_TIME = 2.0f;
    constexpr static const float BANG_WALL_DECAY = 0.17f;

    void centerReadyToStartGameTimerLabel();
    void drawFuzzyPoint(float x, float y, uint32_t c = C_WHITE, int hold = 16, float size = 0.01);
    void resetGame();
    void resetMatch();
    void startReadyToStartGameTimer();
    void updateScoreLabel();
    void updateReadyToStartGameTimerLabel();
    float m_bangLeftWallTimer;
    float m_bangRightWallTimer;

    static const uint32_t COLOUR_WHITE = 0xFFFFFF;
    static const uint32_t COLOUR_RED = 0xFF0000;
    static const uint32_t COLOUR_BLACK = 0x000000;
    static const uint32_t COLOUR_BLUE = 0x0000FF;
    static const uint32_t COLOUR_GREEN = 0x00FF00;
    static const uint32_t COLOUR_YELLOW = 0xFFFF00;
    static const uint32_t COLOUR_ORANGE = 0xFF8100;
    static const uint32_t COLOUR_MAGENTA = 0xFF00FF;
    static const uint32_t COLOUR_CYAN = 0x00FFFF;
    static const uint32_t COLOUR_DARK_BLUE = 0x000066;
    static const uint32_t COLOUR_LIGHT_BLUE = 0x4444FF;

    static const int NOTIFICATION_COLOR = COLOUR_GREEN;

    //not score -, but display points
    int POINT_COMPLEXITY = 8;
    float POINT_SIZE = 0.0125f;

    float m_crashedTimer;

    int m_score;
    QScopedPointer<ldTextLabel> m_scoreLabel;

    QTimer m_readyToStartGameTimer;
    QScopedPointer<ldTextLabel> m_stateLabel;

    float m_wobble3dEffect = 0.75f; //rotation around X axis
    bool m_wobbleDirection = true;
    float m_wobbleUpperLimit = 1.00f;
    float m_wobbleLowerLimit = 0.50f;
    float m_wobbleIncrement = 0.005f;

    float m_wibble3dEffect = 0.00f; //rotation around Y axis
    bool m_wibbleDirection = true;
    float m_wibbleUpperLimit = 0.1f;
    float m_wibbleLowerLimit = -0.1f;
    float m_wibbleIncrement = 0.01f;

    float m_rotation3dEffect = 0.0f; //rotation around Z axis
    float m_rotationIncrement = 0.03f;
    bool m_rotationDirection = true;
    float m_transX = 0.0f;
    float m_transY = 0.0f;
    float m_transZ = 0.0f;

    //NEW TETRIS VARS - refactor names when complete
    static const int STARTING_SPEED = 11;
    static const int SPEED_INCREASE = 1;
    static const int EMPTY = 0; //empty square?
    static const int NUM_ROWS = 20; //screen rows (therefore each row takes up 0.1 (from 1.0 to -1.0)
    static const int NUM_COLUMNS = 15; //screen columns (each col should take up 0.1 (from -0.75 to 0.75)
    static const int REFRESH = 0;
    bool m_isInitialised = false;
    bool m_isGameOver = false;
    int m_counter;
    int m_linesCleared;
    int m_level;
    int m_lineScore;
    int m_gameSpeed;

    static const int TSIZE = 4;
    int thisTetromino[TSIZE][TSIZE];      // Stores the shape of the current falling block
    int nextTetromino[TSIZE][TSIZE];      // Stores the shape of the current falling block
    int frontGrid[NUM_COLUMNS][NUM_ROWS];	// Will contain the layout with the falling block
    int backGrid[NUM_COLUMNS][NUM_ROWS]; //will contain the layout without the falling block
    int lastBackGrid[NUM_COLUMNS][NUM_ROWS];	// Will contain the layout of the previous screen

    //Shapes of the blocks
    int ShapeO[4][4] = {
        {0,0,0,0},
        {0,1,0,0},
        {0,0,0,0},
        {0,0,0,0}
    };

    int ShapeTI[4][4] = {
        {1,1,1,0},
        {0,0,0,0},
        {0,0,0,0},
        {0,0,0,0}
    };

    int ShapeTL[4][4] = {
        {0,0,0,0},
        {0,1,1,0},
        {0,1,0,0},
        {0,0,0,0}
    };

    int ShapePX[4][4] = {
        {0,1,0,0},
        {1,1,1,0},
        {0,1,0,0},
        {0,0,0,0}
    };
    int ShapePC[4][4] = {
        {1,0,1,0},
        {1,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    };
    int ShapePS[4][4] = {
        {1,0,0,0},
        {1,1,1,0},
        {0,0,1,0},
        {0,0,0,0}
    };
    int ShapePZ[4][4] = {
        {0,0,1,0},
        {1,1,1,0},
        {1,0,0,0},
        {0,0,0,0}
    };
    int ShapePO1[4][4] = {
        {1,1,1,0},
        {1,1,0,0},
        {0,0,0,0},
        {0,0,0,0}
    };
    int ShapePO2[4][4] = {
        {1,1,1,0},
        {1,1,0,0},
        {0,0,0,0},
        {0,0,0,0}
    };
    int ShapePI1[4][4] = {
        {1,1,1,1},
        {0,1,0,0},
        {0,0,0,0},
        {0,0,0,0}
    };
    int ShapePI2[4][4] = {
        {1,1,1,1},
        {0,0,1,0},
        {0,0,0,0},
        {0,0,0,0}
    };

    QList<Vec2> lineListNextTetronimo;
    QList<int> colourListNextTetronimo;
    QList<Vec2> lineListThisTetronimo; //probably won't use in the end
    QList<int> colourListThisTetronimo; //probably won't use in the end
    QList<QList<Vec2>> m_lineListFrontGrid;
    QList<int> colourListFrontGrid;

    enum LIST_TYPE{
        NEXT_TETRONIMO, THIS_TETRONIMO, FRONT_GRID
    };

    // Shapes - change to ENUM
    static const int SHAPE_O = 100;
    static const int SHAPE_TI = 101;
    static const int SHAPE_TL = 102;
    static const int SHAPE_PX = 103;
    static const int SHAPE_PC = 104;
    static const int SHAPE_PS = 105;
    static const int SHAPE_PZ = 106;
    static const int SHAPE_PO1 = 107;
    static const int SHAPE_PO2 = 108;
    static const int SHAPE_PI1 = 109;
    static const int SHAPE_PI2 = 110;

    // Directions - rip out and replace
    static const int RIGHT = 201;
    static const int DOWN = 203;
    static const int TOP = 30;
    static const int LEFT = 160;

    float TTTOP = 0.17647f; //30; //=0.1 * 30/340
    float LLLEFT = 0.02963f; //160; = 0.1 * 160/540

    static const int KEY_UP = 72;
    static const int KEY_DOWN = 80;
    static const int KEY_LEFT = 75;
    static const int KEY_RIGHT = 77;
    static const int KEY_ESC = 27;
    static const int KEY_ENTER = 13;
    static const int KEY_SPACE = 32;

    int m_thisTetromino;
    int m_nextTetromino;

    int m_BX;
    int m_BY;

    float m_scoredTimer;

    const float BOUNDS = 0.99f; //Top, bottom, left and right

    //NEW TETRIS FUNCS/METHODS - refactor names when complete
    void SetupDisplayGrid();
    void AssignTetromino(int Shape); // Gives the shape to the current falling block
    void AssignNextTetromino(int Shape); // Gives the shape to the current falling block
    int  GetTetrominoColor(int TetrominoShape); //change this so shapes have fixed colours?
    int  GetRandomTetronimo();
    void GetNextTetromino();
    bool MoveTetromino(int Direction);  //Moves the falling block in the direction
    void UpdateTetrominoAndGrid(int x, int y);
    void UpdateGrid();
    bool CollisionDetection(int Direction);	// returns true when brick collides with something
    void CompletedLineCheck();
    void ClearCompletedLine(int Number);
    bool IncreaseSpeed();
    void RotateTetromino();
    void DisplayFieldGrid();
    void DisplayNextTetromino();
    void GetOutline(LIST_TYPE listType);
    void DisplayGridOutline();
    void DisplayNextTOutline();
    void drawSideWalls(float p_color_stepper, bool isLeftWall);
};

#endif /*__LaserdockVisualizer__ldTetrisVisualizer__*/
