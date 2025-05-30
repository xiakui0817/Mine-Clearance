#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define ROW 9
#define COL 9
#define MINES 10

char mine[ROW][COL];   // 雷的分布
char show[ROW][COL];   // 显示给玩家的棋盘
int mines_left;        // 剩余雷数
int unopened;          // 未翻开格子数
time_t start_time;     // 游戏开始时间

// 初始化棋盘
void initBoard() {
    // 初始化两个棋盘
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            mine[i][j] = '0';
            show[i][j] = '*';
        }
    }
    
    // 随机布置雷
    srand((unsigned int)time(NULL));
    int count = 0;
    while (count < MINES) {
        int x = rand() % ROW;
        int y = rand() % COL;
        if (mine[x][y] == '0') {
            mine[x][y] = '1';
            count++;
        }
    }
    
    mines_left = MINES;
    unopened = ROW * COL;
    start_time = time(NULL);
}

// 显示游戏界面
void displayBoard() {
    system("cls"); // 清屏
    
    // 打印计时和剩余雷数
    int elapsed = (int)(time(NULL) - start_time);
    printf("时间: %02d:%02d    剩余雷数: %d\n", elapsed / 60, elapsed % 60, mines_left);
    
    // 打印列号
    printf("   ");
    for (int j = 0; j < COL; j++) {
        printf("%d ", j + 1);
    }
    printf("\n  ┌");
    for (int j = 0; j < COL; j++) {
        printf("─");
        if (j < COL - 1) printf("┬");
    }
    printf("┐\n");
    
    // 打印棋盘内容
    for (int i = 0; i < ROW; i++) {
        printf("%d │", i + 1);
        for (int j = 0; j < COL; j++) {
            if (show[i][j] == '0') {
                printf("  "); // 0显示为空
            } else {
                printf("%c ", show[i][j]);
            }
            if (j < COL - 1) printf("│");
        }
        printf("│\n");
        
        if (i < ROW - 1) {
            printf("  ├");
            for (int j = 0; j < COL; j++) {
                printf("─");
                if (j < COL - 1) printf("┼");
            }
            printf("┤\n");
        }
    }
    
    printf("  └");
    for (int j = 0; j < COL; j++) {
        printf("─");
        if (j < COL - 1) printf("┴");
    }
    printf("┘\n");
}

// 计算指定位置周围雷的数量
int countMines(int x, int y) {
    int count = 0;
    for (int i = x - 1; i <= x + 1; i++) {
        for (int j = y - 1; j <= y + 1; j++) {
            if (i >= 0 && i < ROW && j >= 0 && j < COL) {
                if (mine[i][j] == '1') {
                    count++;
                }
            }
        }
    }
    return count;
}

// 递归展开空白区域
void expand(int x, int y) {
    if (x < 0 || x >= ROW || y < 0 || y >= COL || show[x][y] != '*') {
        return;
    }
    
    int count = countMines(x, y);
    if (count > 0) {
        show[x][y] = '0' + count;
        unopened--;
        return;
    }
    
    // 周围没有雷，展开周围区域
    show[x][y] = '0';
    unopened--;
    
    // 递归展开相邻8个方向
    for (int i = x - 1; i <= x + 1; i++) {
        for (int j = y - 1; j <= y + 1; j++) {
            if (i == x && j == y) continue;
            expand(i, j);
        }
    }
}

// 玩家操作
void playerMove() {
    int x, y;
    char action;
    
    while (1) {
        printf("请输入操作(f:翻开, m:标记/取消标记), 行和列(示例: f 3 4): ");
        scanf(" %c %d %d", &action, &x, &y);
        
        // 转换为数组索引
        x--;
        y--;
        
        if (x >= 0 && x < ROW && y >= 0 && y < COL) {
            if (action == 'f' || action == 'F') {
                // 翻开操作
                if (show[x][y] == '*' || show[x][y] == '!') {
                    if (mine[x][y] == '1') {
                        // 踩到雷，游戏结束
                        show[x][y] = 'X'; // 显示踩中的雷
                        return;
                    }
                    expand(x, y);
                    return;
                } else {
                    printf("该位置已经翻开!\n");
                }
            } else if (action == 'm' || action == 'M') {
                // 标记/取消标记操作
                if (show[x][y] == '*') {
                    show[x][y] = '!';
                    mines_left--;
                } else if (show[x][y] == '!') {
                    show[x][y] = '*';
                    mines_left++;
                } else {
                    printf("无法标记已翻开的位置!\n");
                }
                return;
            } else {
                printf("无效操作!\n");
            }
        } else {
            printf("无效坐标! 请使用1-%d行, 1-%d列\n", ROW, COL);
        }
    }
}

// 检查游戏是否胜利
int checkWin() {
    // 所有非雷格子都被翻开
    return unopened == MINES;
}

// 游戏结束时显示所有雷的位置
void revealMines() {
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            if (mine[i][j] == '1' && show[i][j] != 'X') {
                show[i][j] = 'M'; // 显示未踩中的雷
            }
        }
    }
}

int main() {
    initBoard();
    
    while (1) {
        displayBoard();
        playerMove();
        
        // 检查游戏结束条件
        if (show[x][y] == 'X') { // 踩到雷
            revealMines();
            displayBoard();
            printf("游戏结束! 你踩到雷了!\n");
            break;
        }
        
        if (checkWin()) {
            revealMines();
            displayBoard();
            int elapsed = (int)(time(NULL) - start_time);
            printf("恭喜你赢了! 用时: %02d:%02d\n", elapsed / 60, elapsed % 60);
            break;
        }
    }
    
    return 0;
}
