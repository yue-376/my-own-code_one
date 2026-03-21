#ifndef HIS_H
#define HIS_H

typedef struct HIS HIS;

HIS *createHIS(void);
void destroyHIS(HIS *his);
void buildCourseDataset(HIS *his);
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
