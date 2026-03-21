## 文件说明：
## 1. 定义 HIS 项目的编译、运行、导出和清理命令；
## 2. 统一管理多源文件构建入口；
## 3. 便于课程设计演示和重复执行测试流程。
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11
TARGET = his_demo
SRC = main.c his.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

export: $(TARGET)
	printf '7\n0\n' | ./$(TARGET)

clean:
	rm -f $(TARGET) *.o
