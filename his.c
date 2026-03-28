/*
 * 文件说明：
 * 1. 实现 HIS 的核心数据结构、链表操作、业务规则和数据校验；
 * 2. 实现科室、医生、患者、药品、处方、病房等实体的增删改查；
 * 3. 实现数据导入导出、统计报表、床位利用率和子菜单逻辑。
 */
#include <ctype.h>
#include "his.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME_LEN 64
#define ID_LEN 32
#define NOTE_LEN 128
#define TITLE_LEN 32
#define TYPE_LEN 32
#define INPUT_LEN 256

/*
 * 轻量级医院信息系统（HIS）课程设计增强版
 * 说明：
 * 1. 全程使用链表组织核心业务数据；
 * 2. 支持患者、医生、科室、病房、药品、医疗记录、处方等实体；
 * 3. 提供新增患者、挂号、住院、开药、查询、统计、导出 txt 数据等功能；
 * 4. 默认生成满足课程设计规模要求的一组样例数据，便于现场演示。
 */

typedef struct Department {
    int id;
    char name[NAME_LEN];
    struct Department *next;
} Department;

typedef struct Doctor {
    int id;
    char name[NAME_LEN];
    int departmentId;
    char title[TITLE_LEN];
    struct Doctor *next;
} Doctor;

typedef struct Patient {
    int id;
    char name[NAME_LEN];
    int age;
    char gender[16];
    char phone[ID_LEN];
    int registeredDoctorId;
    int admittedWardId;
    int admittedBedNo;
    struct Patient *next;
} Patient;

typedef struct Ward {
    int id;
    char name[NAME_LEN];
    int departmentId;
    int totalBeds;
    int usedBeds;
    struct Ward *next;
} Ward;

typedef struct Medicine {
    int id;
    char genericName[NAME_LEN];
    char brandName[NAME_LEN];
    int stock;
    double unitPrice;
    struct Medicine *next;
} Medicine;

typedef struct Record {
    int id;
    int patientId;
    int doctorId;
    char type[TYPE_LEN];
    char note[NOTE_LEN];
    struct Record *next;
} Record;

typedef struct Prescription {
    int id;
    int recordId;
    int patientId;
    int doctorId;
    int medicineId;
    int quantity;
    struct Prescription *next;
} Prescription;

struct HIS {
    Department *departments;
    Doctor *doctors;
    Patient *patients;
    Ward *wards;
    Medicine *medicines;
    Record *records;
    Prescription *prescriptions;
    int nextDepartmentId;
    int nextDoctorId;
    int nextPatientId;
    int nextWardId;
    int nextMedicineId;
    int nextRecordId;
    int nextPrescriptionId;
};

void initHIS(HIS *his);

Ward *findWardById(HIS *his, int id);
Medicine *findMedicineById(HIS *his, int id);

void trimNewline(char *text) {
    size_t len = strlen(text);
    if (len > 0 && text[len - 1] == '\n') {
        text[len - 1] = '\0';
    }
}

void readLine(const char *prompt, char *buffer, size_t size) {
    printf("%s", prompt);
    if (fgets(buffer, (int)size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    trimNewline(buffer);
}

int readInt(const char *prompt) {
    char buffer[INPUT_LEN];
    int value;

    while (1) {
        readLine(prompt, buffer, sizeof(buffer));
        if (sscanf(buffer, "%d", &value) == 1) {
            return value;
        }
        printf("输入无效，请输入整数。\n");
    }
}

int isBlankString(const char *text) {
    while (*text != '\0') {
        if (!isspace((unsigned char)*text)) {
            return 0;
        }
        text++;
    }
    return 1;
}

int isValidGender(const char *gender) {
    return strcmp(gender, "男") == 0 || strcmp(gender, "女") == 0;
}

int isDigitsOnly(const char *text) {
    while (*text != '\0') {
        if (!isdigit((unsigned char)*text)) {
            return 0;
        }
        text++;
    }
    return 1;
}

int isValidPhone(const char *phone) {
    size_t len = strlen(phone);
    return len == 11 && isDigitsOnly(phone);
}

int isValidPatientName(const char *name) {
    size_t len = strlen(name);
    return !isBlankString(name) && len >= 2 && len < NAME_LEN;
}

int isValidAge(int age) {
    return age >= 0 && age <= 120;
}

HIS *createHIS(void) {
    HIS *his = (HIS *)malloc(sizeof(HIS));
    if (his == NULL) {
        return NULL;
    }
    initHIS(his);
    return his;
}

void initHIS(HIS *his) {
    his->departments = NULL;
    his->doctors = NULL;
    his->patients = NULL;
    his->wards = NULL;
    his->medicines = NULL;
    his->records = NULL;
    his->prescriptions = NULL;
    his->nextDepartmentId = 1;
    his->nextDoctorId = 1;
    his->nextPatientId = 1;
    his->nextWardId = 1;
    his->nextMedicineId = 1;
    his->nextRecordId = 1;
    his->nextPrescriptionId = 1;
}

int nextAvailableDepartmentId(HIS *his) {
    int candidate = 1;
    while (1) {
        Department *current = his->departments;
        int used = 0;
        while (current != NULL) {
            if (current->id == candidate) {
                used = 1;
                break;
            }
            current = current->next;
        }
        if (!used) {
            return candidate;
        }
        candidate++;
    }
}

int nextAvailableDoctorId(HIS *his) {
    int candidate = 1;
    while (1) {
        Doctor *current = his->doctors;
        int used = 0;
        while (current != NULL) {
            if (current->id == candidate) {
                used = 1;
                break;
            }
            current = current->next;
        }
        if (!used) {
            return candidate;
        }
        candidate++;
    }
}

int nextAvailablePatientId(HIS *his) {
    int candidate = 1;
    while (1) {
        Patient *current = his->patients;
        int used = 0;
        while (current != NULL) {
            if (current->id == candidate) {
                used = 1;
                break;
            }
            current = current->next;
        }
        if (!used) {
            return candidate;
        }
        candidate++;
    }
}

int nextAvailableWardId(HIS *his) {
    int candidate = 1;
    while (1) {
        Ward *current = his->wards;
        int used = 0;
        while (current != NULL) {
            if (current->id == candidate) {
                used = 1;
                break;
            }
            current = current->next;
        }
        if (!used) {
            return candidate;
        }
        candidate++;
    }
}

int nextAvailableMedicineId(HIS *his) {
    int candidate = 1;
    while (1) {
        Medicine *current = his->medicines;
        int used = 0;
        while (current != NULL) {
            if (current->id == candidate) {
                used = 1;
                break;
            }
            current = current->next;
        }
        if (!used) {
            return candidate;
        }
        candidate++;
    }
}

int nextAvailableRecordId(HIS *his) {
    int candidate = 1;
    while (1) {
        Record *current = his->records;
        int used = 0;
        while (current != NULL) {
            if (current->id == candidate) {
                used = 1;
                break;
            }
            current = current->next;
        }
        if (!used) {
            return candidate;
        }
        candidate++;
    }
}

int nextAvailablePrescriptionId(HIS *his) {
    int candidate = 1;
    while (1) {
        Prescription *current = his->prescriptions;
        int used = 0;
        while (current != NULL) {
            if (current->id == candidate) {
                used = 1;
                break;
            }
            current = current->next;
        }
        if (!used) {
            return candidate;
        }
        candidate++;
    }
}

Department *appendDepartment(HIS *his, const char *name) {
    Department *node = (Department *)malloc(sizeof(Department));
    Department *tail;
    if (node == NULL) {
        return NULL;
    }

    node->id = nextAvailableDepartmentId(his);
    strncpy(node->name, name, NAME_LEN - 1);
    node->name[NAME_LEN - 1] = '\0';
    node->next = NULL;

    if (his->departments == NULL) {
        his->departments = node;
    } else {
        tail = his->departments;
        while (tail->next != NULL) {
            tail = tail->next;
        }
        tail->next = node;
    }
    return node;
}

Doctor *appendDoctor(HIS *his, const char *name, int departmentId, const char *title) {
    Doctor *node = (Doctor *)malloc(sizeof(Doctor));
    Doctor *tail;
    if (node == NULL) {
        return NULL;
    }

    node->id = nextAvailableDoctorId(his);
    node->departmentId = departmentId;
    strncpy(node->name, name, NAME_LEN - 1);
    node->name[NAME_LEN - 1] = '\0';
    strncpy(node->title, title, TITLE_LEN - 1);
    node->title[TITLE_LEN - 1] = '\0';
    node->next = NULL;

    if (his->doctors == NULL) {
        his->doctors = node;
    } else {
        tail = his->doctors;
        while (tail->next != NULL) {
            tail = tail->next;
        }
        tail->next = node;
    }
    return node;
}

Ward *appendWard(HIS *his, const char *name, int departmentId, int totalBeds) {
    Ward *node = (Ward *)malloc(sizeof(Ward));
    Ward *tail;
    if (node == NULL) {
        return NULL;
    }

    node->id = nextAvailableWardId(his);
    node->departmentId = departmentId;
    node->totalBeds = totalBeds;
    node->usedBeds = 0;
    strncpy(node->name, name, NAME_LEN - 1);
    node->name[NAME_LEN - 1] = '\0';
    node->next = NULL;

    if (his->wards == NULL) {
        his->wards = node;
    } else {
        tail = his->wards;
        while (tail->next != NULL) {
            tail = tail->next;
        }
        tail->next = node;
    }
    return node;
}

Medicine *appendMedicine(HIS *his, const char *genericName, const char *brandName, int stock, double unitPrice) {
    Medicine *node = (Medicine *)malloc(sizeof(Medicine));
    Medicine *tail;
    if (node == NULL) {
        return NULL;
    }

    node->id = nextAvailableMedicineId(his);
    node->stock = stock;
    node->unitPrice = unitPrice;
    strncpy(node->genericName, genericName, NAME_LEN - 1);
    node->genericName[NAME_LEN - 1] = '\0';
    strncpy(node->brandName, brandName, NAME_LEN - 1);
    node->brandName[NAME_LEN - 1] = '\0';
    node->next = NULL;

    if (his->medicines == NULL) {
        his->medicines = node;
    } else {
        tail = his->medicines;
        while (tail->next != NULL) {
            tail = tail->next;
        }
        tail->next = node;
    }
    return node;
}

Patient *appendPatient(HIS *his, const char *name, int age, const char *gender, const char *phone) {
    Patient *node = (Patient *)malloc(sizeof(Patient));
    Patient *tail;
    if (!isValidPatientName(name) || !isValidAge(age) || !isValidGender(gender) || !isValidPhone(phone)) {
        return NULL;
    }
    if (node == NULL) {
        return NULL;
    }

    node->id = nextAvailablePatientId(his);
    node->age = age;
    node->registeredDoctorId = 0;
    node->admittedWardId = 0;
    node->admittedBedNo = 0;
    strncpy(node->name, name, NAME_LEN - 1);
    node->name[NAME_LEN - 1] = '\0';
    strncpy(node->gender, gender, sizeof(node->gender) - 1);
    node->gender[sizeof(node->gender) - 1] = '\0';
    strncpy(node->phone, phone, ID_LEN - 1);
    node->phone[ID_LEN - 1] = '\0';
    node->next = NULL;

    if (his->patients == NULL) {
        his->patients = node;
    } else {
        tail = his->patients;
        while (tail->next != NULL) {
            tail = tail->next;
        }
        tail->next = node;
    }
    return node;
}

Record *appendRecord(HIS *his, int patientId, int doctorId, const char *type, const char *note) {
    Record *node = (Record *)malloc(sizeof(Record));
    Record *tail;
    if (node == NULL) {
        return NULL;
    }

    node->id = nextAvailableRecordId(his);
    node->patientId = patientId;
    node->doctorId = doctorId;
    strncpy(node->type, type, TYPE_LEN - 1);
    node->type[TYPE_LEN - 1] = '\0';
    strncpy(node->note, note, NOTE_LEN - 1);
    node->note[NOTE_LEN - 1] = '\0';
    node->next = NULL;

    if (his->records == NULL) {
        his->records = node;
    } else {
        tail = his->records;
        while (tail->next != NULL) {
            tail = tail->next;
        }
        tail->next = node;
    }
    return node;
}

Prescription *appendPrescription(HIS *his, int recordId, int patientId, int doctorId, int medicineId, int quantity) {
    Prescription *node = (Prescription *)malloc(sizeof(Prescription));
    Prescription *tail;
    if (node == NULL) {
        return NULL;
    }

    node->id = nextAvailablePrescriptionId(his);
    node->recordId = recordId;
    node->patientId = patientId;
    node->doctorId = doctorId;
    node->medicineId = medicineId;
    node->quantity = quantity;
    node->next = NULL;

    if (his->prescriptions == NULL) {
        his->prescriptions = node;
    } else {
        tail = his->prescriptions;
        while (tail->next != NULL) {
            tail = tail->next;
        }
        tail->next = node;
    }
    return node;
}

void removeRecordsByPatientId(HIS *his, int patientId) {
    Record *current = his->records;
    Record *previous = NULL;

    while (current != NULL) {
        if (current->patientId == patientId) {
            Record *toDelete = current;
            if (previous == NULL) {
                his->records = current->next;
                current = his->records;
            } else {
                previous->next = current->next;
                current = previous->next;
            }
            free(toDelete);
        } else {
            previous = current;
            current = current->next;
        }
    }
}

int deleteRecordById(HIS *his, int recordId) {
    Record *current = his->records;
    Record *previous = NULL;

    while (current != NULL) {
        if (current->id == recordId) {
            if (previous == NULL) {
                his->records = current->next;
            } else {
                previous->next = current->next;
            }
            free(current);
            return 1;
        }
        previous = current;
        current = current->next;
    }
    return 0;
}

void removePrescriptionsByPatientId(HIS *his, int patientId) {
    Prescription *current = his->prescriptions;
    Prescription *previous = NULL;

    while (current != NULL) {
        if (current->patientId == patientId) {
            Prescription *toDelete = current;
            if (previous == NULL) {
                his->prescriptions = current->next;
                current = his->prescriptions;
            } else {
                previous->next = current->next;
                current = previous->next;
            }
            free(toDelete);
        } else {
            previous = current;
            current = current->next;
        }
    }
}

int deletePatientById(HIS *his, int patientId) {
    Patient *current = his->patients;
    Patient *previous = NULL;

    while (current != NULL) {
        if (current->id == patientId) {
            if (current->admittedWardId != 0) {
                Ward *ward = findWardById(his, current->admittedWardId);
                if (ward != NULL && ward->usedBeds > 0) {
                    ward->usedBeds -= 1;
                }
            }

            removeRecordsByPatientId(his, patientId);
            removePrescriptionsByPatientId(his, patientId);

            if (previous == NULL) {
                his->patients = current->next;
            } else {
                previous->next = current->next;
            }
            free(current);
            return 1;
        }
        previous = current;
        current = current->next;
    }
    return 0;
}

int deleteDepartmentById(HIS *his, int departmentId) {
    Department *current = his->departments;
    Department *previous = NULL;
    Doctor *doctor = his->doctors;
    Ward *ward = his->wards;

    while (doctor != NULL) {
        if (doctor->departmentId == departmentId) {
            return 0;
        }
        doctor = doctor->next;
    }
    while (ward != NULL) {
        if (ward->departmentId == departmentId) {
            return 0;
        }
        ward = ward->next;
    }

    while (current != NULL) {
        if (current->id == departmentId) {
            if (previous == NULL) {
                his->departments = current->next;
            } else {
                previous->next = current->next;
            }
            free(current);
            return 1;
        }
        previous = current;
        current = current->next;
    }
    return 0;
}

int deleteDoctorById(HIS *his, int doctorId) {
    Doctor *current = his->doctors;
    Doctor *previous = NULL;
    Patient *patient = his->patients;
    Record *record = his->records;
    Prescription *prescription = his->prescriptions;

    while (patient != NULL) {
        if (patient->registeredDoctorId == doctorId) {
            return 0;
        }
        patient = patient->next;
    }
    while (record != NULL) {
        if (record->doctorId == doctorId) {
            return 0;
        }
        record = record->next;
    }
    while (prescription != NULL) {
        if (prescription->doctorId == doctorId) {
            return 0;
        }
        prescription = prescription->next;
    }

    while (current != NULL) {
        if (current->id == doctorId) {
            if (previous == NULL) {
                his->doctors = current->next;
            } else {
                previous->next = current->next;
            }
            free(current);
            return 1;
        }
        previous = current;
        current = current->next;
    }
    return 0;
}

int deleteMedicineById(HIS *his, int medicineId) {
    Medicine *current = his->medicines;
    Medicine *previous = NULL;
    Prescription *prescription = his->prescriptions;

    while (prescription != NULL) {
        if (prescription->medicineId == medicineId) {
            return 0;
        }
        prescription = prescription->next;
    }

    while (current != NULL) {
        if (current->id == medicineId) {
            if (previous == NULL) {
                his->medicines = current->next;
            } else {
                previous->next = current->next;
            }
            free(current);
            return 1;
        }
        previous = current;
        current = current->next;
    }
    return 0;
}

int updateMedicineStock(HIS *his, int medicineId, int delta) {
    Medicine *medicine = findMedicineById(his, medicineId);
    if (medicine == NULL) {
        return 0;
    }
    if (medicine->stock + delta < 0) {
        return 0;
    }
    medicine->stock += delta;
    return 1;
}

Department *findDepartmentById(HIS *his, int id) {
    Department *cur = his->departments;
    while (cur != NULL) {
        if (cur->id == id) {
            return cur;
        }
        cur = cur->next;
    }
    return NULL;
}

Doctor *findDoctorById(HIS *his, int id) {
    Doctor *cur = his->doctors;
    while (cur != NULL) {
        if (cur->id == id) {
            return cur;
        }
        cur = cur->next;
    }
    return NULL;
}

Patient *findPatientById(HIS *his, int id) {
    Patient *cur = his->patients;
    while (cur != NULL) {
        if (cur->id == id) {
            return cur;
        }
        cur = cur->next;
    }
    return NULL;
}

Record *findRecordById(HIS *his, int id) {
    Record *cur = his->records;
    while (cur != NULL) {
        if (cur->id == id) {
            return cur;
        }
        cur = cur->next;
    }
    return NULL;
}

Prescription *findPrescriptionById(HIS *his, int id) {
    Prescription *cur = his->prescriptions;
    while (cur != NULL) {
        if (cur->id == id) {
            return cur;
        }
        cur = cur->next;
    }
    return NULL;
}

Ward *findWardById(HIS *his, int id) {
    Ward *cur = his->wards;
    while (cur != NULL) {
        if (cur->id == id) {
            return cur;
        }
        cur = cur->next;
    }
    return NULL;
}

Medicine *findMedicineById(HIS *his, int id) {
    Medicine *cur = his->medicines;
    while (cur != NULL) {
        if (cur->id == id) {
            return cur;
        }
        cur = cur->next;
    }
    return NULL;
}

int registerPatient(HIS *his, int patientId, int doctorId, const char *complaint) {
    Patient *patient = findPatientById(his, patientId);
    Doctor *doctor = findDoctorById(his, doctorId);
    if (patient == NULL || doctor == NULL || complaint == NULL || isBlankString(complaint)) {
        return 0;
    }

    patient->registeredDoctorId = doctorId;
    appendRecord(his, patientId, doctorId, "挂号", complaint);
    return 1;
}

int admitPatient(HIS *his, int patientId, int wardId, const char *diagnosis) {
    Patient *patient = findPatientById(his, patientId);
    Ward *ward = findWardById(his, wardId);
    int doctorId;
    if (patient == NULL || ward == NULL || diagnosis == NULL || isBlankString(diagnosis)) {
        return 0;
    }
    if (patient->registeredDoctorId == 0 || patient->admittedWardId != 0) {
        return 0;
    }
    if (ward->usedBeds >= ward->totalBeds) {
        return 0;
    }

    ward->usedBeds += 1;
    patient->admittedWardId = wardId;
    patient->admittedBedNo = ward->usedBeds;
    doctorId = patient->registeredDoctorId;
    appendRecord(his, patientId, doctorId, "住院", diagnosis);
    return 1;
}

int prescribeMedicine(HIS *his, int patientId, int doctorId, int medicineId, int quantity) {
    Medicine *medicine = findMedicineById(his, medicineId);
    Record *record;
    if (findPatientById(his, patientId) == NULL || findDoctorById(his, doctorId) == NULL || medicine == NULL) {
        return 0;
    }
    if (quantity <= 0 || medicine->stock < quantity) {
        return 0;
    }

    medicine->stock -= quantity;
    record = appendRecord(his, patientId, doctorId, "处方", medicine->genericName);
    if (record == NULL) {
        medicine->stock += quantity;
        return 0;
    }
    if (appendPrescription(his, record->id, patientId, doctorId, medicineId, quantity) == NULL) {
        deleteRecordById(his, record->id);
        medicine->stock += quantity;
        return 0;
    }
    return 1;
}

void printDepartments(HIS *his) {
    Department *cur = his->departments;
    printf("\n=== 科室列表 ===\n");
    while (cur != NULL) {
        printf("科室ID:%d 名称:%s\n", cur->id, cur->name);
        cur = cur->next;
    }
}

void printDoctors(HIS *his) {
    Doctor *cur = his->doctors;
    printf("\n=== 医生列表 ===\n");
    while (cur != NULL) {
        Department *department = findDepartmentById(his, cur->departmentId);
        printf("医生ID:%d 姓名:%s 职称:%s 科室:%s\n",
               cur->id,
               cur->name,
               cur->title,
               department != NULL ? department->name : "未知");
        cur = cur->next;
    }
}

void printWards(HIS *his) {
    Ward *cur = his->wards;
    printf("\n=== 病房列表 ===\n");
    while (cur != NULL) {
        Department *department = findDepartmentById(his, cur->departmentId);
        printf("病房ID:%d 名称:%s 科室:%s 床位:%d/%d\n",
               cur->id,
               cur->name,
               department != NULL ? department->name : "未知",
               cur->usedBeds,
               cur->totalBeds);
        cur = cur->next;
    }
}

void printPatients(HIS *his) {
    Patient *cur = his->patients;
    printf("\n=== 患者列表 ===\n");
    while (cur != NULL) {
        Doctor *doctor = findDoctorById(his, cur->registeredDoctorId);
        Ward *ward = findWardById(his, cur->admittedWardId);
        printf("患者ID:%d 姓名:%s 年龄:%d 性别:%s 电话:%s 挂号医生:%s",
               cur->id,
               cur->name,
               cur->age,
               cur->gender,
               cur->phone,
               doctor != NULL ? doctor->name : "无");
        if (ward != NULL) {
            printf(" 病房:%s 床位:%d", ward->name, cur->admittedBedNo);
        }
        printf("\n");
        cur = cur->next;
    }
}

void printMedicines(HIS *his) {
    Medicine *cur = his->medicines;
    printf("\n=== 药品库存 ===\n");
    while (cur != NULL) {
        printf("药品ID:%d 通用名:%s 商品名:%s 库存:%d 单价:%.2f\n",
               cur->id,
               cur->genericName,
               cur->brandName,
               cur->stock,
               cur->unitPrice);
        cur = cur->next;
    }
}

void printRecords(HIS *his) {
    Record *cur = his->records;
    printf("\n=== 医疗记录 ===\n");
    while (cur != NULL) {
        Patient *patient = findPatientById(his, cur->patientId);
        Doctor *doctor = findDoctorById(his, cur->doctorId);
        printf("记录ID:%d 类型:%s 患者:%s 医生:%s 内容:%s\n",
               cur->id,
               cur->type,
               patient != NULL ? patient->name : "未知患者",
               doctor != NULL ? doctor->name : "未分配",
               cur->note);
        cur = cur->next;
    }
}

void printPrescriptions(HIS *his) {
    Prescription *cur = his->prescriptions;
    printf("\n=== 处方记录 ===\n");
    while (cur != NULL) {
        Patient *patient = findPatientById(his, cur->patientId);
        Doctor *doctor = findDoctorById(his, cur->doctorId);
        Medicine *medicine = findMedicineById(his, cur->medicineId);
        printf("处方ID:%d 关联记录ID:%d 患者:%s 医生:%s 药品:%s 数量:%d\n",
               cur->id,
               cur->recordId,
               patient != NULL ? patient->name : "未知患者",
               doctor != NULL ? doctor->name : "未知医生",
               medicine != NULL ? medicine->genericName : "未知药品",
               cur->quantity);
        cur = cur->next;
    }
}

void searchPrescriptionById(HIS *his, int prescriptionId) {
    Prescription *prescription = findPrescriptionById(his, prescriptionId);
    Patient *patient;
    Doctor *doctor;
    Medicine *medicine;
    Record *record;
    if (prescription == NULL) {
        printf("未找到处方ID为 %d 的记录。\n", prescriptionId);
        return;
    }

    patient = findPatientById(his, prescription->patientId);
    doctor = findDoctorById(his, prescription->doctorId);
    medicine = findMedicineById(his, prescription->medicineId);
    record = findRecordById(his, prescription->recordId);

    printf("\n=== 处方查询结果 ===\n");
    printf("处方ID:%d\n", prescription->id);
    printf("关联记录ID:%d\n", prescription->recordId);
    printf("患者:%s（ID:%d）\n",
           patient != NULL ? patient->name : "未知患者",
           prescription->patientId);
    printf("医生:%s（ID:%d）\n",
           doctor != NULL ? doctor->name : "未知医生",
           prescription->doctorId);
    printf("药品:%s（ID:%d）\n",
           medicine != NULL ? medicine->genericName : "未知药品",
           prescription->medicineId);
    printf("数量:%d\n", prescription->quantity);
    if (record != NULL) {
        printf("对应医疗记录: 记录ID:%d 类型:%s 内容:%s\n",
               record->id,
               record->type,
               record->note);
    } else {
        printf("对应医疗记录: 未找到关联记录。\n");
    }
}

void searchPatientByName(HIS *his, const char *keyword) {
    Patient *cur = his->patients;
    int found = 0;
    if (keyword == NULL || isBlankString(keyword)) {
        printf("查询关键字不能为空。\n");
        return;
    }
    printf("\n=== 患者查询结果 ===\n");
    while (cur != NULL) {
        if (strstr(cur->name, keyword) != NULL) {
            Doctor *doctor = findDoctorById(his, cur->registeredDoctorId);
            printf("患者ID:%d 姓名:%s 电话:%s 挂号医生:%s\n",
                   cur->id,
                   cur->name,
                   cur->phone,
                   doctor != NULL ? doctor->name : "无");
            found = 1;
        }
        cur = cur->next;
    }
    if (!found) {
        printf("未找到姓名中包含“%s”的患者。\n", keyword);
    }
}

void printStatistics(HIS *his) {
    int departmentCount = 0;
    int doctorCount = 0;
    int patientCount = 0;
    int admittedCount = 0;
    int medicineCount = 0;
    Department *department = his->departments;
    Doctor *doctor = his->doctors;
    Patient *patient = his->patients;
    Medicine *medicine = his->medicines;

    while (department != NULL) {
        departmentCount++;
        department = department->next;
    }
    while (doctor != NULL) {
        doctorCount++;
        doctor = doctor->next;
    }
    while (patient != NULL) {
        patientCount++;
        if (patient->admittedWardId != 0) {
            admittedCount++;
        }
        patient = patient->next;
    }
    while (medicine != NULL) {
        medicineCount++;
        medicine = medicine->next;
    }

    printf("\n=== 系统统计 ===\n");
    printf("科室总数: %d\n", departmentCount);
    printf("医生总数: %d\n", doctorCount);
    printf("患者总数: %d\n", patientCount);
    printf("住院患者: %d\n", admittedCount);
    printf("药品种类: %d\n", medicineCount);
}

void printBedUtilization(HIS *his) {
    Ward *ward = his->wards;
    int totalBeds = 0;
    int usedBeds = 0;

    printf("\n=== 床位利用率统计 ===\n");
    while (ward != NULL) {
        double utilization = 0.0;
        Department *department = findDepartmentById(his, ward->departmentId);
        if (ward->totalBeds > 0) {
            utilization = (double)ward->usedBeds * 100.0 / (double)ward->totalBeds;
        }
        printf("病房:%s 科室:%s 已用床位:%d 总床位:%d 利用率:%.2f%%\n",
               ward->name,
               department != NULL ? department->name : "未知",
               ward->usedBeds,
               ward->totalBeds,
               utilization);
        totalBeds += ward->totalBeds;
        usedBeds += ward->usedBeds;
        ward = ward->next;
    }

    if (totalBeds > 0) {
        printf("全院床位利用率: %.2f%% (%d/%d)\n", (double)usedBeds * 100.0 / (double)totalBeds, usedBeds, totalBeds);
    } else {
        printf("当前没有病房数据。\n");
    }
}

void printDepartmentWorkload(HIS *his) {
    Department *department = his->departments;
    printf("\n=== 科室工作量统计 ===\n");
    while (department != NULL) {
        Doctor *doctor = his->doctors;
        int doctorCount = 0;
        int patientCount = 0;
        while (doctor != NULL) {
            if (doctor->departmentId == department->id) {
                Patient *patient = his->patients;
                doctorCount++;
                while (patient != NULL) {
                    if (patient->registeredDoctorId == doctor->id) {
                        patientCount++;
                    }
                    patient = patient->next;
                }
            }
            doctor = doctor->next;
        }
        printf("科室:%s 医生数:%d 已挂号患者数:%d\n", department->name, doctorCount, patientCount);
        department = department->next;
    }
}

int exportPatients(HIS *his, const char *filename) {
    FILE *fp = fopen(filename, "w");
    Patient *cur = his->patients;
    if (fp == NULL) {
        return 0;
    }
    fprintf(fp, "id,name,age,gender,phone,doctorId,wardId,bedNo\n");
    while (cur != NULL) {
        fprintf(fp,
                "%d,%s,%d,%s,%s,%d,%d,%d\n",
                cur->id,
                cur->name,
                cur->age,
                cur->gender,
                cur->phone,
                cur->registeredDoctorId,
                cur->admittedWardId,
                cur->admittedBedNo);
        cur = cur->next;
    }
    fclose(fp);
    return 1;
}

int exportRecords(HIS *his, const char *filename) {
    FILE *fp = fopen(filename, "w");
    Record *cur = his->records;
    if (fp == NULL) {
        return 0;
    }
    fprintf(fp, "id,patientId,doctorId,type,note\n");
    while (cur != NULL) {
        fprintf(fp, "%d,%d,%d,%s,%s\n", cur->id, cur->patientId, cur->doctorId, cur->type, cur->note);
        cur = cur->next;
    }
    fclose(fp);
    return 1;
}

int exportPrescriptions(HIS *his, const char *filename) {
    FILE *fp = fopen(filename, "w");
    Prescription *cur = his->prescriptions;
    if (fp == NULL) {
        return 0;
    }
    fprintf(fp, "id,recordId,patientId,doctorId,medicineId,quantity\n");
    while (cur != NULL) {
        fprintf(fp,
                "%d,%d,%d,%d,%d,%d\n",
                cur->id,
                cur->recordId,
                cur->patientId,
                cur->doctorId,
                cur->medicineId,
                cur->quantity);
        cur = cur->next;
    }
    fclose(fp);
    return 1;
}

int exportMedicines(HIS *his, const char *filename) {
    FILE *fp = fopen(filename, "w");
    Medicine *cur = his->medicines;
    if (fp == NULL) {
        return 0;
    }
    fprintf(fp, "id,genericName,brandName,stock,unitPrice\n");
    while (cur != NULL) {
        fprintf(fp, "%d,%s,%s,%d,%.2f\n", cur->id, cur->genericName, cur->brandName, cur->stock, cur->unitPrice);
        cur = cur->next;
    }
    fclose(fp);
    return 1;
}

void exportAllData(HIS *his) {
    int ok1 = exportPatients(his, "patients.txt");
    int ok2 = exportRecords(his, "records.txt");
    int ok3 = exportPrescriptions(his, "prescriptions.txt");
    int ok4 = exportMedicines(his, "medicines.txt");
    if (ok1 && ok2 && ok3 && ok4) {
        printf("已导出 patients.txt、records.txt、prescriptions.txt、medicines.txt。\n");
    } else {
        printf("导出失败，请检查文件权限。\n");
    }
}

void freeDepartments(Department *head) {
    Department *temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

void freeDoctors(Doctor *head) {
    Doctor *temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

void freePatients(Patient *head) {
    Patient *temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

void freeWards(Ward *head) {
    Ward *temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

void freeMedicines(Medicine *head) {
    Medicine *temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

void freeRecords(Record *head) {
    Record *temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

void freePrescriptions(Prescription *head) {
    Prescription *temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

void cleanupHIS(HIS *his) {
    freeDepartments(his->departments);
    freeDoctors(his->doctors);
    freePatients(his->patients);
    freeWards(his->wards);
    freeMedicines(his->medicines);
    freeRecords(his->records);
    freePrescriptions(his->prescriptions);
    initHIS(his);
}

void destroyHIS(HIS *his) {
    if (his == NULL) {
        return;
    }
    cleanupHIS(his);
    free(his);
}

void seedBaseDepartmentsAndWards(HIS *his, int *departmentIds, int *wardIds) {
    Department *department;
    department = appendDepartment(his, "内科");
    departmentIds[0] = department->id;
    department = appendDepartment(his, "外科");
    departmentIds[1] = department->id;
    department = appendDepartment(his, "儿科");
    departmentIds[2] = department->id;
    department = appendDepartment(his, "骨科");
    departmentIds[3] = department->id;
    department = appendDepartment(his, "急诊科");
    departmentIds[4] = department->id;

    wardIds[0] = appendWard(his, "内科一病区", departmentIds[0], 15)->id;
    wardIds[1] = appendWard(his, "外科一病区", departmentIds[1], 12)->id;
    wardIds[2] = appendWard(his, "儿科一病区", departmentIds[2], 10)->id;
    wardIds[3] = appendWard(his, "骨科一病区", departmentIds[3], 10)->id;
    wardIds[4] = appendWard(his, "急诊留观区", departmentIds[4], 8)->id;
}

void seedDoctors(HIS *his, const int *departmentIds) {
    const char *titles[] = {"住院医师", "主治医师", "副主任医师", "主任医师"};
    const char *doctorNames[] = {
        "顾明轩", "程远舟", "陆知衡", "沈嘉言", "韩启川",
        "许文柏", "魏承安", "严修远", "邵景行", "苏怀瑾",
        "贺云廷", "袁清和", "崔彦霖", "卢绍庭", "任书珩",
        "姜叙白", "乔慕川", "康予安", "段星野", "傅谨言"
    };
    int i;
    for (i = 0; i < 20; i++) {
        appendDoctor(his, doctorNames[i], departmentIds[i % 5], titles[i % 4]);
    }
}

void seedMedicines(HIS *his) {
    appendMedicine(his, "阿莫西林胶囊", "阿莫仙", 300, 1.80);
    appendMedicine(his, "布洛芬缓释胶囊", "芬必得", 260, 2.50);
    appendMedicine(his, "头孢克肟分散片", "世福素", 200, 4.30);
    appendMedicine(his, "维生素C片", "VC银翘", 500, 0.60);
    appendMedicine(his, "葡萄糖注射液", "大输液", 180, 8.00);
    appendMedicine(his, "氯化钠注射液", "生理盐水", 180, 7.50);
    appendMedicine(his, "奥美拉唑肠溶胶囊", "洛赛克", 160, 3.20);
    appendMedicine(his, "蒙脱石散", "思密达", 150, 2.10);
    appendMedicine(his, "左氧氟沙星片", "可乐必妥", 140, 3.90);
    appendMedicine(his, "甲硝唑片", "灭滴灵", 220, 1.20);
    appendMedicine(his, "氨溴索口服液", "沐舒坦", 130, 5.50);
    appendMedicine(his, "双氯芬酸钠缓释片", "扶他林", 120, 2.80);
    appendMedicine(his, "维生素B6片", "VB6", 180, 0.70);
    appendMedicine(his, "乳酸菌素片", "整肠生", 160, 1.60);
    appendMedicine(his, "小儿氨酚黄那敏颗粒", "护彤", 140, 1.50);
    appendMedicine(his, "碘伏消毒液", "医用碘伏", 90, 6.80);
    appendMedicine(his, "云南白药气雾剂", "云南白药", 80, 18.50);
    appendMedicine(his, "地塞米松片", "信谊", 100, 0.90);
    appendMedicine(his, "硫酸庆大霉素注射液", "庆大", 90, 2.20);
    appendMedicine(his, "赖氨匹林散", "赖氨匹林", 110, 1.10);
    appendMedicine(his, "氯雷他定片", "开瑞坦", 150, 2.60);
    appendMedicine(his, "复方甘草片", "甘草片", 170, 0.80);
    appendMedicine(his, "阿奇霉素片", "希舒美", 120, 4.80);
    appendMedicine(his, "氨咖黄敏胶囊", "速效伤风胶囊", 210, 1.30);
    appendMedicine(his, "泮托拉唑钠肠溶片", "潘妥洛克", 100, 3.80);
    appendMedicine(his, "硝苯地平缓释片", "伲福达", 160, 1.90);
    appendMedicine(his, "盐酸二甲双胍片", "格华止", 180, 1.40);
    appendMedicine(his, "诺氟沙星胶囊", "氟哌酸", 150, 1.10);
    appendMedicine(his, "阿司匹林肠溶片", "拜阿司匹灵", 140, 2.20);
    appendMedicine(his, "复方氨基酸注射液", "氨基酸", 70, 12.50);
}

void buildCourseDataset(HIS *his) {
    const char *surnames[] = {
        "赵", "钱", "孙", "李", "周", "吴", "郑", "王", "冯", "陈",
        "褚", "卫", "蒋", "沈", "韩", "杨", "朱", "秦", "尤", "许"
    };
    const char *givenNames[] = {
        "雷", "敏", "华", "静", "强", "芳", "娜", "军", "洋", "婷",
        "磊", "洁", "晨", "雪", "凯", "欣", "博", "莹", "辉", "倩"
    };
    const char *complaints[] = {
        "发热咳嗽三天", "腹痛待查", "关节疼痛", "儿童发烧", "外伤复诊",
        "胃部不适", "头晕乏力", "咽痛", "腰背疼痛", "皮肤过敏"
    };
    const char *diagnoses[] = {
        "肺部感染，建议住院观察", "急性阑尾炎，术前准备", "骨折复位后住院治疗", "高热脱水，补液治疗", "外伤缝合后观察"
    };
    int departmentIds[5];
    int wardIds[5];
    int i;

    cleanupHIS(his);
    seedBaseDepartmentsAndWards(his, departmentIds, wardIds);
    seedDoctors(his, departmentIds);
    seedMedicines(his);

    for (i = 0; i < 135; i++) {
        char patientName[NAME_LEN];
        char phone[ID_LEN];
        char note[NOTE_LEN];
        char gender[16];
        int age = 18 + (i % 63);
        int doctorId;
        int medicineId;
        Patient *patient;

        snprintf(patientName,
                 sizeof(patientName),
                 "%s%s%d",
                 surnames[i % 20],
                 givenNames[(i * 3) % 20],
                 (i % 7) + 1);
        snprintf(phone, sizeof(phone), "139%08d", i + 1);
        strcpy(gender, (i % 2 == 0) ? "男" : "女");
        patient = appendPatient(his, patientName, age, gender, phone);
        doctorId = (i % 20) + 1;
        snprintf(note, sizeof(note), "%s（门诊编号:%03d）", complaints[i % 10], i + 1);
        registerPatient(his, patient->id, doctorId, note);

        if (i < 35) {
            admitPatient(his, patient->id, wardIds[i % 5], diagnoses[i % 5]);
        }
        if (i < 90) {
            medicineId = (i % 30) + 1;
            prescribeMedicine(his, patient->id, doctorId, medicineId, (i % 3) + 1);
        }
    }
}

int loadDataFromDefaultFiles(HIS *his) {
    char line[INPUT_LEN];
    int maxPatientId = 0;
    int maxMedicineId = 0;
    int maxRecordId = 0;
    int maxPrescriptionId = 0;
    FILE *fp;

    cleanupHIS(his);

    fp = fopen("patients.txt", "r");
    if (fp == NULL) {
        return 0;
    }
    while (fgets(line, sizeof(line), fp) != NULL) {
        int id;
        int age;
        int doctorId;
        int wardId;
        int bedNo;
        char name[NAME_LEN];
        char gender[16];
        char phone[ID_LEN];
        trimNewline(line);
        if (isBlankString(line) || strncmp(line, "id,", 3) == 0) {
            continue;
        }
        if (sscanf(line, "%d,%63[^,],%d,%15[^,],%31[^,],%d,%d,%d",
                   &id, name, &age, gender, phone, &doctorId, &wardId, &bedNo) == 8) {
            Patient *patient = appendPatient(his, name, age, gender, phone);
            if (patient == NULL) {
                fclose(fp);
                return 0;
            }
            patient->id = id;
            patient->registeredDoctorId = doctorId;
            patient->admittedWardId = wardId;
            patient->admittedBedNo = bedNo;
            if (id > maxPatientId) {
                maxPatientId = id;
            }
        }
    }
    fclose(fp);

    fp = fopen("medicines.txt", "r");
    if (fp == NULL) {
        cleanupHIS(his);
        return 0;
    }
    while (fgets(line, sizeof(line), fp) != NULL) {
        int id;
        int stock;
        double unitPrice;
        char genericName[NAME_LEN];
        char brandName[NAME_LEN];
        trimNewline(line);
        if (isBlankString(line) || strncmp(line, "id,", 3) == 0) {
            continue;
        }
        if (sscanf(line, "%d,%63[^,],%63[^,],%d,%lf", &id, genericName, brandName, &stock, &unitPrice) == 5) {
            Medicine *medicine = appendMedicine(his, genericName, brandName, stock, unitPrice);
            if (medicine == NULL) {
                fclose(fp);
                cleanupHIS(his);
                return 0;
            }
            medicine->id = id;
            if (id > maxMedicineId) {
                maxMedicineId = id;
            }
        }
    }
    fclose(fp);

    fp = fopen("records.txt", "r");
    if (fp == NULL) {
        cleanupHIS(his);
        return 0;
    }
    while (fgets(line, sizeof(line), fp) != NULL) {
        int id;
        int patientId;
        int doctorId;
        char type[TYPE_LEN];
        char note[NOTE_LEN];
        trimNewline(line);
        if (isBlankString(line) || strncmp(line, "id,", 3) == 0) {
            continue;
        }
        if (sscanf(line, "%d,%d,%d,%31[^,],%127[^\n]", &id, &patientId, &doctorId, type, note) == 5) {
            Record *record = appendRecord(his, patientId, doctorId, type, note);
            if (record == NULL) {
                fclose(fp);
                cleanupHIS(his);
                return 0;
            }
            record->id = id;
            if (id > maxRecordId) {
                maxRecordId = id;
            }
        }
    }
    fclose(fp);

    fp = fopen("prescriptions.txt", "r");
    if (fp == NULL) {
        cleanupHIS(his);
        return 0;
    }
    while (fgets(line, sizeof(line), fp) != NULL) {
        int id;
        int recordId;
        int patientId;
        int doctorId;
        int medicineId;
        int quantity;
        trimNewline(line);
        if (isBlankString(line) || strncmp(line, "id,", 3) == 0) {
            continue;
        }
        if (sscanf(line, "%d,%d,%d,%d,%d,%d", &id, &recordId, &patientId, &doctorId, &medicineId, &quantity) == 6) {
            Prescription *prescription = appendPrescription(his, recordId, patientId, doctorId, medicineId, quantity);
            if (prescription == NULL) {
                fclose(fp);
                cleanupHIS(his);
                return 0;
            }
            prescription->id = id;
            if (id > maxPrescriptionId) {
                maxPrescriptionId = id;
            }
        }
    }
    fclose(fp);

    his->nextPatientId = maxPatientId + 1;
    his->nextMedicineId = maxMedicineId + 1;
    his->nextRecordId = maxRecordId + 1;
    his->nextPrescriptionId = maxPrescriptionId + 1;
    return 1;
}

void addPatientInteractive(HIS *his) {
    char name[NAME_LEN];
    char gender[16];
    char phone[ID_LEN];
    int age;
    Patient *patient;

    readLine("请输入患者姓名: ", name, sizeof(name));
    age = readInt("请输入年龄: ");
    readLine("请输入性别: ", gender, sizeof(gender));
    readLine("请输入电话: ", phone, sizeof(phone));

    if (!isValidPatientName(name)) {
        printf("新增患者失败：姓名不能为空，且长度至少为 2 个字符。\n");
        return;
    }
    if (!isValidAge(age)) {
        printf("新增患者失败：年龄必须在 0 到 120 之间。\n");
        return;
    }
    if (!isValidGender(gender)) {
        printf("新增患者失败：性别只能输入“男”或“女”。\n");
        return;
    }
    if (!isValidPhone(phone)) {
        printf("新增患者失败：电话必须是 11 位数字。\n");
        return;
    }

    patient = appendPatient(his, name, age, gender, phone);
    if (patient != NULL) {
        printf("新增患者成功，患者ID为 %d。\n", patient->id);
    } else {
        printf("新增患者失败，内存申请失败。\n");
    }
}

void deletePatientInteractive(HIS *his) {
    int patientId = readInt("请输入要删除的患者ID: ");
    if (findPatientById(his, patientId) == NULL) {
        printf("删除失败：患者ID不存在。\n");
        return;
    }

    if (deletePatientById(his, patientId)) {
        printf("删除患者成功，关联医疗记录和处方已一并清理。\n");
    } else {
        printf("删除失败：系统未能完成删除操作。\n");
    }
}

void addDepartmentInteractive(HIS *his) {
    char name[NAME_LEN];
    Department *department;
    readLine("请输入科室名称: ", name, sizeof(name));
    if (isBlankString(name)) {
        printf("新增科室失败：科室名称不能为空。\n");
        return;
    }
    department = appendDepartment(his, name);
    if (department != NULL) {
        printf("新增科室成功，科室ID为 %d。\n", department->id);
    } else {
        printf("新增科室失败。\n");
    }
}

void modifyDepartmentInteractive(HIS *his) {
    int departmentId = readInt("请输入要修改的科室ID: ");
    Department *department = findDepartmentById(his, departmentId);
    char name[NAME_LEN];

    if (department == NULL) {
        printf("修改科室失败：科室ID不存在。\n");
        return;
    }

    printf("当前科室名称：%s\n", department->name);
    readLine("请输入新的科室名称（直接回车保持不变）: ", name, sizeof(name));
    if (!isBlankString(name)) {
        strncpy(department->name, name, NAME_LEN - 1);
        department->name[NAME_LEN - 1] = '\0';
    }
    printf("修改科室成功。\n");
}

void deleteDepartmentInteractive(HIS *his) {
    int departmentId = readInt("请输入要删除的科室ID: ");
    if (findDepartmentById(his, departmentId) == NULL) {
        printf("删除科室失败：科室ID不存在。\n");
        return;
    }
    if (deleteDepartmentById(his, departmentId)) {
        printf("删除科室成功。\n");
    } else {
        printf("删除科室失败：该科室下仍有关联医生或病房。\n");
    }
}

void addDoctorInteractive(HIS *his) {
    char name[NAME_LEN];
    char title[TITLE_LEN];
    int departmentId;
    Doctor *doctor;

    readLine("请输入医生姓名: ", name, sizeof(name));
    departmentId = readInt("请输入所属科室ID: ");
    readLine("请输入职称: ", title, sizeof(title));

    if (isBlankString(name)) {
        printf("新增医生失败：姓名不能为空。\n");
        return;
    }
    if (findDepartmentById(his, departmentId) == NULL) {
        printf("新增医生失败：科室ID不存在。\n");
        return;
    }
    if (isBlankString(title)) {
        printf("新增医生失败：职称不能为空。\n");
        return;
    }

    doctor = appendDoctor(his, name, departmentId, title);
    if (doctor != NULL) {
        printf("新增医生成功，医生ID为 %d。\n", doctor->id);
    } else {
        printf("新增医生失败。\n");
    }
}

void modifyDoctorInteractive(HIS *his) {
    int doctorId = readInt("请输入要修改的医生ID: ");
    Doctor *doctor = findDoctorById(his, doctorId);
    char name[NAME_LEN];
    char title[TITLE_LEN];
    char departmentBuffer[INPUT_LEN];
    int departmentId;

    if (doctor == NULL) {
        printf("修改医生失败：医生ID不存在。\n");
        return;
    }

    printf("当前姓名：%s 当前职称：%s 当前科室ID：%d\n", doctor->name, doctor->title, doctor->departmentId);
    readLine("请输入新的医生姓名（直接回车保持不变）: ", name, sizeof(name));
    readLine("请输入新的职称（直接回车保持不变）: ", title, sizeof(title));
    readLine("请输入新的科室ID（直接回车保持不变）: ", departmentBuffer, sizeof(departmentBuffer));

    if (!isBlankString(name)) {
        strncpy(doctor->name, name, NAME_LEN - 1);
        doctor->name[NAME_LEN - 1] = '\0';
    }
    if (!isBlankString(title)) {
        strncpy(doctor->title, title, TITLE_LEN - 1);
        doctor->title[TITLE_LEN - 1] = '\0';
    }
    if (!isBlankString(departmentBuffer)) {
        if (sscanf(departmentBuffer, "%d", &departmentId) != 1 || findDepartmentById(his, departmentId) == NULL) {
            printf("修改医生失败：新的科室ID无效。\n");
            return;
        }
        doctor->departmentId = departmentId;
    }
    printf("修改医生成功。\n");
}

void deleteDoctorInteractive(HIS *his) {
    int doctorId = readInt("请输入要删除的医生ID: ");
    if (findDoctorById(his, doctorId) == NULL) {
        printf("删除医生失败：医生ID不存在。\n");
        return;
    }
    if (deleteDoctorById(his, doctorId)) {
        printf("删除医生成功。\n");
    } else {
        printf("删除医生失败：该医生已有患者、记录或处方关联。\n");
    }
}

void addMedicineInteractive(HIS *his) {
    char genericName[NAME_LEN];
    char brandName[NAME_LEN];
    int stock;
    char priceBuffer[INPUT_LEN];
    double unitPrice;
    Medicine *medicine;

    readLine("请输入药品通用名: ", genericName, sizeof(genericName));
    readLine("请输入药品商品名: ", brandName, sizeof(brandName));
    stock = readInt("请输入初始库存: ");
    readLine("请输入药品单价: ", priceBuffer, sizeof(priceBuffer));

    if (isBlankString(genericName) || isBlankString(brandName)) {
        printf("新增药品失败：药品名称不能为空。\n");
        return;
    }
    if (stock < 0) {
        printf("新增药品失败：库存不能为负数。\n");
        return;
    }
    if (sscanf(priceBuffer, "%lf", &unitPrice) != 1 || unitPrice < 0) {
        printf("新增药品失败：单价格式不正确。\n");
        return;
    }

    medicine = appendMedicine(his, genericName, brandName, stock, unitPrice);
    if (medicine != NULL) {
        printf("新增药品成功，药品ID为 %d。\n", medicine->id);
    } else {
        printf("新增药品失败。\n");
    }
}

void modifyPatientInteractive(HIS *his) {
    int patientId = readInt("请输入要修改的患者ID: ");
    Patient *patient = findPatientById(his, patientId);
    char name[NAME_LEN];
    char gender[16];
    char phone[ID_LEN];
    char ageBuffer[INPUT_LEN];
    int age;

    if (patient == NULL) {
        printf("修改患者失败：患者ID不存在。\n");
        return;
    }

    printf("当前姓名：%s 当前年龄：%d 当前性别：%s 当前电话：%s\n",
           patient->name,
           patient->age,
           patient->gender,
           patient->phone);
    readLine("请输入新的姓名（直接回车保持不变）: ", name, sizeof(name));
    readLine("请输入新的年龄（直接回车保持不变）: ", ageBuffer, sizeof(ageBuffer));
    readLine("请输入新的性别（直接回车保持不变）: ", gender, sizeof(gender));
    readLine("请输入新的电话（直接回车保持不变）: ", phone, sizeof(phone));

    if (!isBlankString(name)) {
        if (!isValidPatientName(name)) {
            printf("修改患者失败：姓名格式不正确。\n");
            return;
        }
        strncpy(patient->name, name, NAME_LEN - 1);
        patient->name[NAME_LEN - 1] = '\0';
    }
    if (!isBlankString(ageBuffer)) {
        if (sscanf(ageBuffer, "%d", &age) != 1 || !isValidAge(age)) {
            printf("修改患者失败：年龄格式不正确。\n");
            return;
        }
        patient->age = age;
    }
    if (!isBlankString(gender)) {
        if (!isValidGender(gender)) {
            printf("修改患者失败：性别只能输入“男”或“女”。\n");
            return;
        }
        strncpy(patient->gender, gender, sizeof(patient->gender) - 1);
        patient->gender[sizeof(patient->gender) - 1] = '\0';
    }
    if (!isBlankString(phone)) {
        if (!isValidPhone(phone)) {
            printf("修改患者失败：电话必须是 11 位数字。\n");
            return;
        }
        strncpy(patient->phone, phone, ID_LEN - 1);
        patient->phone[ID_LEN - 1] = '\0';
    }
    printf("修改患者成功。\n");
}

void deleteMedicineInteractive(HIS *his) {
    int medicineId = readInt("请输入要删除的药品ID: ");
    if (findMedicineById(his, medicineId) == NULL) {
        printf("删除药品失败：药品ID不存在。\n");
        return;
    }
    if (deleteMedicineById(his, medicineId)) {
        printf("删除药品成功。\n");
    } else {
        printf("删除药品失败：该药品已有处方关联。\n");
    }
}

void modifyMedicineInteractive(HIS *his) {
    int medicineId = readInt("请输入要修改的药品ID: ");
    Medicine *medicine = findMedicineById(his, medicineId);
    char genericName[NAME_LEN];
    char brandName[NAME_LEN];
    char stockBuffer[INPUT_LEN];
    char priceBuffer[INPUT_LEN];
    int stock;
    double unitPrice;

    if (medicine == NULL) {
        printf("修改药品失败：药品ID不存在。\n");
        return;
    }

    printf("当前通用名：%s 当前商品名：%s 当前库存：%d 当前单价：%.2f\n",
           medicine->genericName,
           medicine->brandName,
           medicine->stock,
           medicine->unitPrice);
    readLine("请输入新的通用名（直接回车保持不变）: ", genericName, sizeof(genericName));
    readLine("请输入新的商品名（直接回车保持不变）: ", brandName, sizeof(brandName));
    readLine("请输入新的库存（直接回车保持不变）: ", stockBuffer, sizeof(stockBuffer));
    readLine("请输入新的单价（直接回车保持不变）: ", priceBuffer, sizeof(priceBuffer));

    if (!isBlankString(genericName)) {
        strncpy(medicine->genericName, genericName, NAME_LEN - 1);
        medicine->genericName[NAME_LEN - 1] = '\0';
    }
    if (!isBlankString(brandName)) {
        strncpy(medicine->brandName, brandName, NAME_LEN - 1);
        medicine->brandName[NAME_LEN - 1] = '\0';
    }
    if (!isBlankString(stockBuffer)) {
        if (sscanf(stockBuffer, "%d", &stock) != 1 || stock < 0) {
            printf("修改药品失败：库存格式不正确。\n");
            return;
        }
        medicine->stock = stock;
    }
    if (!isBlankString(priceBuffer)) {
        if (sscanf(priceBuffer, "%lf", &unitPrice) != 1 || unitPrice < 0) {
            printf("修改药品失败：单价格式不正确。\n");
            return;
        }
        medicine->unitPrice = unitPrice;
    }
    printf("修改药品成功。\n");
}

void adjustMedicineStockInteractive(HIS *his) {
    int medicineId = readInt("请输入药品ID: ");
    int delta = readInt("请输入库存调整量（可输入负数）: ");
    if (findMedicineById(his, medicineId) == NULL) {
        printf("库存调整失败：药品ID不存在。\n");
        return;
    }
    if (updateMedicineStock(his, medicineId, delta)) {
        printf("库存调整成功。\n");
    } else {
        printf("库存调整失败：调整后库存不能小于 0。\n");
    }
}

void importDepartmentsFromFileInteractive(HIS *his) {
    char filename[INPUT_LEN];
    char line[INPUT_LEN];
    int imported = 0;
    int skipped = 0;
    FILE *fp;

    readLine("请输入科室导入文件名: ", filename, sizeof(filename));
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("导入失败：无法打开文件。\n");
        return;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        trimNewline(line);
        if (isBlankString(line) || strcmp(line, "name") == 0) {
            continue;
        }
        if (appendDepartment(his, line) != NULL) {
            imported++;
        } else {
            skipped++;
        }
    }
    fclose(fp);
    printf("科室导入完成：成功 %d 条，跳过 %d 条。\n", imported, skipped);
}

void importDoctorsFromFileInteractive(HIS *his) {
    char filename[INPUT_LEN];
    char line[INPUT_LEN];
    int imported = 0;
    int skipped = 0;
    FILE *fp;

    readLine("请输入医生导入文件名: ", filename, sizeof(filename));
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("导入失败：无法打开文件。\n");
        return;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        char *name;
        char *departmentStr;
        char *title;
        int departmentId;
        trimNewline(line);
        if (isBlankString(line)) {
            continue;
        }
        name = strtok(line, ",");
        departmentStr = strtok(NULL, ",");
        title = strtok(NULL, ",");
        if (name == NULL || departmentStr == NULL || title == NULL || strcmp(name, "name") == 0) {
            skipped++;
            continue;
        }
        if (sscanf(departmentStr, "%d", &departmentId) != 1 || findDepartmentById(his, departmentId) == NULL) {
            skipped++;
            continue;
        }
        if (appendDoctor(his, name, departmentId, title) != NULL) {
            imported++;
        } else {
            skipped++;
        }
    }
    fclose(fp);
    printf("医生导入完成：成功 %d 条，跳过 %d 条。\n", imported, skipped);
}

void importPatientsFromFileInteractive(HIS *his) {
    char filename[INPUT_LEN];
    char line[INPUT_LEN];
    int imported = 0;
    int skipped = 0;
    FILE *fp;

    readLine("请输入患者导入文件名: ", filename, sizeof(filename));
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("导入失败：无法打开文件。\n");
        return;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        char *name;
        char *ageStr;
        char *gender;
        char *phone;
        int age;
        trimNewline(line);
        if (isBlankString(line)) {
            continue;
        }
        name = strtok(line, ",");
        ageStr = strtok(NULL, ",");
        gender = strtok(NULL, ",");
        phone = strtok(NULL, ",");
        if (name == NULL || ageStr == NULL || gender == NULL || phone == NULL || strcmp(name, "name") == 0) {
            skipped++;
            continue;
        }
        if (sscanf(ageStr, "%d", &age) != 1) {
            skipped++;
            continue;
        }
        if (appendPatient(his, name, age, gender, phone) != NULL) {
            imported++;
        } else {
            skipped++;
        }
    }
    fclose(fp);
    printf("患者导入完成：成功 %d 条，跳过 %d 条。\n", imported, skipped);
}

void importMedicinesFromFileInteractive(HIS *his) {
    char filename[INPUT_LEN];
    char line[INPUT_LEN];
    int imported = 0;
    int skipped = 0;
    FILE *fp;

    readLine("请输入药品导入文件名: ", filename, sizeof(filename));
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("导入失败：无法打开文件。\n");
        return;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        char *genericName;
        char *brandName;
        char *stockStr;
        char *priceStr;
        int stock;
        double unitPrice;
        trimNewline(line);
        if (isBlankString(line)) {
            continue;
        }
        genericName = strtok(line, ",");
        brandName = strtok(NULL, ",");
        stockStr = strtok(NULL, ",");
        priceStr = strtok(NULL, ",");
        if (genericName == NULL || brandName == NULL || stockStr == NULL || priceStr == NULL || strcmp(genericName, "genericName") == 0) {
            skipped++;
            continue;
        }
        if (sscanf(stockStr, "%d", &stock) != 1 || sscanf(priceStr, "%lf", &unitPrice) != 1) {
            skipped++;
            continue;
        }
        if (appendMedicine(his, genericName, brandName, stock, unitPrice) != NULL) {
            imported++;
        } else {
            skipped++;
        }
    }
    fclose(fp);
    printf("药品导入完成：成功 %d 条，跳过 %d 条。\n", imported, skipped);
}

void registerPatientInteractive(HIS *his) {
    int patientId = readInt("请输入患者ID: ");
    int doctorId = readInt("请输入医生ID: ");
    char complaint[NOTE_LEN];
    readLine("请输入主诉: ", complaint, sizeof(complaint));

    if (findPatientById(his, patientId) == NULL) {
        printf("挂号失败：患者ID不存在。\n");
        return;
    }
    if (findDoctorById(his, doctorId) == NULL) {
        printf("挂号失败：医生ID不存在。\n");
        return;
    }
    if (isBlankString(complaint)) {
        printf("挂号失败：主诉不能为空。\n");
        return;
    }

    if (registerPatient(his, patientId, doctorId, complaint)) {
        printf("挂号成功。\n");
    } else {
        printf("挂号失败，请检查患者ID和医生ID。\n");
    }
}

void admitPatientInteractive(HIS *his) {
    int patientId = readInt("请输入患者ID: ");
    int wardId = readInt("请输入病房ID: ");
    char diagnosis[NOTE_LEN];
    Patient *patient = findPatientById(his, patientId);
    Ward *ward = findWardById(his, wardId);
    readLine("请输入住院诊断: ", diagnosis, sizeof(diagnosis));

    if (patient == NULL) {
        printf("住院办理失败：患者ID不存在。\n");
        return;
    }
    if (ward == NULL) {
        printf("住院办理失败：病房ID不存在。\n");
        return;
    }
    if (patient->registeredDoctorId == 0) {
        printf("住院办理失败：患者还没有挂号，不能直接住院。\n");
        return;
    }
    if (patient->admittedWardId != 0) {
        printf("住院办理失败：该患者已经住院。\n");
        return;
    }
    if (isBlankString(diagnosis)) {
        printf("住院办理失败：住院诊断不能为空。\n");
        return;
    }

    if (admitPatient(his, patientId, wardId, diagnosis)) {
        printf("住院办理成功。\n");
    } else {
        printf("住院办理失败，请检查患者ID、病房ID或床位余量。\n");
    }
}

void prescribeMedicineInteractive(HIS *his) {
    int patientId = readInt("请输入患者ID: ");
    int doctorId = readInt("请输入医生ID: ");
    int medicineId = readInt("请输入药品ID: ");
    int quantity = readInt("请输入开药数量: ");

    if (findPatientById(his, patientId) == NULL) {
        printf("处方录入失败：患者ID不存在。\n");
        return;
    }
    if (findDoctorById(his, doctorId) == NULL) {
        printf("处方录入失败：医生ID不存在。\n");
        return;
    }
    if (findMedicineById(his, medicineId) == NULL) {
        printf("处方录入失败：药品ID不存在。\n");
        return;
    }
    if (quantity <= 0) {
        printf("处方录入失败：开药数量必须大于 0。\n");
        return;
    }

    if (prescribeMedicine(his, patientId, doctorId, medicineId, quantity)) {
        printf("处方录入成功。\n");
    } else {
        printf("处方录入失败，请检查输入或库存。\n");
    }
}

void searchPatientInteractive(HIS *his) {
    char keyword[NAME_LEN];
    readLine("请输入患者姓名关键字: ", keyword, sizeof(keyword));
    if (isBlankString(keyword)) {
        printf("查询失败：关键字不能为空。\n");
        return;
    }
    searchPatientByName(his, keyword);
}

void searchPrescriptionInteractive(HIS *his) {
    int prescriptionId = readInt("请输入处方ID: ");
    if (prescriptionId <= 0) {
        printf("查询失败：处方ID必须大于 0。\n");
        return;
    }
    searchPrescriptionById(his, prescriptionId);
}

void showMainMenu(void) {
    printf("\n========== 轻量级 HIS 课程设计版 ==========\n");
    printf("1. 科室管理\n");
    printf("2. 医生管理\n");
    printf("3. 患者管理\n");
    printf("4. 药品管理\n");
    printf("5. 诊疗业务\n");
    printf("6. 查询统计\n");
    printf("7. 导出 txt 数据文件\n");
    printf("8. 从默认文件重新加载数据\n");
    printf("0. 退出系统\n");
}

void showDepartmentMenu(void) {
    printf("\n--- 科室管理 ---\n");
    printf("1. 查看科室列表\n");
    printf("2. 新增科室\n");
    printf("3. 修改科室\n");
    printf("4. 删除科室\n");
    printf("5. 从文件导入科室\n");
    printf("0. 返回上一级\n");
}

void showDoctorMenu(void) {
    printf("\n--- 医生管理 ---\n");
    printf("1. 查看医生列表\n");
    printf("2. 新增医生\n");
    printf("3. 修改医生\n");
    printf("4. 删除医生\n");
    printf("5. 从文件导入医生\n");
    printf("0. 返回上一级\n");
}

void showPatientMenu(void) {
    printf("\n--- 患者管理 ---\n");
    printf("1. 查看患者列表\n");
    printf("2. 新增患者\n");
    printf("3. 修改患者\n");
    printf("4. 删除患者\n");
    printf("5. 患者姓名查询\n");
    printf("6. 从文件导入患者\n");
    printf("0. 返回上一级\n");
}

void showMedicineMenu(void) {
    printf("\n--- 药品管理 ---\n");
    printf("1. 查看药品库存\n");
    printf("2. 新增药品\n");
    printf("3. 修改药品\n");
    printf("4. 删除药品\n");
    printf("5. 调整药品库存\n");
    printf("6. 从文件导入药品\n");
    printf("0. 返回上一级\n");
}

void showBusinessMenu(void) {
    printf("\n--- 诊疗业务 ---\n");
    printf("1. 办理挂号\n");
    printf("2. 办理住院\n");
    printf("3. 开具处方\n");
    printf("0. 返回上一级\n");
}

void showQueryMenu(void) {
    printf("\n--- 查询统计 ---\n");
    printf("1. 查看病房列表\n");
    printf("2. 查看医疗记录\n");
    printf("3. 查看处方记录\n");
    printf("4. 按处方ID查询对应医疗记录\n");
    printf("5. 查看综合统计\n");
    printf("6. 查看床位利用率\n");
    printf("7. 查看科室工作量统计\n");
    printf("0. 返回上一级\n");
}

void departmentMenuLoop(HIS *his) {
    int choice;
    while (1) {
        showDepartmentMenu();
        choice = readInt("请输入菜单编号: ");
        switch (choice) {
            case 1:
                printDepartments(his);
                break;
            case 2:
                addDepartmentInteractive(his);
                break;
            case 3:
                modifyDepartmentInteractive(his);
                break;
            case 4:
                deleteDepartmentInteractive(his);
                break;
            case 5:
                importDepartmentsFromFileInteractive(his);
                break;
            case 0:
                return;
            default:
                printf("菜单编号无效，请重新输入。\n");
        }
    }
}

void doctorMenuLoop(HIS *his) {
    int choice;
    while (1) {
        showDoctorMenu();
        choice = readInt("请输入菜单编号: ");
        switch (choice) {
            case 1:
                printDoctors(his);
                break;
            case 2:
                addDoctorInteractive(his);
                break;
            case 3:
                modifyDoctorInteractive(his);
                break;
            case 4:
                deleteDoctorInteractive(his);
                break;
            case 5:
                importDoctorsFromFileInteractive(his);
                break;
            case 0:
                return;
            default:
                printf("菜单编号无效，请重新输入。\n");
        }
    }
}

void patientMenuLoop(HIS *his) {
    int choice;
    while (1) {
        showPatientMenu();
        choice = readInt("请输入菜单编号: ");
        switch (choice) {
            case 1:
                printPatients(his);
                break;
            case 2:
                addPatientInteractive(his);
                break;
            case 3:
                modifyPatientInteractive(his);
                break;
            case 4:
                deletePatientInteractive(his);
                break;
            case 5:
                searchPatientInteractive(his);
                break;
            case 6:
                importPatientsFromFileInteractive(his);
                break;
            case 0:
                return;
            default:
                printf("菜单编号无效，请重新输入。\n");
        }
    }
}

void medicineMenuLoop(HIS *his) {
    int choice;
    while (1) {
        showMedicineMenu();
        choice = readInt("请输入菜单编号: ");
        switch (choice) {
            case 1:
                printMedicines(his);
                break;
            case 2:
                addMedicineInteractive(his);
                break;
            case 3:
                modifyMedicineInteractive(his);
                break;
            case 4:
                deleteMedicineInteractive(his);
                break;
            case 5:
                adjustMedicineStockInteractive(his);
                break;
            case 6:
                importMedicinesFromFileInteractive(his);
                break;
            case 0:
                return;
            default:
                printf("菜单编号无效，请重新输入。\n");
        }
    }
}

void businessMenuLoop(HIS *his) {
    int choice;
    while (1) {
        showBusinessMenu();
        choice = readInt("请输入菜单编号: ");
        switch (choice) {
            case 1:
                registerPatientInteractive(his);
                break;
            case 2:
                admitPatientInteractive(his);
                break;
            case 3:
                prescribeMedicineInteractive(his);
                break;
            case 0:
                return;
            default:
                printf("菜单编号无效，请重新输入。\n");
        }
    }
}

void queryMenuLoop(HIS *his) {
    int choice;
    while (1) {
        showQueryMenu();
        choice = readInt("请输入菜单编号: ");
        switch (choice) {
            case 1:
                printWards(his);
                break;
            case 2:
                printRecords(his);
                break;
            case 3:
                printPrescriptions(his);
                break;
            case 4:
                searchPrescriptionInteractive(his);
                break;
            case 5:
                printStatistics(his);
                break;
            case 6:
                printBedUtilization(his);
                break;
            case 7:
                printDepartmentWorkload(his);
                break;
            case 0:
                return;
            default:
                printf("菜单编号无效，请重新输入。\n");
        }
    }
}
