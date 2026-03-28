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

    if (loadDataFromDefaultFiles(his)) {
        printf("系统已初始化：已从 patients.txt、records.txt、prescriptions.txt、medicines.txt 读取数据。\n");
    } else {
        printf("系统初始化警告：默认数据文件读取失败，当前为未加载数据状态。\n");
    }

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
                if (loadDataFromDefaultFiles(his)) {
                    printf("已从默认文件重新加载数据。\n");
                } else {
                    printf("重新加载失败：请检查 patients.txt、records.txt、prescriptions.txt、medicines.txt。\n");
                }
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
