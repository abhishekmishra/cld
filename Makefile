CC=gcc
CFLAGS=-std=c11 `pkg-config --cflags json-c` `pkg-config --cflags libcurl`  `pkg-config --cflags libarchive` -I./src -fPIC
# To use colour logs...
#CFLAGS+= -DLOG_USE_COLOR
LIBS=`pkg-config --libs json-c` `pkg-config --libs libcurl`  `pkg-config --libs libarchive`
CLI_LIBS=-ledit# -ltermcap
LDFLAGS=-fPIC -shared
OUT_DIR=./bin/release
OBJ_DIR=./obj
TEST_OBJ_DIR=./obj
SRC_DIR=./src
TEST_DIR=./test
MKDIR_P=mkdir -p
debug: CFLAGS += -DDEBUG -g -O0
debug: OUT_DIR = ./bin/debug
debug: CC=gcc
test: OUT_DIR = ./bin/debug
test: CFLAGS += `pkg-config --cflags cmocka`
test: LIBS+= `pkg-config --libs cmocka`

.PHONY: directories

all: 		directories clibdocker.so cld
debug: 		directories clibdocker.so cld
test:		debug test_clibdocker
test_main:  debug clibdocker

## see https://stackoverflow.com/questions/1950926/create-directories-using-make-file
## for creating output directories
directories: $(OUT_DIR) $(OBJ_DIR)

$(OUT_DIR):
			$(MKDIR_P) $(OUT_DIR)

$(OBJ_DIR):
			$(MKDIR_P) $(OBJ_DIR)
			
# see https://www.gnu.org/software/make/manual/html_node/Wildcard-Function.html
# Get list of object files, with paths
OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/*.c))
			
clibdocker.so:	$(filter-out %cld.o, $(OBJECTS)) 
	$(CC) $(CFLAGS) -o $(OUT_DIR)/$@ $^ $(LIBS) $(LDFLAGS)
			
cld: $(OBJECTS)
	$(CC) $(CFLAGS) -o $(OUT_DIR)/$@ $^ $(LIBS) $(CLI_LIBS)
			
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@
	
$(TEST_OBJ_DIR)/test_%.o: $(TEST_DIR)/test_%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_OBJ_DIR)/main.o: $(TEST_DIR)/main.c
	$(CC) $(CFLAGS) -c $< -o $@
	
TEST_OBJECTS := $(patsubst $(TEST_DIR)/%.c,$(TEST_OBJ_DIR)/%.o,$(wildcard $(TEST_DIR)/*.c))

# Link with everything but main.o (because it contains another definition of main.	
test_clibdocker: $(filter-out %main.o, $(TEST_OBJECTS)) $(filter-out %cld.o, $(OBJECTS))
			$(CC) $(CFLAGS) -o $(OUT_DIR)/$@ $^ $(LIBS)
			./bin/debug/test_clibdocker

clibdocker:	$(OBJECTS) $(TEST_OBJ_DIR)/main.o
			$(CC) $(CFLAGS) -o $(OUT_DIR)/$@ $^ $(LIBS)

clean:
			rm -f ./obj/*.o ./bin/release/*
			rm -f ./obj/*.o ./bin/debug/*
			
