CC=g++
CFLAGS=-std=c++11 -Wextra -Wall -Wno-unused -O3 -DNDEBUG -pedantic -fPIC

# 添加 CaDiCaL 路径
CADICAL_DIR=../cadical-master
CADICAL_LIB=$(CADICAL_DIR)/build/libcadical.a

MAJOR=1
MINOR=1
VERSION=$(MAJOR).$(MINOR)

# 修改：直接列出所有对象文件（用于静态库）
OBJECTS:=main.o nenofex.o stack.o queue.o mem.o atpg.o

# 位置无关的对象文件（用于共享库）
PIC_OBJECTS:=main.fpico nenofex.fpico stack.fpico queue.fpico mem.fpico atpg.fpico

# 默认目标：生成静态库和可执行文件
all: libnenofex.a nenofex

UNAME:=$(shell uname)

ifeq ($(UNAME), Darwin)
# Mac OS X
SONAME=-install_name
TARGETS+=libnenofex.$(VERSION).dylib
else
SONAME=-soname
TARGETS+=libnenofex.so.$(VERSION)
endif

.SUFFIXES: .c .o .fpico

# 编译普通 .o 文件（用于静态库和可执行文件）
.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

# 编译位置无关的 .fpico 文件（用于共享库）
.c.fpico:
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

# 可执行文件（链接静态库）
nenofex: $(OBJECTS) $(CADICAL_LIB)
	$(CC) $(CFLAGS) -o nenofex $(OBJECTS) $(CADICAL_LIB)

# 静态库（用于 DepQBF 链接）
libnenofex.a: $(OBJECTS)
	@echo "Creating static library libnenofex.a..."
	ar rcs $@ $^
	@echo "Static library created successfully"
	@echo "Exporting symbols:"
	@nm $@ | grep -E "nenofex_" | head -10 || echo "Warning: No nenofex_* symbols found"

# 共享库
libnenofex.so.$(VERSION): $(PIC_OBJECTS)
	$(CC) $(LFLAGS) -shared -Wl,$(SONAME),libnenofex.so.$(MAJOR) $^ -o $@

libnenofex.$(VERSION).dylib: $(PIC_OBJECTS)
	$(CC) $(LFLAGS) -shared -Wl,$(SONAME),libnenofex.$(MAJOR).dylib $^ -o $@

# 添加 CaDiCaL 构建规则
$(CADICAL_LIB):
	cd $(CADICAL_DIR) && ./configure && make

# 安装规则（将静态库复制到主目录）
install: libnenofex.a
	@echo "Installing libnenofex.a to parent directory..."
	cp libnenofex.a ..
	@echo "Installation complete"

# 检查符号是否可见
check-symbols: libnenofex.a
	@echo "=== Checking exported symbols ==="
	@nm libnenofex.a | grep -E "nenofex_" || echo "No nenofex_* symbols found!"
	@echo "=== Done ==="

# 强制重新编译所有文件并生成静态库
rebuild: clean all check-symbols

clean:
	rm -f *.fpico *.out *.gcda *.gcno *.gcov *.o *.a nenofex

cleanall: clean
	rm -f ./nenofex *.so.$(VERSION) *.dylib

# 帮助信息
help:
	@echo "Available targets:"
	@echo "  make            - Build static library and executable"
	@echo "  make libnenofex.a - Build static library only"
	@echo "  make nenofex    - Build executable only"
	@echo "  make install    - Copy static library to parent directory"
	@echo "  make check-symbols - Check if nenofex symbols are exported"
	@echo "  make rebuild    - Clean and rebuild everything"
	@echo "  make clean      - Remove object files and library"
	@echo "  make cleanall   - Remove all generated files"