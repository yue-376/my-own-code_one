/*
 * 文件说明：
 * 1. 作为程序主入口，负责创建和销毁 HIS 系统对象；
 * 2. 负责驱动顶层主菜单和整体业务流程分发；
 * 3. 不承载具体业务逻辑，只负责调用 his.h 中声明的接口。
 */
#include "his.h"
#include <stdio.h>

int main(void) {
    HIS *his = createHIS();
    int choice;

    if (his == NULL) {
        printf("系统初始化失败：内存不足。\n");
        return 1;
    }

    buildCourseDataset(his);
    printf("系统已初始化：当前已生成 5 个科室、20 名医生、120 名患者、35 名住院患者、30 种药品。\n");

    while (1) {
        showMainMenu();
        choice = readInt("请输入菜单编号: ");
        switch (choice) {
            case 1:
                departmentMenuLoop(his);
                break;
            case 2:
                doctorMenuLoop(his);
                break;
            case 3:
                patientMenuLoop(his);
                break;
            case 4:
                medicineMenuLoop(his);
                break;
            case 5:
                businessMenuLoop(his);
                break;
            case 6:
                queryMenuLoop(his);
                break;
            case 7:
                exportAllData(his);
                break;
            case 8:
                buildCourseDataset(his);
                printf("已重新生成课程设计样例数据。\n");
                break;
            case 0:
                destroyHIS(his);
                printf("感谢使用，系统已退出。\n");
                return 0;
            default:
                printf("菜单编号无效，请重新输入。\n");
        }
    }
}
