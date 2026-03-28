/*
 * 文件说明：
 * 1. 声明 HIS 系统对外暴露的核心接口；
 * 2. 对外隐藏 HIS 结构体内部实现细节；
 * 3. 供 main.c 和其它实现文件包含与调用。
 */
#ifndef HIS_H
#define HIS_H

typedef struct HIS HIS;

HIS *createHIS(void);
void destroyHIS(HIS *his);
void buildCourseDataset(HIS *his);
int loadDataFromDefaultFiles(HIS *his);
void showMainMenu(void);
int readInt(const char *prompt);
void departmentMenuLoop(HIS *his);
void doctorMenuLoop(HIS *his);
void patientMenuLoop(HIS *his);
void medicineMenuLoop(HIS *his);
void businessMenuLoop(HIS *his);
void queryMenuLoop(HIS *his);
void exportAllData(HIS *his);

#endif
